// XMLTools.cpp : Defines the initialization routines for the DLL.
//

// notepad++
#include "StdAfx.h"
#include "XMLTools.h"
#include "PluginInterface.h"
#include "Scintilla.h"

// dialogs
#include "InputDlg.h"
#include "XPathEvalDlg.h"
#include "SelectFileDlg.h"
#include "MessageDlg.h"
#include "XSLTransformDlg.h"
#include "HowtoUseDlg.h"
#include "OptionsDlg.h"
#include "DebugDlg.h"
#include "AboutBoxDlg.h"
#include "Report.h"

// other
#include <stack>
#include <shlwapi.h>
#include <shlobj.h>
#include <stdlib.h>
#include <direct.h>
#include <sstream>
#include <assert.h>
#include <locale>
#include <algorithm>
#include <array>
#include <map>

#include "MSXMLHelper.h"

//#define __XMLTOOLS_DEBUG__

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//
//  Note!
//
//    If this DLL is dynamically linked against the MFC
//    DLLs, any functions exported from this DLL which
//    call into MFC must have the AFX_MANAGE_STATE macro
//    added at the very beginning of the function.
//
//    For example:
//
//    extern "C" BOOL PASCAL EXPORT ExportedFunction()
//    {
//      AFX_MANAGE_STATE(AfxGetStaticModuleState());
//      // normal function body here
//    }
//
//    It is very important that this macro appear in each
//    function, prior to any calls into MFC.  This means that
//    it must appear as the first statement within the
//    function, even before any object variable declarations
//    as their constructors may generate calls into the MFC
//    DLL.
//
//    Please see MFC Technical Notes 33 and 58 for additional
//    details.
//

// This is the name which will be displayed in Plugins Menu
const wchar_t PLUGIN_NAME[] = L"XML Tools";
const wchar_t localConfFile[] = L"doLocalConf.xml";

// The number of functionality
#ifdef _DEBUG
  const int TOTAL_FUNCS = 32;
#else
  const int TOTAL_FUNCS = 31;
#endif
int nbFunc = TOTAL_FUNCS;

NppData nppData;
CDebugDlg* debugdlg = new CDebugDlg();
HHOOK hook = NULL;

// PATHs
wchar_t pluginHomePath[MAX_PATH] = { '\0' };
wchar_t pluginConfigPath[MAX_PATH] = { '\0' };
wchar_t iniFilePath[MAX_PATH] = { '\0' };
const wchar_t sectionName[] = L"XML Tools";

// Declaration of functionality (FuncItem) Array
FuncItem funcItem[TOTAL_FUNCS];
bool doCheckXML = false,
     doValidation = false,
     /*doPrettyPrint = false,*/
     doCloseTag = false,
     doAutoIndent = false,
     doAttrAutoComplete = false,
     doAutoXMLType = false,
     doPreventXXE = true,
     doAllowHuge = false,
     doPrettyPrintAllOpenFiles = false;

struct struct_proxyoptions proxyoptions;
struct struct_xmltoolsoptions xmltoolsoptions;

int menuitemCheckXML = -1,
    menuitemValidation = -1,
    /*menuitemPrettyPrint = -1,*/
    menuitemCloseTag = -1,
    menuitemAutoIndent = -1,
    menuitemAttrAutoComplete = -1,
    menuitemAutoXMLType = -1,
    menuitemPreventXXE = -1,
    menuitemAllowHuge = -1,
    menuitemPrettyPrintAllFiles = -1;

std::wstring lastXMLSchema(L"");

int nbopenfiles1, nbopenfiles2;
std::map<LRESULT,bool> hasAnnotations;

// Here're the declaration my functions ///////////////////////////////////////
bool hasCurrentDocAnnotations();

void insertXMLCheckTag();
void autoXMLCheck();
void manualXMLCheck();

void insertValidationTag();
void autoValidation();
void manualValidation();

void insertXMLCloseTag();
void closeXMLTag();

void insertTagAutoIndent();
void tagAutoIndent();

void insertAttributeAutoComplete();
void attributeAutoComplete();

bool setAutoXMLType();
void insertAutoXMLType();

void togglePreventXXE();
void toggleAllowHuge();

void prettyPrintXML();
void prettyPrintAttributes();
//void insertPrettyPrintTag();
void linearizeXML();
void togglePrettyPrintAllFiles();
int initDocIterator();
bool hasNextDoc(int* iter);

void getCurrentXPath(bool precise);
void getCurrentXPathStd();
void getCurrentXPathPredicate();
void evaluateXPath();

void performXSLTransform();

void convertXML2Text();
void convertText2XML();

void commentSelection();
void uncommentSelection();

void aboutBox();
void optionsDlg();
void howtoUse();
void updateProxyConfig();
void dbg(CStringW line);
void dbgln(CStringW line);
void debugDlg();

int performXMLCheck(int informIfNoError);
void initializePlugin();
void savePluginParams();

///////////////////////////////////////////////////////////////////////////////

void registerShortcut(FuncItem *item, bool enableALT, bool enableCTRL, bool enableSHIFT, unsigned char key) {
  if (!item) return;
  item->_pShKey = new ShortcutKey; // no parentheses needed as it's Plain Old Data (POD) otherwise C4345
  item->_pShKey->_isAlt = enableALT;
  item->_pShKey->_isCtrl = enableCTRL;
  item->_pShKey->_isShift = enableSHIFT;
  item->_pShKey->_key = key;
}

// get given language as string (for debug purposes only)
char* getLangType(LangType lg) {
  if (lg == L_TEXT        ) return "L_TEXT";
  if (lg == L_PHP         ) return "L_PHP";
  if (lg == L_C           ) return "L_C";
  if (lg == L_CPP         ) return "L_CPP";
  if (lg == L_CS          ) return "L_CS";
  if (lg == L_OBJC        ) return "L_OBJC";
  if (lg == L_JAVA        ) return "L_JAVA";
  if (lg == L_RC          ) return "L_RC";
  if (lg == L_HTML        ) return "L_HTML";
  if (lg == L_XML         ) return "L_XML";
  if (lg == L_MAKEFILE    ) return "L_MAKEFILE";
  if (lg == L_PASCAL      ) return "L_PASCAL";
  if (lg == L_BATCH       ) return "L_BATCH";
  if (lg == L_INI         ) return "L_INI";
  if (lg == L_ASCII       ) return "L_ASCII";
  if (lg == L_USER        ) return "L_USER";
  if (lg == L_ASP         ) return "L_ASP";
  if (lg == L_SQL         ) return "L_SQL";
  if (lg == L_VB          ) return "L_VB";
  if (lg == L_JS          ) return "L_JS";
  if (lg == L_CSS         ) return "L_CSS";
  if (lg == L_PERL        ) return "L_PERL";
  if (lg == L_PYTHON      ) return "L_PYTHON";
  if (lg == L_LUA         ) return "L_LUA";
  if (lg == L_TEX         ) return "L_TEX";
  if (lg == L_FORTRAN     ) return "L_FORTRAN";
  if (lg == L_BASH        ) return "L_BASH";
  if (lg == L_FLASH       ) return "L_FLASH";
  if (lg == L_NSIS        ) return "L_NSIS";
  if (lg == L_TCL         ) return "L_TCL";
  if (lg == L_LISP        ) return "L_LISP";
  if (lg == L_SCHEME      ) return "L_SCHEME";
  if (lg == L_ASM         ) return "L_ASM";
  if (lg == L_DIFF        ) return "L_DIFF";
  if (lg == L_CAML        ) return "L_CAML";
  if (lg == L_ADA         ) return "L_ADA";
  if (lg == L_VERILOG     ) return "L_VERILOG";
  if (lg == L_MATLAB      ) return "L_MATLAB";
  if (lg == L_HASKELL     ) return "L_HASKELL";
  if (lg == L_INNO        ) return "L_INNO";
  if (lg == L_SEARCHRESULT) return "L_SEARCHRESULT";
  if (lg == L_CMAKE       ) return "L_CMAKE";
  if (lg == L_YAML        ) return "L_YAML";
  if (lg == L_COBOL       ) return "L_COBOL";
  if (lg == L_GUI4CLI     ) return "L_GUI4CLI";
  if (lg == L_D           ) return "L_D";
  if (lg == L_POWERSHELL  ) return "L_POWERSHELL";
  if (lg == L_R           ) return "L_R";
  if (lg == L_JSP         ) return "L_JSP";
  if (lg == L_COFFEESCRIPT) return "L_COFFEESCRIPT";
  if (lg == L_EXTERNAL    ) return "L_EXTERNAL";

  return "";
}

/////////////////////////////////////////////////////////////////////////////
// CXMLToolsApp

BEGIN_MESSAGE_MAP(CXMLToolsApp, CWinApp)
  //{{AFX_MSG_MAP(CXMLToolsApp)
    // NOTE - the ClassWizard will add and remove mapping macros here.
    //    DO NOT EDIT what you see in these blocks of generated code!
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CXMLToolsApp construction

CXMLToolsApp::CXMLToolsApp() {
  dbgln("XML Tools plugin");
  dbg("version "); dbg(XMLTOOLS_VERSION_NUMBER); dbg(" "); dbgln(XMLTOOLS_VERSION_STATUS);
}

CXMLToolsApp::~CXMLToolsApp() {
  // Don't forget to de-allocate your shortcut here
  for (int i = 0; i < nbFunc; ++i) {
    if (funcItem[i]._pShKey) delete funcItem[i]._pShKey;
  }
}

void initializePlugin() {
  dbgln("initializePlugin()");

  dbg("Get plugin home dir... ");
  ::SendMessage(nppData._nppHandle, NPPM_GETPLUGINHOMEPATH, MAX_PATH, (LPARAM)pluginHomePath);
  PathAppend(pluginHomePath, L"\\XMLTools");
  //_chdir(Report::narrow(pluginHomePath).data());
  dbgln(pluginHomePath);

  dbg("Get plugin config dir... ");
  ::SendMessage(nppData._nppHandle, NPPM_GETPLUGINSCONFIGDIR, MAX_PATH, (LPARAM)pluginConfigPath);
  dbgln(pluginConfigPath);

  dbg("Locating XMLTools.ini... ");
  Report::strcpy(iniFilePath, pluginConfigPath);
  PathAppend(iniFilePath, L"\\XMLTools.ini");
  dbgln(iniFilePath);

  int menuentry = 0;
  for (int i = 0; i < nbFunc; ++i) {
    funcItem[i]._init2Check = false;
  }

  dbg("Building plugin menu entries... ");

  Report::strcpy(funcItem[menuentry]._itemName, L"Enable XML syntax auto-check");
  funcItem[menuentry]._pFunc = insertXMLCheckTag;
  funcItem[menuentry]._init2Check = (::GetPrivateProfileInt(sectionName, L"doCheckXML", 1, iniFilePath) != 0);
  doCheckXML = funcItem[menuentry]._init2Check;
  menuitemCheckXML = menuentry;
  ++menuentry;

  Report::strcpy(funcItem[menuentry]._itemName, L"Check XML syntax now");
  funcItem[menuentry]._pFunc = manualXMLCheck;
  ++menuentry;

  funcItem[menuentry++]._pFunc = NULL;  //----------------------------------------

  Report::strcpy(funcItem[menuentry]._itemName, L"Enable auto-validation");
  funcItem[menuentry]._pFunc = insertValidationTag;
  funcItem[menuentry]._init2Check = doValidation = (::GetPrivateProfileInt(sectionName, L"doValidation", 0, iniFilePath) != 0);
  doValidation = funcItem[menuentry]._init2Check;
  menuitemValidation = menuentry;
  ++menuentry;

  Report::strcpy(funcItem[menuentry]._itemName, L"Validate now");
  funcItem[menuentry]._pFunc = manualValidation;
  registerShortcut(funcItem + menuentry, true, true, true, 'M');
  ++menuentry;

  funcItem[menuentry++]._pFunc = NULL;  //----------------------------------------

  Report::strcpy(funcItem[menuentry]._itemName, L"Tag auto-close");
  funcItem[menuentry]._pFunc = insertXMLCloseTag;
  funcItem[menuentry]._init2Check = doCloseTag = (::GetPrivateProfileInt(sectionName, L"doCloseTag", 1, iniFilePath) != 0);
  doCloseTag = funcItem[menuentry]._init2Check;
  menuitemCloseTag = menuentry;
  ++menuentry;
  /*
  Report::strcpy(funcItem[menuentry]._itemName, L"Tag auto-indent");
  funcItem[menuentry]._pFunc = insertTagAutoIndent;
  funcItem[menuentry]._init2Check = doAutoIndent = (::GetPrivateProfileInt(sectionName, L"doAutoIndent", 0, iniFilePath) != 0);
  doAutoIndent = funcItem[menuentry]._init2Check;
  menuitemAutoIndent = menuentry;
  ++menuentry;

  Report::strcpy(funcItem[menuentry]._itemName, L"Auto-complete attributes");
  funcItem[menuentry]._pFunc = insertAttributeAutoComplete;
  funcItem[menuentry]._init2Check = doAttrAutoComplete = (::GetPrivateProfileInt(sectionName, L"doAttrAutoComplete", 0, iniFilePath) != 0);
  doAttrAutoComplete = funcItem[menuentry]._init2Check;
  menuitemAttrAutoComplete = menuentry;
  ++menuentry;
  */
  funcItem[menuentry++]._pFunc = NULL;  //----------------------------------------

  Report::strcpy(funcItem[menuentry]._itemName, L"Set XML type automatically");
  funcItem[menuentry]._pFunc = insertAutoXMLType;
  funcItem[menuentry]._init2Check = doAutoXMLType = (::GetPrivateProfileInt(sectionName, L"doAutoXMLType", 1, iniFilePath) != 0);
  doAutoXMLType = funcItem[menuentry]._init2Check;
  menuitemAutoXMLType = menuentry;
  ++menuentry;

  Report::strcpy(funcItem[menuentry]._itemName, L"Prevent XXE");
  funcItem[menuentry]._pFunc = togglePreventXXE;
  funcItem[menuentry]._init2Check = doPreventXXE = (::GetPrivateProfileInt(sectionName, L"doPreventXXE", 1, iniFilePath) != 0);
  doPreventXXE = funcItem[menuentry]._init2Check;
  menuitemPreventXXE = menuentry;
  ++menuentry;

  Report::strcpy(funcItem[menuentry]._itemName, L"Allow huge files");
  funcItem[menuentry]._pFunc = toggleAllowHuge;
  funcItem[menuentry]._init2Check = doAllowHuge = (::GetPrivateProfileInt(sectionName, L"doAllowHuge", 0, iniFilePath) != 0);
  doAllowHuge = funcItem[menuentry]._init2Check;
  menuitemAllowHuge = menuentry;
  ++menuentry;

  funcItem[menuentry++]._pFunc = NULL;  //----------------------------------------

  Report::strcpy(funcItem[menuentry]._itemName, L"Pretty print");
  registerShortcut(funcItem + menuentry, true, true, true, 'B');
  funcItem[menuentry]._pFunc = prettyPrintXML;
  ++menuentry;

  Report::strcpy(funcItem[menuentry]._itemName, L"Pretty print (indent attributes)");
  registerShortcut(funcItem + menuentry, true, true, true, 'A');
  funcItem[menuentry]._pFunc = prettyPrintAttributes;
  ++menuentry;
  /*
  Report::strcpy(funcItem[menuentry]._itemName, L"Enable auto pretty print (libXML) [experimental]");
  funcItem[menuentry]._pFunc = insertPrettyPrintTag;
  funcItem[menuentry]._init2Check = doPrettyPrint = (::GetPrivateProfileInt(sectionName, L"doPrettyPrint", 0, iniFilePath) != 0);
  doPrettyPrint = funcItem[menuentry]._init2Check;
  menuitemPrettyPrint = menuentry;
  ++menuentry;
  */
  Report::strcpy(funcItem[menuentry]._itemName, L"Linearize XML");
  registerShortcut(funcItem + menuentry, true, true, true, 'L');
  funcItem[menuentry]._pFunc = linearizeXML;
  ++menuentry;

  Report::strcpy(funcItem[menuentry]._itemName, L"Apply to all open files");
  funcItem[menuentry]._pFunc = togglePrettyPrintAllFiles;
  funcItem[menuentry]._init2Check = (::GetPrivateProfileInt(sectionName, L"doPrettyPrintAllOpenFiles", 0, iniFilePath) != 0);
  doPrettyPrintAllOpenFiles = funcItem[menuentry]._init2Check;
  menuitemPrettyPrintAllFiles = menuentry;
  ++menuentry;

  funcItem[menuentry++]._pFunc = NULL;  //----------------------------------------

  Report::strcpy(funcItem[menuentry]._itemName, L"Current XML Path");
  registerShortcut(funcItem + menuentry, true, true, true, 'P');
  funcItem[menuentry]._pFunc = getCurrentXPathStd;
  ++menuentry;

  Report::strcpy(funcItem[menuentry]._itemName, L"Current XML Path with predicates");
  funcItem[menuentry]._pFunc = getCurrentXPathPredicate;
  ++menuentry;

  Report::strcpy(funcItem[menuentry]._itemName, L"Evaluate XPath expression...");
  funcItem[menuentry]._pFunc = evaluateXPath;
  ++menuentry;

  funcItem[menuentry++]._pFunc = NULL;  //----------------------------------------

  Report::strcpy(funcItem[menuentry]._itemName, L"XSL Transformation...");
  funcItem[menuentry]._pFunc = performXSLTransform;
  ++menuentry;

  funcItem[menuentry++]._pFunc = NULL;  //----------------------------------------

  Report::strcpy(funcItem[menuentry]._itemName, L"Escape characters in selection (<> → &&lt;&&gt;)");
  funcItem[menuentry]._pFunc = convertXML2Text;
  ++menuentry;

  Report::strcpy(funcItem[menuentry]._itemName, L"Unescape characters in selection (&&lt;&&gt; → <>)");
  funcItem[menuentry]._pFunc = convertText2XML;
  ++menuentry;

  funcItem[menuentry++]._pFunc = NULL;  //----------------------------------------

  Report::strcpy(funcItem[menuentry]._itemName, L"Comment selection");
  registerShortcut(funcItem + menuentry, true, true, true, 'C');
  funcItem[menuentry]._pFunc = commentSelection;
  ++menuentry;

  Report::strcpy(funcItem[menuentry]._itemName, L"Uncomment selection");
  registerShortcut(funcItem + menuentry, true, true, true, 'R');
  funcItem[menuentry]._pFunc = uncommentSelection;
  ++menuentry;

  funcItem[menuentry++]._pFunc = NULL;  //----------------------------------------

  Report::strcpy(funcItem[menuentry]._itemName, L"Options...");
  funcItem[menuentry]._pFunc = optionsDlg;
  ++menuentry;

#ifdef _DEBUG
  Report::strcpy(funcItem[menuentry]._itemName, L"Debug window...");
  funcItem[menuentry]._pFunc = debugDlg;
  ++menuentry;
#endif

  Report::strcpy(funcItem[menuentry]._itemName, L"About XML Tools / Donate...");
  funcItem[menuentry]._pFunc = aboutBox;
  ++menuentry;

  CoInitialize(NULL);

  dbgln("done.");

  // Load proxy settings and xml features in ini file
  proxyoptions.status = (::GetPrivateProfileInt(sectionName, L"proxyEnabled", 0, iniFilePath) == 1);
  ::GetPrivateProfileString(sectionName, L"proxyHost", L"192.168.0.1", proxyoptions.host, 255, iniFilePath);
  proxyoptions.port = ::GetPrivateProfileInt(sectionName, L"proxyPort", 8080, iniFilePath);
  ::GetPrivateProfileString(sectionName, L"proxyUser", L"", proxyoptions.username, 255, iniFilePath);
  ::GetPrivateProfileString(sectionName, L"proxyPass", L"", proxyoptions.password, 255, iniFilePath);
  xmltoolsoptions.prohibitDTD = (::GetPrivateProfileInt(sectionName, L"prohibitDTD", 0, iniFilePath) == 1);
  xmltoolsoptions.useAnnotations = (::GetPrivateProfileInt(sectionName, L"useAnnotations", 1, iniFilePath) == 1);
  xmltoolsoptions.annotationStyle = ::GetPrivateProfileInt(sectionName, L"annotationStyle", 12, iniFilePath);
  xmltoolsoptions.convertAmp = (::GetPrivateProfileInt(sectionName, L"convertAmp", 1, iniFilePath) == 1);
  xmltoolsoptions.convertLt = (::GetPrivateProfileInt(sectionName, L"convertLt", 1, iniFilePath) == 1);
  xmltoolsoptions.convertGt = (::GetPrivateProfileInt(sectionName, L"convertGt", 1, iniFilePath) == 1);
  xmltoolsoptions.convertQuote = (::GetPrivateProfileInt(sectionName, L"convertQuote", 1, iniFilePath) == 1);
  xmltoolsoptions.convertApos = (::GetPrivateProfileInt(sectionName, L"convertApos", 1, iniFilePath) == 1);
  xmltoolsoptions.ppAutoclose = (::GetPrivateProfileInt(sectionName, L"ppAutoclose", 1, iniFilePath) == 1);

  updateProxyConfig();

  assert(menuentry == nbFunc);

  //Report::_printf_inf("menu entries: %d", menuentry);

  /*
  Report::_printf_inf("%s\ndoCheckXML: %d %d\ndoValidation: %d %d\ndoCloseTag: %d %d\ndoAutoXMLType: %d %d\ndoPreventXXE: %d %d\ndoAllowHuge: %d %d\nisLocal: %d",
  iniFilePath,
  doCheckXML, funcItem[menuitemCheckXML]._init2Check,
  doValidation, funcItem[menuitemValidation]._init2Check,
  doCloseTag, funcItem[menuitemCloseTag]._init2Check,
  doAutoXMLType, funcItem[menuitemAutoXMLType]._init2Check,
  doPreventXXE, funcItem[menuitemPreventXXE]._init2Check,
  doAllowHuge, funcItem[menuitemAllowHuge]._init2Check,
  isLocal);
  */

  dbgln("Initialization finished.");
  dbgln("");
}

void savePluginParams() {
  dbgln("savePluginParams()");

  funcItem[menuitemCheckXML]._init2Check = doCheckXML;
  funcItem[menuitemValidation]._init2Check = doValidation;
  //funcItem[menuitemPrettyPrint]._init2Check = doPrettyPrint;
  funcItem[menuitemCloseTag]._init2Check = doCloseTag;
  //funcItem[menuitemAutoIndent]._init2Check = doAutoIndent;
  //funcItem[menuitemAttrAutoComplete]._init2Check = doAttrAutoComplete;
  funcItem[menuitemAutoXMLType]._init2Check = doAutoXMLType;
  funcItem[menuitemPreventXXE]._init2Check = doPreventXXE;
  funcItem[menuitemAllowHuge]._init2Check = doAllowHuge;
  funcItem[menuitemPrettyPrintAllFiles]._init2Check = doPrettyPrintAllOpenFiles;

  ::WritePrivateProfileString(sectionName, L"doCheckXML", doCheckXML?L"1":L"0", iniFilePath);
  ::WritePrivateProfileString(sectionName, L"doValidation", doValidation?L"1":L"0", iniFilePath);
  //::WritePrivateProfileString(sectionName, L"doPrettyPrint", doPrettyPrint?L"1":L"0", iniFilePath);
  ::WritePrivateProfileString(sectionName, L"doCloseTag", doCloseTag?L"1":L"0", iniFilePath);
  //::WritePrivateProfileString(sectionName, L"doAutoIndent", doAutoIndent?L"1":L"0", iniFilePath);
  //::WritePrivateProfileString(sectionName, L"doAttrAutoComplete", doAttrAutoComplete?L"1":L"0", iniFilePath);
  ::WritePrivateProfileString(sectionName, L"doAutoXMLType", doAutoXMLType?L"1":L"0", iniFilePath);
  ::WritePrivateProfileString(sectionName, L"doPreventXXE", doPreventXXE?L"1":L"0", iniFilePath);
  ::WritePrivateProfileString(sectionName, L"doAllowHuge", doAllowHuge?L"1" : L"0", iniFilePath);
  ::WritePrivateProfileString(sectionName, L"doPrettyPrintAllOpenFiles", doPrettyPrintAllOpenFiles?L"1":L"0", iniFilePath);
}


HMODULE GetCurrentModule() {
  HMODULE hModule = NULL;
  GetModuleHandleEx(
    GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS,
    (LPCTSTR)GetCurrentModule,
    &hModule);

  return hModule;
}

static LRESULT CALLBACK KeyboardProc(int ncode, WPARAM wparam, LPARAM lparam) {
  if (ncode == HC_ACTION && wparam == VK_ESCAPE) {
    clearAnnotations();
  }

  return CallNextHookEx(hook, ncode, wparam, lparam); // pass control to next hook in the hook chain
}

/*
 *--------------------------------------------------
 * The 4 extern functions are mandatory
 * They will be called by Notepad++ plugins system
 *--------------------------------------------------
*/

// The setInfo function gets the needed infos from Notepad++ plugins system
extern "C" __declspec(dllexport) void setInfo(NppData notpadPlusData) {
  nppData = notpadPlusData;

  initializePlugin();
}

// The getName function tells Notepad++ plugins system its name
extern "C" __declspec(dllexport) const TCHAR* getName() {
  return PLUGIN_NAME;
}

// The getFuncsArray function gives Notepad++ plugins system the pointer FuncItem Array
// and the size of this array (the number of functions)
extern "C" __declspec(dllexport) FuncItem * getFuncsArray(int *nbF) {
  *nbF = nbFunc;
  return funcItem;
}

// For v.3.3 compatibility
extern "C" __declspec(dllexport) LRESULT messageProc(UINT Message, WPARAM wParam, LPARAM lParam) {
  return TRUE;
}

HWND getCurrentHScintilla(int which) {
  return (which == 0)?nppData._scintillaMainHandle:nppData._scintillaSecondHandle;
};

// If you don't need get the notification from Notepad++, just let it be empty.
extern "C" __declspec(dllexport) void beNotified(SCNotification *notifyCode) {
  switch (notifyCode->nmhdr.code) {
    case NPPN_READY: {
      dbgln("NPP Event: NPPN_READY");
      HMENU hMenu = ::GetMenu(nppData._nppHandle);
      if (hMenu) {
        ::CheckMenuItem(hMenu, funcItem[menuitemCheckXML]._cmdID, MF_BYCOMMAND | (doCheckXML?MF_CHECKED:MF_UNCHECKED));
        ::CheckMenuItem(hMenu, funcItem[menuitemValidation]._cmdID, MF_BYCOMMAND | (doValidation?MF_CHECKED:MF_UNCHECKED));
//      ::CheckMenuItem(hMenu, funcItem[menuitemPrettyPrint]._cmdID, MF_BYCOMMAND | (doPrettyPrint?MF_CHECKED:MF_UNCHECKED));
        ::CheckMenuItem(hMenu, funcItem[menuitemCloseTag]._cmdID, MF_BYCOMMAND | (doCloseTag?MF_CHECKED:MF_UNCHECKED));
//      ::CheckMenuItem(hMenu, funcItem[menuitemAutoIndent]._cmdID, MF_BYCOMMAND | (doAutoIndent?MF_CHECKED:MF_UNCHECKED));
//      ::CheckMenuItem(hMenu, funcItem[menuitemAttrAutoComplete]._cmdID, MF_BYCOMMAND | (doAttrAutoComplete?MF_CHECKED:MF_UNCHECKED));
        ::CheckMenuItem(hMenu, funcItem[menuitemAutoXMLType]._cmdID, MF_BYCOMMAND | (doAutoXMLType?MF_CHECKED:MF_UNCHECKED));
        ::CheckMenuItem(hMenu, funcItem[menuitemPreventXXE]._cmdID, MF_BYCOMMAND | (doPreventXXE?MF_CHECKED:MF_UNCHECKED));
        ::CheckMenuItem(hMenu, funcItem[menuitemAllowHuge]._cmdID, MF_BYCOMMAND | (doAllowHuge ? MF_CHECKED : MF_UNCHECKED));
        ::CheckMenuItem(hMenu, funcItem[menuitemPrettyPrintAllFiles]._cmdID, MF_BYCOMMAND | (doPrettyPrintAllOpenFiles?MF_CHECKED:MF_UNCHECKED));

        if (hook) {
          UnhookWindowsHookEx(hook);
          hook = NULL;
        }
        else {
          hook = SetWindowsHookEx(WH_KEYBOARD, KeyboardProc, (HINSTANCE)GetCurrentModule(), ::GetCurrentThreadId());
        }

        #ifdef DEBUG
          debugdlg->Create(CDebugDlg::IDD,NULL);
        #endif
      }
    }
    case NPPN_FILEBEFORESAVE: {
      dbgln("NPP Event: NPPN_FILEBEFORESAVE");
      LangType docType;
      ::SendMessage(nppData._nppHandle, NPPM_GETCURRENTLANGTYPE, 0, (LPARAM)&docType);
      if (docType == L_XML) {
        // comme la validation XSD effectue également un check de syntaxe, on n'exécute
        // le autoXMLCheck() que si doValidation est FALSE et doCheckXML est TRUE.
        if (doValidation) autoValidation();
        else if (doCheckXML) autoXMLCheck();
      }
      break;
    }
    case SCN_CHARADDED: {
      dbgln("NPP Event: SCN_CHARADDED");
      LangType docType;
      ::SendMessage(nppData._nppHandle, NPPM_GETCURRENTLANGTYPE, 0, (LPARAM)&docType);
      if (doAutoXMLType && docType != L_XML) {
        if (setAutoXMLType()) {
          docType = L_XML;
        }
      }
      if (docType == L_XML) {
        // remarque: le closeXMLTag doit s'exécuter avant les autres
        if (doCloseTag && notifyCode->ch == '>') {
          closeXMLTag();
        }
        //if (doAutoIndent && lastChar == '\n') tagAutoIndent();
        //if (doAttrAutoComplete && lastChar == '\"') attributeAutoComplete();
      }

      break;
    }
    case SCN_MODIFIED: {
      if ((notifyCode->modificationType == SC_MOD_INSERTTEXT || notifyCode->modificationType == SC_MOD_DELETETEXT) && hasCurrentDocAnnotations()) {
        dbgln(Report::str_format("NPP Event: SCN_MODIFIED [%d]", notifyCode->modificationType).c_str());
        clearAnnotations();
      }
      break;
    }
    case NPPN_FILEOPENED: {
      dbgln("NPP Event: NPPN_FILEOPENED");
      TCHAR filename[MAX_PATH];
      ::SendMessage(nppData._nppHandle, NPPM_GETFULLPATHFROMBUFFERID, notifyCode->nmhdr.idFrom, reinterpret_cast<LPARAM>(filename));
      dbg("  bufferID: "); dbgln(std::to_string(static_cast<unsigned long long>(notifyCode->nmhdr.idFrom)).c_str());
      dbg("  filename: "); dbgln(filename);
      break;
    }
    case NPPN_BUFFERACTIVATED: {
      dbgln("NPP Event: NPPN_BUFFERACTIVATED");
      if (doAutoXMLType) {
        // si le fichier n'a pas de type défini et qu'il commence par "<?xml ", on lui attribue le type L_XML
        LangType docType = L_EXTERNAL;
        ::SendMessage(nppData._nppHandle, NPPM_GETCURRENTLANGTYPE, 0, (LPARAM)&docType);
        dbg("  Current langtype: "); dbgln(std::to_string(static_cast<unsigned long long>(docType)).c_str());
        //Report::_printf_inf("%s", getLangType(docType));
        if (docType != L_XML) {
          setAutoXMLType();
        }
      }
      break;
    }
    case NPPN_SHUTDOWN: {
      CoUninitialize();

      hasAnnotations.clear();

      savePluginParams();
      break;
    }
    default: {
      //dbg("NPP Event: "); dbgln(std::to_string(notifyCode->nmhdr.code).c_str());
    }
  }
}



#ifdef UNICODE
  extern "C" __declspec(dllexport) BOOL isUnicode() {
    return TRUE;
  }
#endif //UNICODE

void insertXMLCheckTag() {
  dbgln("insertXMLCheckTag()");

  doCheckXML = !doCheckXML;
  ::CheckMenuItem(::GetMenu(nppData._nppHandle), funcItem[menuitemCheckXML]._cmdID, MF_BYCOMMAND | (doCheckXML?MF_CHECKED:MF_UNCHECKED));
  savePluginParams();
}

void insertValidationTag() {
  dbgln("insertValidationTag()");

  doValidation = !doValidation;
  ::CheckMenuItem(::GetMenu(nppData._nppHandle), funcItem[menuitemValidation]._cmdID, MF_BYCOMMAND | (doValidation?MF_CHECKED:MF_UNCHECKED));
  savePluginParams();
}

/*
void insertPrettyPrintTag() {
  dbgln("insertPrettyPrintTag()");

  doPrettyPrint = !doPrettyPrint;
  ::CheckMenuItem(::GetMenu(nppData._nppHandle), funcItem[menuitemPrettyPrint]._cmdID, MF_BYCOMMAND | (doPrettyPrint?MF_CHECKED:MF_UNCHECKED));
  savePluginParams();
}
*/

void insertXMLCloseTag() {
  dbgln("insertXMLCloseTag()");

  doCloseTag = !doCloseTag;
  ::CheckMenuItem(::GetMenu(nppData._nppHandle), funcItem[menuitemCloseTag]._cmdID, MF_BYCOMMAND | (doCloseTag?MF_CHECKED:MF_UNCHECKED));
  savePluginParams();
}

bool tagAutoIndentWarningDisplayed = false;
void insertTagAutoIndent() {
  dbgln("insertTagAutoIndent()");

  if (!tagAutoIndentWarningDisplayed) {
    Report::_printf_inf(L"This function is in alpha state and might disappear in future release.");
    tagAutoIndentWarningDisplayed = true;
  }

  doAutoIndent = !doAutoIndent;
  ::CheckMenuItem(::GetMenu(nppData._nppHandle), funcItem[menuitemAutoIndent]._cmdID, MF_BYCOMMAND | (doAutoIndent?MF_CHECKED:MF_UNCHECKED));
  savePluginParams();
}

bool insertAttributeAutoCompleteWarningDisplayed = false;
void insertAttributeAutoComplete() {
  dbgln("insertAttributeAutoComplete()");

  if (!insertAttributeAutoCompleteWarningDisplayed) {
    Report::_printf_inf(L"This function is in alpha state and might disappear in future release.");
    insertAttributeAutoCompleteWarningDisplayed = true;
  }

  doAttrAutoComplete = !doAttrAutoComplete;
  ::CheckMenuItem(::GetMenu(nppData._nppHandle), funcItem[menuitemAttrAutoComplete]._cmdID, MF_BYCOMMAND | (doAttrAutoComplete?MF_CHECKED:MF_UNCHECKED));
  savePluginParams();
}

void insertAutoXMLType() {
  dbgln("insertAutoXMLType()");

  doAutoXMLType = !doAutoXMLType;
  ::CheckMenuItem(::GetMenu(nppData._nppHandle), funcItem[menuitemAutoXMLType]._cmdID, MF_BYCOMMAND | (doAutoXMLType?MF_CHECKED:MF_UNCHECKED));
  savePluginParams();
}

void togglePreventXXE() {
  dbgln("togglePreventXXE()");

  doPreventXXE = !doPreventXXE;
  ::CheckMenuItem(::GetMenu(nppData._nppHandle), funcItem[menuitemPreventXXE]._cmdID, MF_BYCOMMAND | (doPreventXXE?MF_CHECKED:MF_UNCHECKED));
  savePluginParams();
}

void toggleAllowHuge() {
  dbgln("toggleAllowHuge()");

  doAllowHuge = !doAllowHuge;
  ::CheckMenuItem(::GetMenu(nppData._nppHandle), funcItem[menuitemAllowHuge]._cmdID, MF_BYCOMMAND | (doAllowHuge ? MF_CHECKED : MF_UNCHECKED));
  savePluginParams();
}

void optionsDlg() {
  dbgln("optionsDlg()");

  COptionsDlg* dlg = new COptionsDlg(NULL);
  if (dlg->DoModal() == IDOK) {
    ::WritePrivateProfileString(sectionName, L"proxyEnabled", proxyoptions.status?L"1":L"0", iniFilePath);
    ::WritePrivateProfileString(sectionName, L"proxyHost", proxyoptions.host, iniFilePath);
    ::WritePrivateProfileString(sectionName, L"proxyPort", std::to_wstring(static_cast<long>(proxyoptions.port)).c_str(), iniFilePath);
    ::WritePrivateProfileString(sectionName, L"proxyUser", proxyoptions.username, iniFilePath);
    ::WritePrivateProfileString(sectionName, L"proxyPass", proxyoptions.password, iniFilePath);
    ::WritePrivateProfileString(sectionName, L"prohibitDTD", xmltoolsoptions.prohibitDTD ? L"1" : L"0", iniFilePath);
    ::WritePrivateProfileString(sectionName, L"useAnnotations", xmltoolsoptions.useAnnotations ? L"1" : L"0", iniFilePath);
    ::WritePrivateProfileString(sectionName, L"annotationStyle", std::to_wstring(static_cast<int>(xmltoolsoptions.annotationStyle)).c_str(), iniFilePath);
    ::WritePrivateProfileString(sectionName, L"convertAmp", xmltoolsoptions.convertAmp ? L"1" : L"0", iniFilePath);
    ::WritePrivateProfileString(sectionName, L"convertLt", xmltoolsoptions.convertLt ? L"1" : L"0", iniFilePath);
    ::WritePrivateProfileString(sectionName, L"convertGt", xmltoolsoptions.convertGt ? L"1" : L"0", iniFilePath);
    ::WritePrivateProfileString(sectionName, L"convertQuote", xmltoolsoptions.convertQuote ? L"1" : L"0", iniFilePath);
    ::WritePrivateProfileString(sectionName, L"convertApos", xmltoolsoptions.convertApos ? L"1" : L"0", iniFilePath);
    ::WritePrivateProfileString(sectionName, L"ppAutoclose", xmltoolsoptions.ppAutoclose ? L"1" : L"0", iniFilePath);

    updateProxyConfig();
  }
}

void debugDlg() {
  debugdlg->ShowWindow(SW_SHOW);
}

void dbg(CStringW line) {
  #ifdef DEBUG
    debugdlg->addLine(line);
  #endif
}

void dbgln(CStringW line) {
  #ifdef DEBUG
    debugdlg->addLine(line+"\r\n");
  #endif
}

void updateProxyConfig() {
  dbgln("updateProxyConfig()");

  // proxy settings for libxml
  if (proxyoptions.status && wcslen(proxyoptions.host) > 0) {
    std::string proxyurl("http://");
/*
    if (wcslen(proxyoptions.username) > 0) {
      proxyurl += Report::wchar2char(proxyoptions.username);

      if (wcslen(proxyoptions.password) > 0) {
        proxyurl += ":";
        proxyurl += Report::wchar2char(proxyoptions.password);
      }
      proxyurl += "@";
    }
*/

    proxyurl += Report::wchar2char(proxyoptions.host);
    proxyurl += ":";
    proxyurl += std::to_string(static_cast<long long>(proxyoptions.port));

    // v3
    //pXmlNanoHTTPScanProxy(proxyurl.c_str());  // http://toto:admin@127.0.0.1:8080
  } else {
    // v3
    //pXmlNanoHTTPScanProxy(NULL);
  }
}

void aboutBox() {
  dbgln("aboutBox()");

  CAboutBoxDlg* dlg = new CAboutBoxDlg();
  dlg->DoModal();
}

void howtoUse() {
  dbgln("howtoUse()");

  CHowtoUseDlg* dlg = new CHowtoUseDlg();
  dlg->DoModal();
}

///////////////////////////////////////////////////////////////////////////////

// tricky way of centering text on given vertical and horizontal position
// for vertical position, we get the contol size and estimate the number
// of lines based on text height then we force centering by going to wanted
// line +- nlines/2
// for horizontal position, we calculate the text width and check if it is
// out of the bounding box; then scroll horizontally if required
void centerOnPosition(HWND view, size_t line, size_t hofs, size_t wofs, const char* text) {
  try {
    RECT rect;
    GetClientRect(view, &rect);
    int height = (int) ::SendMessage(view, SCI_TEXTHEIGHT, line, NULL);
    int last = (int) ::SendMessage(view, SCI_GETMAXLINESTATE, NULL, NULL);
    size_t nlines_2 = (rect.bottom - rect.top) / (2 * height);

    // force uncollapse target line and ensure line is visible
    ::SendMessage(view, SCI_ENSUREVISIBLE, line - 1, NULL);
    ::SendMessage(view, SCI_SETFIRSTVISIBLELINE, line - 1, NULL);

    // center on line
    if (line - 1 + nlines_2 > (size_t) last) {
      // line is on the end of document
      ::SendMessage(view, SCI_SHOWLINES, line - 1, line + hofs); // force surround lines visibles if folded
    } else {
     ::SendMessage(view, SCI_GOTOLINE, line - 1 - nlines_2, 0);
     ::SendMessage(view, SCI_GOTOLINE, line - 1 + nlines_2, 0);
    }

    ::SendMessage(view, SCI_GOTOLINE, line - 1, 0);

    // center on column
    if (text != NULL) {
      size_t width = (size_t) ::SendMessage(view, SCI_TEXTWIDTH, 32, reinterpret_cast<LPARAM>(text));
      size_t margins = (size_t) ::SendMessage(view, SCI_GETMARGINS, 0, 0);
      size_t wmargin = 0;
      for (size_t i = 0; i < margins; ++i) {
        wmargin += (size_t) ::SendMessage(view, SCI_GETMARGINWIDTHN, i, 0);
      }

      if (width + wofs > (rect.right - rect.left - wmargin)) {
        // error message goes out of the bounding box
        size_t scroll = width + wofs - (rect.right - rect.left - wmargin);
        ::SendMessage(view, SCI_SETXOFFSET, (int) scroll, 0);
      }
    }

  } catch (...) {}
}

void displayXMLError(std::wstring wmsg, HWND view, size_t line, size_t linepos, size_t filepos) {
  // clear final \r\n
  std::string::size_type p = wmsg.find_last_not_of(L"\r\n");
  if (p != std::string::npos && p < wmsg.length()) {
    wmsg.erase(p+1, std::string::npos);
  }

  if (view == NULL) {
    int currentEdit;
    ::SendMessage(nppData._nppHandle, NPPM_GETCURRENTSCINTILLA, 0, (LPARAM)&currentEdit);
    view = getCurrentHScintilla(currentEdit);
  }

  LRESULT bufferid = ::SendMessage(nppData._nppHandle, NPPM_GETCURRENTBUFFERID, 0, 0);
  UniMode encoding = UniMode(::SendMessage(nppData._nppHandle, NPPM_GETBUFFERENCODING, bufferid, 0));

  if (xmltoolsoptions.useAnnotations) {
    if (line == NULL) {
      line = (size_t) ::SendMessage(nppData._nppHandle, NPPM_GETCURRENTLINE, 0, 0) + 1;
    }

    int maxannotwidth = 0;
    char* buffer = NULL;
    if (linepos != NULL) {
      if (SendMessage(view, SCI_GETWRAPMODE, 0, 0) == 0) {
        size_t linelen = (size_t) ::SendMessage(view, SCI_LINELENGTH, line - 1, 0);
        buffer = new char[linelen + sizeof(char)];
        memset(buffer, '\0', linelen + sizeof(char));
        ::SendMessage(view, SCI_GETLINE, line - 1, reinterpret_cast<LPARAM>(buffer));

        // add spaces before each line to align the annotation on linepos
        // @todo: scroll horizontally if necessary to make annotation visible in current view
        if (linepos <= linelen) {
          size_t i;
          std::wstring tabs, spaces;

          // calculate tabs width
          int tabwidth = (int) ::SendMessage(view, SCI_GETTABWIDTH, 0, 0);
          if (tabwidth <= 0) tabwidth = 4;
          for (int i = 0; i < tabwidth; ++i) tabs += ' ';

          // replace all char except tabs with space
          for (i = 0; i < linepos - 1; ++i) {
            if (buffer[i] == '\t') spaces += tabs;
            else spaces += ' ';
          }
          buffer[linepos - 1] = '\0'; // force buffer end (required for horizontal scrolling)

          tabs.clear();

          wmsg.insert(0, spaces);
          std::string::size_type oldpos = spaces.length(), pos = wmsg.find(L"\r\n");
          int tmpannotwidth;
          while (pos != std::string::npos) {
            tmpannotwidth = (int) ::SendMessage(view, SCI_TEXTWIDTH, 32, reinterpret_cast<LPARAM>(Report::castChar(wmsg.substr(oldpos, pos - oldpos), encoding).c_str()));
            if (tmpannotwidth > maxannotwidth) maxannotwidth = tmpannotwidth;

            pos += lstrlen(L"\r\n");
            wmsg.insert(pos, spaces);
            oldpos = pos + spaces.length();
            pos = wmsg.find(L"\r\n", pos);
          }

          // calculate last line
          tmpannotwidth = (int) ::SendMessage(view, SCI_TEXTWIDTH, 32, reinterpret_cast<LPARAM>(Report::castChar(wmsg.substr(oldpos, wmsg.length() - oldpos), encoding).c_str()));
          if (tmpannotwidth > maxannotwidth) maxannotwidth = tmpannotwidth;

          spaces.clear();
        }
      }
    }

    // display error as an annotation

    //char* styles = new char[wmsg.length()];
    //memset(styles, xmltoolsoptions.annotationStyle, wmsg.length());
    //memset(styles, 0, wmsg.find(L"\r\n")+1);

    ::SendMessage(view, SCI_ANNOTATIONSETTEXT, line - 1, reinterpret_cast<LPARAM>(Report::castChar(wmsg, encoding).c_str()));

    //::SendMessage(view, SCI_ANNOTATIONSETSTYLES, line - 1, reinterpret_cast<LPARAM>(styles));
    ::SendMessage(view, SCI_ANNOTATIONSETSTYLE, line - 1, xmltoolsoptions.annotationStyle);
    ::SendMessage(view, SCI_ANNOTATIONSETVISIBLE, 1, NULL);

    hasAnnotations[::SendMessage(nppData._nppHandle, NPPM_GETCURRENTBUFFERID, 0, 0)] = true;

    centerOnPosition(view, line, std::count(wmsg.begin(), wmsg.end(), '\n'), maxannotwidth, buffer);

    if (buffer != NULL) delete[] buffer;

    if (filepos != NULL) {
      ::SendMessage(view, SCI_GOTOPOS, filepos - 1, 0);
    }
  } else {
    Report::_printf_err(wmsg);
  }
}

void displayXMLError(IXMLDOMParseError* pXMLErr, HWND view, const wchar_t* szDesc) {
  HRESULT hr = S_OK;
  long line = 0;
  long linepos = 0;
  long filepos = 0;
  BSTR bstrReason = NULL;

  CHK_HR(pXMLErr->get_line(&line));
  CHK_HR(pXMLErr->get_linepos(&linepos));
  CHK_HR(pXMLErr->get_filepos(&filepos));
  CHK_HR(pXMLErr->get_reason(&bstrReason));

  if (szDesc != NULL) {
    displayXMLError(Report::str_format(L"%s - line %d, pos %d: \r\n%s", szDesc, line, linepos, bstrReason), view, line, linepos, filepos+1);
  } else {
    displayXMLError(Report::str_format(L"XML Parsing error - line %d, pos %d: \r\n%s", line, linepos, bstrReason), view, line, linepos, filepos+1);
  }

CleanUp:
  SysFreeString(bstrReason);
}

bool hasCurrentDocAnnotations() {
  if (!xmltoolsoptions.useAnnotations) return false;
  try {
    return hasAnnotations.at(::SendMessage(nppData._nppHandle, NPPM_GETCURRENTBUFFERID, 0, 0));
  }
  catch (const std::out_of_range) {}

  return false;
}

void clearAnnotations(HWND view) {
  if (hasCurrentDocAnnotations()) {
    if (view == NULL) {
      int currentEdit;
      ::SendMessage(nppData._nppHandle, NPPM_GETCURRENTSCINTILLA, 0, (LPARAM)&currentEdit);
      view = getCurrentHScintilla(currentEdit);
    }
    ::SendMessage(view, SCI_ANNOTATIONCLEARALL, NULL, NULL);
    hasAnnotations[::SendMessage(nppData._nppHandle, NPPM_GETCURRENTBUFFERID, 0, 0)] = false;
  }
}

int performXMLCheck(int informIfNoError) {
  dbgln("performXMLCheck()");

  int currentEdit, currentLength, res = 0;
  ::SendMessage(nppData._nppHandle, NPPM_GETCURRENTSCINTILLA, 0, (LPARAM)&currentEdit);
  HWND hCurrentEditView = getCurrentHScintilla(currentEdit);

  clearAnnotations(hCurrentEditView);

  currentLength = (int) ::SendMessage(hCurrentEditView, SCI_GETLENGTH, 0, 0);

  char *data = new char[currentLength + sizeof(char)];
  if (!data) return -1;  // allocation error, abort check
  memset(data, '\0', currentLength + sizeof(char));

  ::SendMessage(hCurrentEditView, SCI_GETTEXT, currentLength + sizeof(char), reinterpret_cast<LPARAM>(data));

  //updateProxyConfig();
  HRESULT hr = S_OK;
  IXMLDOMDocument2* pXMLDom = NULL;
  IXMLDOMParseError* pXMLErr = NULL;
  VARIANT_BOOL varStatus;
  VARIANT varCurrentData;

  Report::char2VARIANT(data, &varCurrentData);

  delete[] data;
  data = NULL;

  CHK_HR(CreateAndInitDOM(&pXMLDom));
  CHK_HR(pXMLDom->load(varCurrentData, &varStatus));
  if (varStatus == VARIANT_TRUE) {
    if (informIfNoError) {
      Report::_printf_inf(L"No error detected.");
    }
  } else {
    CHK_HR(pXMLDom->get_parseError(&pXMLErr));
    displayXMLError(pXMLErr, hCurrentEditView, L"XML Parsing error");
  }

CleanUp:
  SAFE_RELEASE(pXMLDom);
  SAFE_RELEASE(pXMLErr);
  VariantClear(&varCurrentData);

  return res;
}

void autoXMLCheck() {
  dbgln("autoXMLCheck()");

  performXMLCheck(0);
}

void manualXMLCheck() {
  dbgln("manualXMLCheck()");

  performXMLCheck(1);
}

///////////////////////////////////////////////////////////////////////////////

CSelectFileDlg* pSelectFileDlg = NULL;
void XMLValidation(int informIfNoError) {
  dbgln("XMLValidation()");

  HRESULT hr = S_OK;
  IXMLDOMDocument2* pXMLDom = NULL;
  IXMLDOMParseError* pXMLErr = NULL;
  IXMLDOMNodeList* pNodes = NULL;
  IXMLDOMNode* pNode = NULL;
  IXMLDOMElement* pElement = NULL;
  IXMLDOMSchemaCollection2* pXS = NULL;
  IXMLDOMDocument2* pXD = NULL;
  VARIANT_BOOL varStatus;
  VARIANT varXML;
  BSTR bstrNodeName = NULL;
  long length;

  // 0. change current folder
  TCHAR currenPath[MAX_PATH] = { '\0' };
  ::SendMessage(nppData._nppHandle, NPPM_GETCURRENTDIRECTORY, MAX_PATH, (LPARAM)currenPath);
  _chdir(Report::narrow(currenPath).c_str());

  // 1. check xml syntax
  bool abortValidation = false;
  std::string xml_schema("");
  int currentEdit, currentLength;
  ::SendMessage(nppData._nppHandle, NPPM_GETCURRENTSCINTILLA, 0, (LPARAM)&currentEdit);
  HWND hCurrentEditView = getCurrentHScintilla(currentEdit);
  currentLength = (int) ::SendMessage(hCurrentEditView, SCI_GETLENGTH, 0, 0);

  char *data = new char[currentLength + sizeof(char)];
  if (!data) return;  // allocation error, abort check
  memset(data, '\0', currentLength + sizeof(char));

  ::SendMessage(hCurrentEditView, SCI_GETTEXT, currentLength + sizeof(char), reinterpret_cast<LPARAM>(data));

  Report::char2VARIANT(data, &varXML);

  CHK_HR(CreateAndInitDOM(&pXMLDom, (INIT_OPTION_VALIDATEONPARSE | INIT_OPTION_RESOLVEEXTERNALS)));

  /*
  // Configure DOM properties for namespace selection.
  CHK_HR(pXMLDoc->setProperty(L"SelectionLanguage", "XPath"));
  //_bstr_t ns = L"xmlns:x='urn:book'";
  CHK_HR(pXMLDoc->setProperty(L"SelectionNamespaces", "xmlns:x='urn:book'"));
  */

  CHK_HR(pXMLDom->load(varXML, &varStatus));
  if (varStatus == VARIANT_TRUE) {
    // search for xsi:noNamespaceSchemaLocation or xsi:schemaLocation
    CHK_HR(pXMLDom->setProperty(L"SelectionNamespaces", _variant_t("xmlns:xsi='http://www.w3.org/2001/XMLSchema-instance'")));
    hr = pXMLDom->selectNodes(L"/*/@xsi:noNamespaceSchemaLocation", &pNodes);
    CHK_HR(pNodes->get_length(&length));
    SAFE_RELEASE(pNodes);
    bool nnsl = (length > 0);
    hr = pXMLDom->selectNodes(L"/*/@xsi:schemaLocation", &pNodes);
    CHK_HR(pNodes->get_length(&length));
    bool sl = (length > 0);

    if (nnsl || sl) {
      // if noNamespaceSchemaLocation or schemaLocation attribute is present,
      // validation is supposed OK since xml is loaded with INIT_OPTION_VALIDATEONPARSE
      // option
      Report::_printf_inf(L"No error detected.");
    } else {
      // if noNamespaceSchemaLocation or schemaLocation attributes are not present,
      // we must prompt for XSD to user and create a schema cache
      if (pSelectFileDlg == NULL) {
        pSelectFileDlg = new CSelectFileDlg();
      }
      //pSelectFileDlg->m_sSelectedFilename = lastXMLSchema.c_str();

      CStringW rootSample = "<";
      CHK_HR(pXMLDom->get_documentElement(&pElement));
      CHK_HR(pElement->get_nodeName(&bstrNodeName));
      rootSample += bstrNodeName;
      rootSample += "\r\n    xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"";
      rootSample += "\r\n    xsi:noNamespaceSchemaLocation=\"XSD_FILE_PATH\">";

      pSelectFileDlg->m_sRootElementSample = rootSample;
      if (pSelectFileDlg->DoModal() == IDOK) {
        //lastXMLSchema = pSelectFileDlg->m_sSelectedFilename;

        // Create a schema cache and add schema to it (currently with no namespace)
        CHK_HR(CreateAndInitSchema(&pXS));
        hr = pXS->add(_bstr_t(pSelectFileDlg->m_sValidationNamespace), CComVariant(pSelectFileDlg->m_sSelectedFilename));
        if (SUCCEEDED(hr)) {
          // Create a DOMDocument and set its properties.
          CHK_HR(CreateAndInitDOM(&pXD, (INIT_OPTION_VALIDATEONPARSE | INIT_OPTION_RESOLVEEXTERNALS)));
          CHK_HR(pXD->putref_schemas(CComVariant(pXS)));

          /*
          pXD->put_async(VARIANT_FALSE);
          pXD->put_validateOnParse(VARIANT_TRUE);
          pXD->put_resolveExternals(VARIANT_TRUE);
          */

          CHK_HR(pXD->load(varXML, &varStatus));
          if (varStatus == VARIANT_TRUE) {
            Report::_printf_inf(L"No error detected.");
          } else {
            CHK_HR(pXD->get_parseError(&pXMLErr));
            displayXMLError(pXMLErr, hCurrentEditView, L"XML Validation error");
          }
        } else {
          Report::_printf_err(L"Invalid schema or missing namespace.");
        }
      } else {
        goto CleanUp;
      }
    }
  } else {
    CHK_HR(pXMLDom->get_parseError(&pXMLErr));
    displayXMLError(pXMLErr, hCurrentEditView, L"XML Validation error");
  }

CleanUp:
  SAFE_RELEASE(pXMLDom);
  SAFE_RELEASE(pXMLErr);
  SAFE_RELEASE(pNodes);
  SAFE_RELEASE(pNode);
  SAFE_RELEASE(pElement);
  SAFE_RELEASE(pXS);
  SAFE_RELEASE(pXD);
  VariantClear(&varXML);
  SysFreeString(bstrNodeName);
}

void autoValidation() {
  dbgln("autoValidation()");

  XMLValidation(0);
}

void manualValidation() {
  dbgln("manualValidation()");

  XMLValidation(1);
}

///////////////////////////////////////////////////////////////////////////////

void closeXMLTag() {
  dbgln("closeXMLTag()");

  char buf[512];
  int currentEdit;
  ::SendMessage(nppData._nppHandle, NPPM_GETCURRENTSCINTILLA, 0, (LPARAM)&currentEdit);
  HWND hCurrentEditView = getCurrentHScintilla(currentEdit);
  int currentPos = int(::SendMessage(hCurrentEditView, SCI_GETCURRENTPOS, 0, 0));
  int beginPos = currentPos - (sizeof(buf) - 1);
  int startPos = (beginPos > 0)?beginPos:0;
  int size = currentPos - startPos;
  int insertStringSize = 2;

  #define MAX_TAGNAME_LENGTH 516
  char insertString[MAX_TAGNAME_LENGTH] = "</";

  if (size >= 3) {
    struct TextRange tr = {{startPos, currentPos}, buf};
    ::SendMessage(hCurrentEditView, SCI_GETTEXTRANGE, 0, (LPARAM)&tr);

    if (buf[size-2] != '/' && buf[size-2] != '-') {
      const char* pBegin = &buf[0];
      const char* pCur = &buf[size - 2];
      int insertStringSize = 2;

      // search the beginning of tag
      // TODO: optimize by not looping on every char !
      for (; pCur > pBegin && *pCur != '<' && *pCur != '>' ;) {
        --pCur;
      }

      if (*pCur == '<') {
        ++pCur;
        if (*pCur == '/' || *pCur == '!') return;

        // search attributes of
        while (*pCur != '>' && *pCur != ' ' && *pCur != '\n' && *pCur != '\r') {
        //while (IsCharAlphaNumeric(*pCur) || strchr(":_-.", *pCur) != NULL) {
          if (insertStringSize == MAX_TAGNAME_LENGTH-1) return;
          insertString[insertStringSize++] = *pCur;
          ++pCur;
        }
      }

      if (insertStringSize == MAX_TAGNAME_LENGTH-1) return;
      insertString[insertStringSize++] = '>';
      insertString[insertStringSize] = '\0';

      if (insertStringSize > 3) {
        ::SendMessage(hCurrentEditView, SCI_BEGINUNDOACTION, 0, 0);
        ::SendMessage(hCurrentEditView, SCI_REPLACESEL, 0, (LPARAM)insertString);
        ::SendMessage(hCurrentEditView, SCI_SETSEL, currentPos, currentPos);
        ::SendMessage(hCurrentEditView, SCI_ENDUNDOACTION, 0, 0);
      }
    }
  }

  #undef MAX_TAGNAME_LENGTH
}

///////////////////////////////////////////////////////////////////////////////

void tagAutoIndent() {
  dbgln("tagAutoIndent()");

  // On n'indente que si l'on est dans un noeud (au niveau de l'attribut ou
  // au niveau du contenu. Donc on recherche le dernier < ou >. S'il s'agit
  // d'un >, on regarde qu'il n'y ait pas de / avant (sinon on se retrouve
  // au même niveau et il n'y a pas d'indentation à faire)
  // Si le dernier symbole que l'on trouve est un <, alors on indente.

  char buf[512];
  int currentEdit;
  ::SendMessage(nppData._nppHandle, NPPM_GETCURRENTSCINTILLA, 0, (LPARAM)&currentEdit);
  HWND hCurrentEditView = getCurrentHScintilla(currentEdit);
  int currentPos = int(::SendMessage(hCurrentEditView, SCI_GETCURRENTPOS, 0, 0));
  int beginPos = currentPos - (sizeof(buf) - 1);
  int startPos = (beginPos > 0)?beginPos:0;
  int size = currentPos - startPos;

  struct TextRange tr = {{startPos, currentPos}, buf};
  ::SendMessage(hCurrentEditView, SCI_GETTEXTRANGE, 0, (LPARAM)&tr);

  int tabwidth = (int) ::SendMessage(hCurrentEditView, SCI_GETTABWIDTH, 0, 0);
  bool usetabs = (bool) ::SendMessage(hCurrentEditView, SCI_GETUSETABS, 0, 0);
  if (tabwidth <= 0) tabwidth = 4;

  bool ignoreIndentation = false;
  if (size >= 1) {
    const char* pBegin = &buf[0];
    const char* pCur = &buf[size - 1];

    for (; pCur > pBegin && *pCur != '>' ;) --pCur;
    if (pCur > pBegin) {
      if (*(pCur-1) == '/') ignoreIndentation = true;  // si on a "/>", on abandonne l'indentation
      // maintenant, on recherche le <
      while (pCur > pBegin && *pCur != '<') --pCur;
      if (*pCur == '<' && *(pCur+1) == '/') ignoreIndentation = true; // si on a "</", on abandonne aussi

      int insertStringSize = 0;
      char insertString[516] = { '\0' };

      --pCur;
      // on récupère l'indentation actuelle
      while (pCur > pBegin && *pCur != '\n' && *pCur != '\r') {
        if (*pCur == '\t') insertString[insertStringSize++] = '\t';
        else insertString[insertStringSize++] = ' ';

        --pCur;
      }

      // et on ajoute une indentation
      if (!ignoreIndentation) {
        if (usetabs) insertString[insertStringSize++] = '\t';
        else {
          for (int i = 0; i < tabwidth; ++i) insertString[insertStringSize++] = ' ';
        }
      }

      currentPos += insertStringSize;

      // on a trouvé le <, il reste à insérer une indentation après le curseur
      ::SendMessage(hCurrentEditView, SCI_REPLACESEL, 0, (LPARAM)insertString);
      ::SendMessage(hCurrentEditView, SCI_SETSEL, currentPos, currentPos);
    }
  }
}

///////////////////////////////////////////////////////////////////////////////

void attributeAutoComplete() {
  dbgln(L"attributeAutoComplete()");

  Report::_printf_inf(L"attributeAutoComplete()");
}

///////////////////////////////////////////////////////////////////////////////

bool setAutoXMLType() {
  dbgln("setAutoXMLType()");

  int currentEdit;
  ::SendMessage(nppData._nppHandle, NPPM_GETCURRENTSCINTILLA, 0, (LPARAM)&currentEdit);
  HWND hCurrentEditView = getCurrentHScintilla(currentEdit);

  // on récupère les 6 premiers caractères du fichier
  char head[8] = { '\0' };
  ::SendMessage(hCurrentEditView, SCI_GETTEXT, 7, reinterpret_cast<LPARAM>(&head));

  if (strlen(head) >= 6 && !strcmp(head, "<?xml ")) {
    ::SendMessage(nppData._nppHandle, NPPM_SETCURRENTLANGTYPE, 0, (LPARAM) L_XML);
    return true;
  }

  return false;
}

///////////////////////////////////////////////////////////////////////////////

std::wstring currentXPath(bool preciseXPath) {
  dbgln("currentXPath()");

  HRESULT hr = S_OK;
  ISAXXMLReader* pRdr = NULL;
  variant_t varXML;
  PathBuilder pPB;

  int currentEdit;
  std::string::size_type currentLength, currentPos;
  ::SendMessage(nppData._nppHandle, NPPM_GETCURRENTSCINTILLA, 0, (LPARAM)&currentEdit);
  HWND hCurrentEditView = getCurrentHScintilla(currentEdit);
  currentLength = (int) ::SendMessage(hCurrentEditView, SCI_GETLENGTH, 0, 0);

  std::wstring nodepath(L"");

  char *data = new char[currentLength + sizeof(char)];
  if (!data) return nodepath;  // allocation error, abort check
  size_t size = (currentLength + sizeof(char))*sizeof(char);
  memset(data, '\0', size);

  currentPos = long(::SendMessage(hCurrentEditView, SCI_GETCURRENTPOS, 0, 0));
  ::SendMessage(hCurrentEditView, SCI_GETTEXT, currentLength + sizeof(char), reinterpret_cast<LPARAM>(data));

  std::string str(data);
  delete [] data;
  data = NULL;

  // end tag pos
  std::string::size_type begpos = str.find_first_of("<");
  std::string::size_type endpos = str.find_last_of(">");

  // let's reach the end of current tag (if we are inside a tag)
  if (currentPos > begpos && currentPos <= endpos) {
    currentPos = str.find_last_of("\"<>", currentPos-1)+1;
    bool cursorInAttribute = false;

    // check if we are in a closing tag
    if (currentPos >= 1 && currentPos < currentLength - 2 * sizeof(char) && str.at(currentPos-1) == '<'  && str.at(currentPos) == '!' && str.at(currentPos+1) == '-' && str.at(currentPos+2) == '-') {  // check if in a comment
      return nodepath;
    } else if (currentPos >= 1 && str.at(currentPos-1) == '<' && str.at(currentPos) == '/') {
      // if we are inside closing tag (inside </x>, let's go back before '<' char so we are inside node)
      --currentPos;
    } else {
      if (currentPos >= 2 && str.at(currentPos - 1) == '\"' && str.at(currentPos - 2) == '=') {
        cursorInAttribute = true;
        currentPos = str.find('\"', currentPos) + 1;
      } else {
        // let's get the end of current tag or text
        size_t s = str.find_first_of("<>", currentPos);
        // if inside a auto-closing tag (ex. '<x/>'), let's go back before '/' char, the '>' is added before slash)
        if (s > 0 && str.at(s) == '>' && str.at(s - 1) == '/') currentPos = s - 1;
        else currentPos = s;
      }
    }

    str.erase(currentPos);
    str += ">";

    varXML.SetString(str.c_str());

    CHK_HR(CreateAndInitSAX(&pRdr));
    CHK_HR(pRdr->putContentHandler(&pPB));

    // no CHK_HR on next call because it will fail since our xml has been truncated at current cursor location
    pRdr->parse(varXML);

    nodepath = pPB.getPath(preciseXPath);
    pRdr->Release();
  }

CleanUp:

  return nodepath;
}

void getCurrentXPath(bool precise) {
  dbgln("getCurrentXPath()");

  std::wstring nodepath(currentXPath(precise));
  std::wstring tmpmsg(L"Current node cannot be resolved.");

  if (nodepath.length() > 0) {
    tmpmsg = nodepath;
    tmpmsg += L"\n\n(Path has been copied into clipboard)";

    ::OpenClipboard(NULL);
    ::EmptyClipboard();
    size_t size = (nodepath.length()+1) * sizeof(wchar_t);
    HGLOBAL hClipboardData = GlobalAlloc(NULL, size);
    wchar_t * pchData = (wchar_t*)GlobalLock(hClipboardData);
    memcpy(pchData, (wchar_t*) nodepath.c_str(), size);
    ::GlobalUnlock(hClipboardData);
    ::SetClipboardData(CF_UNICODETEXT, hClipboardData);
    ::CloseClipboard();
  }

  Report::_printf_inf(tmpmsg.c_str());
}
void getCurrentXPathStd() {
  dbgln("getCurrentXPathStd()");
  getCurrentXPath(false);
}
void getCurrentXPathPredicate() {
  dbgln("getCurrentXPathPredicate()");
  getCurrentXPath(true);
}

///////////////////////////////////////////////////////////////////////////////

CXPathEvalDlg *pXPathEvalDlg = NULL;

void evaluateXPath() {
  dbgln("evaluateXPath()");

  if (pXPathEvalDlg == NULL) {
    pXPathEvalDlg = new CXPathEvalDlg(NULL, NULL);
    pXPathEvalDlg->Create(CXPathEvalDlg::IDD,NULL);
  }
  pXPathEvalDlg->ShowWindow(SW_SHOW);
}

///////////////////////////////////////////////////////////////////////////////

CXSLTransformDlg *pXSLTransformDlg = NULL;
void performXSLTransform() {
  dbgln("performXSLTransform()");

  if (pXSLTransformDlg == NULL) {
    pXSLTransformDlg = new CXSLTransformDlg(NULL, NULL);
    pXSLTransformDlg->Create(CXSLTransformDlg::IDD,NULL);
  }
  pXSLTransformDlg->ShowWindow(SW_SHOW);
}

///////////////////////////////////////////////////////////////////////////////

std::string& ltrim(std::string& str, const std::string& chars = "\t\n\v\f\r ") {
  str.erase(0, str.find_first_not_of(chars));
  return str;
}

std::string& rtrim(std::string& str, const std::string& chars = "\t\n\v\f\r ") {
  str.erase(str.find_last_not_of(chars) + 1);
  return str;
}

std::string& trim(std::string& str, const std::string& chars = "\t\n\v\f\r ") {
  return ltrim(rtrim(str, chars), chars);
}

std::string& trimxml(std::string& str, std::string eolchar, bool breaklines, bool breaktags, const std::string& chars = "\t\n\v\f\r ") {
  bool in_tag = false, in_header = false;
  char cc;
  std::string::size_type curpos = 0, lastpos = 0, lastlen = 0, lasteolpos = 0, tmppos;
  size_t eolcharlen = eolchar.length();
  size_t eolcharpos = eolchar.find('\n');

  while (curpos < str.length() && (curpos = str.find_first_of("<>\"'\n", curpos)) != std::string::npos) {
    switch (cc = str.at(curpos)) {
      case '<': {
        if (curpos < str.length() - 4 && !str.compare(curpos, 4, "<!--")) {            // is comment start ?
          // skip the comment
          curpos = str.find("-->", curpos + 1) + 2;
        }
        else if (curpos < str.length() - 9 && !str.compare(curpos, 9, "<![CDATA[")) {       // is CDATA start ?
          // skip the CDATA
          curpos = str.find("]]>", curpos + 1) + 2;
        }
        else if (curpos < str.length() - 2 && !str.compare(curpos, 2, "</")) {              // end tag (ex: "</sample>")
          curpos = str.find(">", curpos + 1);

          // add line break if next non space char is "<"
          if (breaklines) {
            tmppos = str.find_first_not_of(chars, curpos + 1);
            if (tmppos != std::string::npos && str.at(tmppos) == '<') {
              str.insert(curpos + 1, eolchar);
            }
          }
        }
        else {
          in_tag = true;
          if (curpos < str.length() - 2 && !str.compare(curpos, 2, "<?")) {
            in_header = true;
            ++curpos;
          }

          // skip the tag name
          char endtag = '/';
          if (in_header) endtag = '?';
          curpos = str.find_first_of("\t\n\v\f\r ?/>", curpos + 1);
          if (curpos != std::string::npos) {
            tmppos = str.find_first_not_of("\t\n\v\f\r ", curpos);
            if (tmppos != std::string::npos) {
              // trim space before attribute or ">" char
              str.erase(curpos, tmppos - curpos);
              if (str.at(curpos) != '>' && str.at(curpos) != endtag) {
                str.insert(curpos, " ");
                ++curpos;
              }
            }
            --curpos;
          }
        }
        break;
      }
      case '>': {
        if (in_tag) {
          in_tag = false;
          in_header = false;

          // add line break if next non space char is <
          if (breaklines) {
            tmppos = str.find_first_not_of(chars, curpos + 1);
            if (tmppos != std::string::npos && str.at(tmppos) == '<') {
              str.insert(curpos + 1, eolchar);
            }
          }
        }
        break;
      }
      case '\"':
      case '\'': {
        if (in_tag) {
          // trim spaces arround "=" char
          tmppos = str.find_last_not_of("\t\n\v\f\r ", curpos - 1);
          if (tmppos != std::string::npos && tmppos < curpos && str.at(tmppos) == '=') {
            // remove spaces after "="
            str.erase(tmppos + 1, curpos - tmppos - 1);
            curpos = tmppos + 1;
            // remove spaces before "="
            tmppos = str.find_last_not_of("\t\n\v\f\r ", tmppos - 1);
            if (tmppos != std::string::npos) {
              str.erase(tmppos + 1, curpos - tmppos - 2);
              curpos = tmppos + 2;
            }
          }
          // skip attribute text
          curpos = str.find(cc, curpos + 1);

          // trim spaces after attribute
          tmppos = str.find_first_not_of("\t\n\v\f\r ", curpos + 1);
          if (tmppos != std::string::npos) {
            char endtag = '/';
            if (in_header) endtag = '?';

            // add line break if not the last attribute
            if (breaktags && !in_header && str.at(tmppos) != '>' && str.at(tmppos) != endtag) {
              str.insert(curpos + 1, eolchar);
            }
            else if (!breaktags) {
              str.erase(curpos + 1, tmppos - curpos - 1);
              if (str.at(curpos + 1) != '>' && str.at(curpos + 1) != endtag) {
                str.insert(curpos + 1, " ");
                ++curpos;
              }
            }
          }
          else {
            
          }
        }
        break;
      }
      case '\n': {
        // trim line

        curpos -= eolcharpos;

        if (in_tag && !breaktags) {
          // we must remove line breaks
          tmppos = str.find_first_not_of("\t\n\v\f\r ", curpos + 1);
          if (tmppos != std::string::npos) {
            str.erase(curpos, tmppos - curpos);
          }
          if (str.at(curpos - 1) == '"' || str.at(curpos - 1) == '\'') {
            str.insert(curpos, " ");
            ++curpos;
          }
        } else {  // = if (!in_tag || breaktags)
          std::string tmp = trim(str.substr(lasteolpos, curpos - lasteolpos));
          str.replace(lasteolpos, curpos - lasteolpos, tmp);
          curpos = lasteolpos + tmp.length();
          lasteolpos = curpos;

          while (lasteolpos >= eolcharlen && !str.compare(lasteolpos - eolcharlen, eolcharlen, eolchar)) {
            lasteolpos -= eolcharlen;
          }

          lasteolpos += eolcharlen;
        }

        curpos += (eolcharlen - 1);
        
        break;
      }
    }

    ++curpos;

    // inifinite loop protection
    tmppos = str.length();
    if (curpos == lastpos && lastlen == tmppos) {
      dbgln("TRIM: INIFINITE LOOP DETECTED");
      break;
    }
    lastpos = curpos;
    lastlen = tmppos;
  }

  if (lasteolpos < str.length()) {
    str.replace(lasteolpos, str.length() - lasteolpos, trim(str.substr(lasteolpos, str.length() - lasteolpos)));
  }

  return str;
}

void prettyPrint(bool autoindenttext, bool addlinebreaks, bool indentattributes) {
  dbgln("prettyPrint()");

  int docIterator = initDocIterator();
  while (hasNextDoc(&docIterator)) {
    int currentEdit, currentLength, isReadOnly;
    ::SendMessage(nppData._nppHandle, NPPM_GETCURRENTSCINTILLA, 0, (LPARAM)&currentEdit);
    HWND hCurrentEditView = getCurrentHScintilla(currentEdit);

    isReadOnly = (int) ::SendMessage(hCurrentEditView, SCI_GETREADONLY, 0, 0);
    if (isReadOnly) return;

    char *data = NULL;

    // some state variables
    bool in_tag = false;

    // some counters
    std::string::size_type curpos = 0, lastpos = 0, lastlen = 0, tmppos, xmllevel = 0, tagnamelen = 0;
    // some char value (pc = previous char, cc = current char, nc = next char, nnc = next next char)
    char cc;

    int tabwidth = (int) ::SendMessage(hCurrentEditView, SCI_GETTABWIDTH, 0, 0);
    bool usetabs = (bool) ::SendMessage(hCurrentEditView, SCI_GETUSETABS, 0, 0);
    if (tabwidth <= 0) tabwidth = 4;

    // use the selection
    long selstart = (long) ::SendMessage(hCurrentEditView, SCI_GETSELECTIONSTART, 0, 0);
    long selend = (long) ::SendMessage(hCurrentEditView, SCI_GETSELECTIONEND, 0, 0);
    // TODO: when oprating on selection, we should determinate the node indentation level
    //       we could use the XPath retrieval function to perform this

    std::string str("");
    std::string eolchar;
    int eolmode;
    Report::getEOLChar(hCurrentEditView, &eolmode, &eolchar);
    size_t eolcharlen = eolchar.length();
    size_t eolcharpos = eolchar.find('\n');

    if (selend > selstart) {
      currentLength = selend-selstart;
      data = new char[currentLength + sizeof(char)];
      if (!data) return;  // allocation error, abort check
      memset(data, '\0', currentLength + sizeof(char));
      ::SendMessage(hCurrentEditView, SCI_GETSELTEXT, 0, reinterpret_cast<LPARAM>(data));
    } else {
      currentLength = (int) ::SendMessage(hCurrentEditView, SCI_GETLENGTH, 0, 0);
      data = new char[currentLength + sizeof(char)];
      if (!data) return;  // allocation error, abort check
      memset(data, '\0', currentLength + sizeof(char));
      ::SendMessage(hCurrentEditView, SCI_GETTEXT, currentLength + sizeof(char), reinterpret_cast<LPARAM>(data));
    }

    str += data;
    delete[] data;
    data = NULL;

    // first pass: trim lines
    str = trimxml(str, eolchar, true, indentattributes);

    #ifdef _DEBUG
      if (selend <= selstart) {
        ::SendMessage(hCurrentEditView, SCI_SETTEXT, 0, reinterpret_cast<LPARAM>(str.c_str()));
      }
    #endif



    // second pass: indentation
    while (curpos < str.length() && (curpos = str.find_first_of("<>\"'\n", curpos)) != std::string::npos) {
      switch (cc = str.at(curpos)) {
        case '<': {
          if (curpos < str.length() - 2 && !str.compare(curpos, 2, "<?")) {                   // is "<?xml ...?>" definition ?
            // skip the comment
            curpos = str.find("?>", curpos + 1) + 1;
          }
          else if (curpos < str.length() - 4 && !str.compare(curpos, 4, "<!--")) {            // is comment start ?
            // skip the comment
            curpos = str.find("-->", curpos + 1) + 2;
          }
          else if (curpos < str.length() - 9 && !str.compare(curpos, 9, "<![CDATA[")) {       // is CDATA start ?
            // skip the CDATA
            curpos = str.find("]]>", curpos + 1) + 2;
          }
          else if (curpos < str.length() - 2 && !str.compare(curpos, 2, "</")) {              // end tag (ex: "</sample>")
            curpos = str.find(">", curpos + 1);
            if (xmllevel > 0) --xmllevel;
          }
          else {                                                                              // beg tag
            in_tag = true;
            ++xmllevel;

            // skip the tag name
            tmppos = str.find_first_of("\t\n\v\f\r />", curpos + 1);
            if (tmppos != std::string::npos) {
              // calculate tag name length
              if (indentattributes) tagnamelen = tmppos - curpos - 1;

              curpos = tmppos - 1;
            }
          }
          break;
        }
        case '>': {
          if (in_tag) {
            in_tag = false;
            if (curpos > 0 && !str.compare(curpos - 1, 1, "/")) {                             // auto-closing tag (ex: "<sample/>")
              --xmllevel;
            }
          }
          break;
        }
        case '\"':
        case '\'': {
          if (in_tag) {
            // skip attribute text
            curpos = str.find(cc, curpos + 1);
          }
          break;
        }
        case '\n': {
          // fix indentation
          curpos -= eolcharpos;   // line break may have several chars

          if (xmllevel > 0) {
            // we apply a delta when next tag is a closing tag
            long delta = 0;
            tmppos = curpos + eolcharlen;
            if (tmppos < str.length() - 1 && str.at(tmppos) == '<' && str.at(tmppos + 1) == '/') {
              delta = 1;
            }

            // apply indentation
            if (usetabs) {
              str.insert(curpos + eolcharlen, (xmllevel - delta), '\t');
            }
            else {
              str.insert(curpos + eolcharlen, tabwidth * (xmllevel - delta), ' ');
            }
          }

          curpos += eolcharlen;

          if (in_tag && indentattributes) {
            // do something
            str.insert(curpos, tagnamelen, ' ');
          }

          --curpos;

          break;
        }
      }

      ++curpos;

      // inifinite loop protection
      tmppos = str.length();
      if (curpos == lastpos && lastlen == tmppos) {
        dbgln("PRETTYPRINT: INIFINITE LOOP DETECTED");
        break;
      }
      lastpos = curpos;
      lastlen = tmppos;
    }

    // Send formatted string to scintilla
    if (selend > selstart) {
      ::SendMessage(hCurrentEditView, SCI_REPLACESEL, 0, reinterpret_cast<LPARAM>(str.c_str()));
    }
    else {
      ::SendMessage(hCurrentEditView, SCI_SETTEXT, 0, reinterpret_cast<LPARAM>(str.c_str()));
    }

    str.clear();

    // Put scroll at the left of the view
    ::SendMessage(hCurrentEditView, SCI_SETXOFFSET, 0, 0);
  }
}

void prettyPrintXML() {
  dbgln("prettyPrintXML()");

  prettyPrint(false, true, false);
}

void prettyPrintAttributes() {
  dbgln("prettyPrintAttributes()");

  prettyPrint(false, true, true);

  /*
  int docIterator = initDocIterator();
  while (hasNextDoc(&docIterator)) {
    int currentEdit, currentLength, isReadOnly;
    ::SendMessage(nppData._nppHandle, NPPM_GETCURRENTSCINTILLA, 0, (LPARAM)&currentEdit);
    HWND hCurrentEditView = getCurrentHScintilla(currentEdit);

    isReadOnly = (int) ::SendMessage(hCurrentEditView, SCI_GETREADONLY, 0, 0);
    if (isReadOnly) return;

    currentLength = (int) ::SendMessage(hCurrentEditView, SCI_GETLENGTH, 0, 0);

    char *data = new char[currentLength + sizeof(char)];
    if (!data) return;  // allocation error, abort check
    memset(data, '\0', currentLength + sizeof(char));

    ::SendMessage(hCurrentEditView, SCI_GETTEXT, currentLength + sizeof(char), reinterpret_cast<LPARAM>(data));

    int tabwidth = (int) ::SendMessage(hCurrentEditView, SCI_GETTABWIDTH, 0, 0);
    bool usetabs = (bool) ::SendMessage(hCurrentEditView, SCI_GETUSETABS, 0, 0);
    if (tabwidth <= 0) tabwidth = 4;

    HRESULT hr = S_OK;
    IXMLDOMDocument2* pXMLDom = NULL;
    VARIANT_BOOL varStatus;
    VARIANT varCurrentData;

    bool in_comment = false, in_header = false, in_attribute = false, in_nodetext = false, in_cdata = false;
    size_t curpos = 0, strlength = 0;
    std::string lineindent = "";
    char pc, cc, nc, nnc;
    int tagsignlevel = 0, nattrs = 0;

    std::string eolchar;
    int eolmode;

    Report::char2VARIANT(data, &varCurrentData);
    std::string str = data;
    delete[] data;

    CHK_HR(CreateAndInitDOM(&pXMLDom));
    CHK_HR(pXMLDom->load(varCurrentData, &varStatus));
    if (varStatus != VARIANT_TRUE) {
      Report::_printf_err(L"Errors detected in content. Please correct them before applying pretty print.");
      goto CleanUp;
    }

    Report::getEOLChar(hCurrentEditView, &eolmode, &eolchar);

    size_t prevspecchar = -1;
    while (curpos < str.length() && (curpos = str.find_first_of("<>\"",curpos)) >= 0) {
      strlength = str.length();
      if (curpos < strlength-3 && !str.compare(curpos,4,"<!--")) in_comment = true;
      if (curpos < strlength-8 && !str.compare(curpos,9,"<![CDATA[")) in_cdata = true;
      else if (curpos < strlength-1 && !str.compare(curpos,2,"<?")) in_header = true;
      else if (curpos < strlength && !str.compare(curpos,1,"\"") &&
               prevspecchar >= 0 && str.at(prevspecchar) == '<') in_attribute = !in_attribute;

      if (!in_comment && !in_cdata && !in_header) {
        if (curpos > 0) pc = str.at(curpos-1);
        else pc = ' ';

        cc = str.at(curpos);

        if (curpos < strlength-1) nc = str.at(curpos+1);
        else nc = ' ';

        if (curpos < strlength-2) nnc = str.at(curpos+2);
        else nnc = ' ';

        if (cc == '<') {
          prevspecchar = curpos++;
          ++tagsignlevel;
          in_nodetext = false;
          if (nc != '/' && (nc != '!' || nnc == '[')) nattrs = 0;
        } else if (cc == '>' && !in_attribute) {
          // > are ignored inside attributes
          if (pc != '/' && pc != ']') { in_nodetext = true; }
          --tagsignlevel;
          nattrs = 0;
          prevspecchar = curpos++;
        } else if (in_attribute) {
          if (++nattrs > 1) {
            size_t attrpos = str.find_last_of(eolchar+"\t ", curpos-1)+1;
            if (!Report::isEOL(str, strlength, (unsigned int) attrpos, eolmode)) {
              size_t spacpos = str.find_last_not_of(eolchar+"\t ", attrpos-1)+1;
              str.replace(spacpos, attrpos-spacpos, lineindent);
              curpos -= attrpos-spacpos;
              curpos += lineindent.length();
            }
          } else {
            size_t attrpos = str.find_last_of(eolchar+"\t ", curpos-1)+1;
            if (!Report::isEOL(str, strlength, (unsigned int) attrpos, eolmode)) {
              size_t linestart = str.find_last_of(eolchar, attrpos-1)+1;
              lineindent = str.substr(linestart, attrpos-linestart);
              size_t lineindentlen = lineindent.length();
              for (size_t i = 0; i < lineindentlen; ++i) {
                char lic = lineindent.at(i);
                if (lic != '\t' && lic != ' ') {
                    lineindent.replace(i, 1, " ");
                }
              }
              lineindent.insert(0,eolchar);
            }
          }
          ++curpos;
        } else {
          ++curpos;
        }
      } else {
        if (in_comment && curpos > 1 && !str.compare(curpos - 2, 3, "-->")) in_comment = false;
        else if (in_cdata && curpos > 1 && !str.compare(curpos - 2, 3, "]]>")) in_cdata = false;
        else if (in_header && curpos > 0 && !str.compare(curpos - 1, 2, "?>")) in_header = false;
        ++curpos;
      }
    }

    // Send formatted string to scintilla
    ::SendMessage(hCurrentEditView, SCI_SETTEXT, 0, reinterpret_cast<LPARAM>(str.c_str()));

    // Put scroll at the left of the view
    ::SendMessage(hCurrentEditView, SCI_SETXOFFSET, 0, 0);

  CleanUp:
    SAFE_RELEASE(pXMLDom);
    VariantClear(&varCurrentData);
  }
  */
}

///////////////////////////////////////////////////////////////////////////////

void linearizeXML() {
  dbgln("linearizeXML()");

  int docIterator = initDocIterator();
  while (hasNextDoc(&docIterator)) {
    int currentEdit, currentLength;
    ::SendMessage(nppData._nppHandle, NPPM_GETCURRENTSCINTILLA, 0, (LPARAM)&currentEdit);
    HWND hCurrentEditView = getCurrentHScintilla(currentEdit);

    char* data = NULL;

    // use the selection
    long selstart = 0, selend = 0;
    // désactivé : le fait de prettyprinter que la sélection pose problème pour l'indentation
    // il faudrait calculer l'indentation de la 1re ligne de sélection, mais l'indentation
    // de cette ligne n'est peut-être pas correcte. On pourrait la déterminer en récupérant
    // le path de la banche sélectionnée...
    selstart = (long) ::SendMessage(hCurrentEditView, SCI_GETSELECTIONSTART, 0, 0);
    selend = (long) ::SendMessage(hCurrentEditView, SCI_GETSELECTIONEND, 0, 0);

    if (selend > selstart) {
      currentLength = selend - selstart;
      data = new char[currentLength + sizeof(char)];
      if (!data) return;  // allocation error, abort check
      memset(data, '\0', currentLength + sizeof(char));
      ::SendMessage(hCurrentEditView, SCI_GETSELTEXT, 0, reinterpret_cast<LPARAM>(data));
    }
    else {
      currentLength = (int) ::SendMessage(hCurrentEditView, SCI_GETLENGTH, 0, 0);
      data = new char[currentLength + sizeof(char)];
      if (!data) return;  // allocation error, abort check
      memset(data, '\0', currentLength + sizeof(char));
      ::SendMessage(hCurrentEditView, SCI_GETTEXT, currentLength + sizeof(char), reinterpret_cast<LPARAM>(data));
    }

    std::string str(data);
    delete [] data;
    data = NULL;

    std::string eolchar;
    int eolmode;
    Report::getEOLChar(hCurrentEditView, &eolmode, &eolchar);

    std::string::size_type curpos = 0, nexwchar_t;
    bool enableInsert = false;

    while ((curpos = str.find_first_of(eolchar, curpos)) != std::string::npos) {
      nexwchar_t = str.find_first_not_of(eolchar, curpos);
      str.erase(curpos, nexwchar_t-curpos);

      // Let erase leading space chars on line
      if (curpos != std::string::npos && curpos < str.length()) {
        nexwchar_t = str.find_first_not_of(" \t", curpos);
        if (nexwchar_t != std::string::npos && nexwchar_t >= curpos) {
          // And if the 1st char of next line is not '<' and last char of preceding
          // line is not '>', then we consider we are in text content, then let put
          // a space char
          enableInsert = false;
          if (curpos > 0 && str.at(nexwchar_t) != '<' && str.at(curpos-1) != '>') {
            enableInsert = true;
            if (nexwchar_t > curpos) --nexwchar_t;
          }

          if (nexwchar_t > curpos) str.erase(curpos, nexwchar_t-curpos);
          else if (enableInsert) str.insert(nexwchar_t, " ");
        }
      }
    }

    // Send formatted string to scintilla
    if (selend > selstart) {
      ::SendMessage(hCurrentEditView, SCI_REPLACESEL, 0, reinterpret_cast<LPARAM>(str.c_str()));
    }
    else {
      ::SendMessage(hCurrentEditView, SCI_SETTEXT, 0, reinterpret_cast<LPARAM>(str.c_str()));
    }

    str.clear();
  }
}

void togglePrettyPrintAllFiles() {
  dbgln("togglePrettyPrintAllFiles()");

  doPrettyPrintAllOpenFiles = !doPrettyPrintAllOpenFiles;
  ::CheckMenuItem(::GetMenu(nppData._nppHandle), funcItem[menuitemPrettyPrintAllFiles]._cmdID, MF_BYCOMMAND | (doPrettyPrintAllOpenFiles?MF_CHECKED:MF_UNCHECKED));
  savePluginParams();
}

int initDocIterator() {
  dbgln("initDocIterator()");

  nbopenfiles1 = (int) ::SendMessage(nppData._nppHandle, NPPM_GETNBOPENFILES, 0, PRIMARY_VIEW);
  nbopenfiles2 = (int) ::SendMessage(nppData._nppHandle, NPPM_GETNBOPENFILES, 0, SECOND_VIEW);

  if (::SendMessage(nppData._nppHandle, NPPM_GETCURRENTDOCINDEX, 0, MAIN_VIEW) < 0) nbopenfiles1 = 0;
  if (::SendMessage(nppData._nppHandle, NPPM_GETCURRENTDOCINDEX, 0, SUB_VIEW) < 0) nbopenfiles2 = 0;

  //Report::_printf_inf(Report::str_format("%d %d",nbopenfiles1,nbopenfiles2));

  return 0;
}

bool hasNextDoc(int* iter) {
  dbgln("hasNextDoc()");

  if (doPrettyPrintAllOpenFiles) {
    if (*iter < 0 || *iter >= (nbopenfiles1+nbopenfiles2)) return false;

    if (*iter < nbopenfiles1) {
      ::SendMessage(nppData._nppHandle, NPPM_ACTIVATEDOC, MAIN_VIEW, (*iter));
    } else if (*iter >= nbopenfiles1 && *iter < nbopenfiles1+nbopenfiles2) {
      ::SendMessage(nppData._nppHandle, NPPM_ACTIVATEDOC, SUB_VIEW, (*iter)-nbopenfiles1);
    } else {
      return false;
    }

    ++(*iter);
    return true;
  } else {
    ++(*iter);
    return ((*iter) == 1);
  }
}

///////////////////////////////////////////////////////////////////////////////

void convertText2XML() {
  dbgln("convertText2XML()");

  int currentEdit, isReadOnly;
  ::SendMessage(nppData._nppHandle, NPPM_GETCURRENTSCINTILLA, 0, (LPARAM)&currentEdit);
  HWND hCurrentEditView = getCurrentHScintilla(currentEdit);

  isReadOnly = (int) ::SendMessage(hCurrentEditView, SCI_GETREADONLY, 0, 0);
  if (isReadOnly) return;

  size_t selstart = (size_t) ::SendMessage(hCurrentEditView, SCI_GETSELECTIONSTART, 0, 0);
  size_t selend = (size_t) ::SendMessage(hCurrentEditView, SCI_GETSELECTIONEND, 0, 0);
  size_t sellength = selend-selstart;

  if (selend <= selstart) {
    Report::_printf_err(L"Please select text to transform before you call the function.");
    return;
  }

  char *data = new char[sellength+1];
  if (!data) return;  // allocation error, abort check
  memset(data, '\0', sellength+1);

  ::SendMessage(hCurrentEditView, SCI_GETSELTEXT, 0, reinterpret_cast<LPARAM>(data));

  std::string str(data);
  delete [] data;
  data = NULL;
  std::string::size_type curpos;

  if (xmltoolsoptions.convertApos) {
    curpos = sellength;
    while (curpos != std::string::npos && (curpos = str.rfind("&apos;", curpos)) != std::string::npos) {
      if (curpos != std::string::npos) {
        str.replace(curpos, strlen("&apos;"), "'");
        sellength = sellength - strlen("&apos;") + strlen("'");
      }
      --curpos;
    }
  }

  if (xmltoolsoptions.convertQuote) {
    curpos = sellength;
    while (curpos != std::string::npos && (curpos = str.rfind("&quot;", curpos)) != std::string::npos) {
      if (curpos != std::string::npos) {
        str.replace(curpos, strlen("&quot;"), "\"");
        sellength = sellength - strlen("&quot;") + strlen("\"");
      }
      --curpos;
    }
  }

  if (xmltoolsoptions.convertLt) {
    curpos = sellength;
    while (curpos != std::string::npos && (curpos = str.rfind("&lt;", curpos)) != std::string::npos) {
      if (curpos != std::string::npos) {
        str.replace(curpos, strlen("&lt;"), "<");
        sellength = sellength - strlen("&lt;") + strlen("<");
      }
      --curpos;
    }
  }

  if (xmltoolsoptions.convertGt) {
    curpos = sellength;
    while (curpos != std::string::npos && (curpos = str.rfind("&gt;", curpos)) != std::string::npos) {
      if (curpos != std::string::npos) {
        str.replace(curpos, strlen("&gt;"), ">");
        sellength = sellength - strlen("&gt;") + strlen(">");
      }
      --curpos;
    }
  }

  if (xmltoolsoptions.convertAmp) {
    curpos = sellength;
    while (curpos != std::string::npos && (curpos = str.rfind("&amp;", curpos)) != std::string::npos) {
      if (curpos != std::string::npos) {
        str.replace(curpos, strlen("&amp;"), "&");
        sellength = sellength - strlen("&amp;") + strlen("&");
      }
      --curpos;
    }
  }

  // Replace the selection with new string
  ::SendMessage(hCurrentEditView, SCI_REPLACESEL, 0, reinterpret_cast<LPARAM>(str.c_str()));

  // Defines selection without scrolling
  ::SendMessage(hCurrentEditView, SCI_SETCURRENTPOS, selstart, 0);
  ::SendMessage(hCurrentEditView, SCI_SETANCHOR, selstart+sellength, 0);

  str.clear();
}

///////////////////////////////////////////////////////////////////////////////

void convertXML2Text() {
  dbgln("convertXML2Text()");

  int currentEdit, isReadOnly;
  ::SendMessage(nppData._nppHandle, NPPM_GETCURRENTSCINTILLA, 0, (LPARAM)&currentEdit);
  HWND hCurrentEditView = getCurrentHScintilla(currentEdit);

  isReadOnly = (int) ::SendMessage(hCurrentEditView, SCI_GETREADONLY, 0, 0);
  if (isReadOnly) return;

  size_t selstart = (size_t) ::SendMessage(hCurrentEditView, SCI_GETSELECTIONSTART, 0, 0);
  size_t selend = (size_t) ::SendMessage(hCurrentEditView, SCI_GETSELECTIONEND, 0, 0);
  size_t sellength = selend-selstart;

  if (selend <= selstart) {
    Report::_printf_err(L"Please select text to transform before you call the function.");
    return;
  }

  char *data = new char[sellength+1];
  if (!data) return;  // allocation error, abort check
  memset(data, '\0', sellength+1);

  ::SendMessage(hCurrentEditView, SCI_GETSELTEXT, 0, reinterpret_cast<LPARAM>(data));

  std::string str(data);
  delete [] data;
  data = NULL;
  std::string::size_type curpos;

  if (xmltoolsoptions.convertAmp) {
    curpos = sellength;
    while (curpos != std::string::npos && (curpos = str.rfind("&", curpos)) != std::string::npos) {
      if (curpos != std::string::npos) {
        str.replace(curpos, strlen("&"), "&amp;");
        sellength = sellength + strlen("&amp;") - strlen("&");
      }
      --curpos;
    }
  }

  if (xmltoolsoptions.convertLt) {
    curpos = sellength;
    while (curpos != std::string::npos && (curpos = str.rfind("<", curpos)) != std::string::npos) {
      if (curpos != std::string::npos) {
        str.replace(curpos, strlen("<"), "&lt;");
        sellength = sellength + strlen("&lt;") - strlen("<");
      }
      --curpos;
    }
  }

  if (xmltoolsoptions.convertGt) {
    curpos = sellength;
    while (curpos != std::string::npos && (curpos = str.rfind(">", curpos)) != std::string::npos) {
      if (curpos != std::string::npos) {
        str.replace(curpos, strlen(">"), "&gt;");
        sellength = sellength + strlen("&gt;") - strlen(">");
      }
      --curpos;
    }
  }

  if (xmltoolsoptions.convertQuote) {
    curpos = sellength;
    while (curpos != std::string::npos && (curpos = str.rfind("\"", curpos)) != std::string::npos) {
      if (curpos != std::string::npos) {
        str.replace(curpos, strlen("\""), "&quot;");
        sellength = sellength + strlen("&quot;") - strlen("\"");
      }
      --curpos;
    }
  }

  if (xmltoolsoptions.convertApos) {
    curpos = sellength;
    while (curpos != std::string::npos && (curpos = str.rfind("'", curpos)) != std::string::npos) {
      if (curpos != std::string::npos) {
        str.replace(curpos, strlen("'"), "&apos;");
        sellength = sellength + strlen("&apos;") - strlen("'");
      }
      --curpos;
    }
  }

  // Replace the selection with new string
  ::SendMessage(hCurrentEditView, SCI_REPLACESEL, 0, reinterpret_cast<LPARAM>(str.c_str()));

  // Defines selection without scrolling
  ::SendMessage(hCurrentEditView, SCI_SETCURRENTPOS, selstart, 0);
  ::SendMessage(hCurrentEditView, SCI_SETANCHOR, selstart+sellength, 0);

  str.clear();
}

///////////////////////////////////////////////////////////////////////////////

int validateSelectionForComment(std::string str, std::string::size_type sellength) {
  dbgln("validateSelectionForComment()");

  // Validate the selection
  std::stack<int> checkstack;
  std::string::size_type curpos = 0;
  int errflag = 0;
  while (curpos <= sellength && !errflag && (curpos = str.find_first_of("<-*", curpos)) != std::string::npos) {
    if (curpos > sellength) break;

    if (!str.compare(curpos, 4, "<!--")) {
      checkstack.push(0);
    }
    if (!str.compare(curpos, 3, "-->")) {
      if (!checkstack.empty()) {
        if (checkstack.top() != 0) errflag = checkstack.top();
        else checkstack.pop();
      } else {
        errflag = -3;
        break;
      }
    }
    if (!str.compare(curpos, 3, "<![")) {
      std::string::size_type endvalpos = str.find("]**", curpos);
      if (endvalpos != std::string::npos) checkstack.push(atoi(str.substr(curpos+3,endvalpos).c_str()));
    }
    if (!str.compare(curpos, 3, "**[")) {
      if (!checkstack.empty()) {
        std::string::size_type endvalpos = str.find("]>", curpos);
        if (endvalpos != std::string::npos && atoi(str.substr(curpos+3,endvalpos).c_str()) != checkstack.top()) errflag = -2;
        else checkstack.pop();
      } else {
        errflag = -4;
        break;
      }
    }
    ++curpos;
  }
  if (!checkstack.empty()) errflag = -1;

  return errflag;
}

void commentSelection() {
  dbgln("commentSelection()");

  long currentEdit;
  int isReadOnly;
  ::SendMessage(nppData._nppHandle, NPPM_GETCURRENTSCINTILLA, 0, (LPARAM)&currentEdit);
  HWND hCurrentEditView = getCurrentHScintilla(currentEdit);

  isReadOnly = (int) ::SendMessage(hCurrentEditView, SCI_GETREADONLY, 0, 0);
  if (isReadOnly) return;

  size_t selstart = (size_t) ::SendMessage(hCurrentEditView, SCI_GETSELECTIONSTART, 0, 0);
  size_t selend = (size_t) ::SendMessage(hCurrentEditView, SCI_GETSELECTIONEND, 0, 0);
  size_t sellength = selend-selstart;

  if (selend <= selstart) {
    Report::_printf_err(L"Please select text to transform before you call the function.");
    return;
  }

  char *data = new char[sellength+sizeof(char)];
  if (!data) return;  // allocation error, abort check
  memset(data, '\0', sellength+sizeof(char));

  ::SendMessage(hCurrentEditView, SCI_GETSELTEXT, 0, reinterpret_cast<LPARAM>(data));

  while (sellength >= 0 && (data[sellength] == '\r' || data[sellength] == '\n')) {
    data[sellength--] = '\0';
  }

  std::string str(data);
  delete [] data;
  data = NULL;

  int errflag = validateSelectionForComment(str, sellength);
  if (errflag != 0) {
    wchar_t msg[512];
    swprintf(msg, 512, L"The current selection covers part of another comment.\nUncomment process may be not applicable.\n\nDo you want to continue ? Error code %d", errflag);
    if (::MessageBox(nppData._nppHandle, msg, L"XML Tools plugin", MB_YESNO | MB_ICONASTERISK) == IDNO) {
      str.clear();
      return;
    }
  }

  std::string::size_type curpos = sellength;
  while (curpos != std::string::npos && (curpos = str.rfind("<!{", curpos)) != std::string::npos) {
    if (curpos != std::string::npos) {
      size_t endvalpos = str.find("}**", curpos);
      int endval = atoi(str.substr(curpos+3,endvalpos).c_str());
      char tmpstr[64];
      sprintf(tmpstr, "<!{%d}**", endval+1);
      str.replace(curpos,endvalpos-curpos+3,tmpstr);
      sellength += (strlen(tmpstr)-(endvalpos-curpos+3));
    }
    --curpos;
  }
  curpos = sellength;
  while (curpos != std::string::npos && (curpos = str.rfind("**{", curpos)) != std::string::npos) {
    if (curpos != std::string::npos) {
      size_t endvalpos = str.find("}>", curpos);
      int endval = atoi(str.substr(curpos+3,endvalpos).c_str());
      char tmpstr[64];
      sprintf(tmpstr, "**{%d}>", endval+1);
      str.replace(curpos,endvalpos-curpos+2,tmpstr);
      sellength += (strlen(tmpstr)-(endvalpos-curpos+2));
    }
    --curpos;
  }

  curpos = sellength;
  while (curpos != std::string::npos && (curpos = str.rfind("<!--", curpos)) != std::string::npos) {
    if (curpos != std::string::npos) {
      str.replace(curpos,4,"<!{1}**");
      sellength += 3;
    }
    --curpos;
  }
  curpos = sellength;
  while (curpos != std::string::npos && (curpos = str.rfind("-->", curpos)) != std::string::npos) {
    if (curpos != std::string::npos) {
      str.replace(curpos,3,"**{1}>");
      sellength += 3;
    }
    --curpos;
  }

  str.insert(0,"<!--"); sellength += 4;
  str.insert(sellength,"-->"); sellength += 3;

  // Replace the selection with new string
  ::SendMessage(hCurrentEditView, SCI_REPLACESEL, 0, reinterpret_cast<LPARAM>(str.c_str()));

  // Defines selection without scrolling
  ::SendMessage(hCurrentEditView, SCI_SETCURRENTPOS, selstart, 0);
  ::SendMessage(hCurrentEditView, SCI_SETANCHOR, selstart+sellength, 0);

  str.clear();
}

///////////////////////////////////////////////////////////////////////////////

void uncommentSelection() {
  dbgln("uncommentSelection()");

  long currentEdit;
  int isReadOnly;
  ::SendMessage(nppData._nppHandle, NPPM_GETCURRENTSCINTILLA, 0, (LPARAM)&currentEdit);
  HWND hCurrentEditView = getCurrentHScintilla(currentEdit);

  isReadOnly = (int) ::SendMessage(hCurrentEditView, SCI_GETREADONLY, 0, 0);
  if (isReadOnly) return;

  size_t selstart = (size_t) ::SendMessage(hCurrentEditView, SCI_GETSELECTIONSTART, 0, 0);
  size_t selend = (size_t) ::SendMessage(hCurrentEditView, SCI_GETSELECTIONEND, 0, 0);
  size_t sellength = selend-selstart;

  if (selend <= selstart) {
    Report::_printf_err(L"Please select text to transform before you call the function.");
    return;
  }

  char *data = new char[sellength+sizeof(char)];
  if (!data) return;  // allocation error, abort check
  memset(data, '\0', sellength+sizeof(char));

  ::SendMessage(hCurrentEditView, SCI_GETSELTEXT, 0, reinterpret_cast<LPARAM>(data));

  std::string str(data);
  delete [] data;
  data = NULL;

  int errflag = validateSelectionForComment(str, sellength);
  if (errflag != 0) {
    wchar_t msg[512];
    swprintf(msg, 512, L"Unable to uncomment the current selection.\nError code is %d.", errflag);
    Report::_printf_err(msg);
    str.clear();
    return;
  }

  // Proceed to uncomment
  std::string::size_type curpos = sellength;
  while (curpos != std::string::npos && (curpos = str.rfind("-->", curpos)) != std::string::npos) {
    if (curpos != std::string::npos) {
      str.erase(curpos,3);
      sellength -= 3;
    }
    --curpos;
  }
  curpos = sellength;
  while (curpos != std::string::npos && (curpos = str.rfind("<!--", curpos)) != std::string::npos) {
    if (curpos != std::string::npos) {
      str.erase(curpos,4);
      sellength -= 4;
    }
    --curpos;
  }

  curpos = sellength;
  while (curpos != std::string::npos && (curpos = str.rfind("<!{", curpos)) != std::string::npos) {
    if (curpos != std::string::npos) {
      size_t endvalpos = str.find("}**", curpos);
      int endval = atoi(str.substr(curpos+3,endvalpos).c_str());
      if (endval > 1) {
        char tmpstr[64];
        sprintf(tmpstr, "<!{%d}**", endval-1);
        str.replace(curpos,endvalpos-curpos+3,tmpstr);
        sellength += (strlen(tmpstr)-(endvalpos-curpos+3));
      } else {
        str.replace(curpos,endvalpos-curpos+3,"<!--");
        sellength += (4-(endvalpos-curpos+3));
      }
    }
    --curpos;
  }
  curpos = sellength;
  while (curpos != std::string::npos && (curpos = str.rfind("**{", curpos)) != std::string::npos) {
    if (curpos != std::string::npos) {
      size_t endvalpos = str.find("}>", curpos);
      int endval = atoi(str.substr(curpos+3,endvalpos).c_str());
      if (endval > 1) {
        char tmpstr[64];
        sprintf(tmpstr, "**{%d}>", endval-1);
        str.replace(curpos,endvalpos-curpos+2,tmpstr);
        sellength += (strlen(tmpstr)-(endvalpos-curpos+2));
      } else {
        str.replace(curpos,endvalpos-curpos+2,"-->");
        sellength += (3-(endvalpos-curpos+2));
      }
    }
    --curpos;
  }

  // Replace the selection with new string
  ::SendMessage(hCurrentEditView, SCI_REPLACESEL, 0, reinterpret_cast<LPARAM>(str.c_str()));

  // Defines selection without scrolling
  ::SendMessage(hCurrentEditView, SCI_SETCURRENTPOS, selstart, 0);
  ::SendMessage(hCurrentEditView, SCI_SETANCHOR, selstart+sellength, 0);

  str.clear();
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CXMLToolsApp object

CXMLToolsApp* theApp = new CXMLToolsApp();
