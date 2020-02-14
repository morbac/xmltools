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
  const int TOTAL_FUNCS = 34;
#else
  const int TOTAL_FUNCS = 33;
#endif
int nbFunc = TOTAL_FUNCS;

NppData nppData;
CDebugDlg* debugdlg = new CDebugDlg();

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
bool hasAnnotations = false;

// Here're the declaration my functions ///////////////////////////////////////
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
void prettyPrintXMLBreaks();
void prettyPrintText();
void prettyPrintSelection();
void prettyPrintAttributes();
//void insertPrettyPrintTag();
void linearizeXML();
void togglePrettyPrintAllFiles();
int initDocIterator();
bool hasNextDoc(int* iter);

void getCurrentXPath();
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

  Report::strcpy(funcItem[menuentry]._itemName, L"Pretty print (XML only)");
  funcItem[menuentry]._pFunc = prettyPrintXML;
  ++menuentry;

  Report::strcpy(funcItem[menuentry]._itemName, L"Pretty print (XML only - with line breaks)");
  registerShortcut(funcItem + menuentry, true, true, true, 'B');
  funcItem[menuentry]._pFunc = prettyPrintXMLBreaks;
  ++menuentry;

  Report::strcpy(funcItem[menuentry]._itemName, L"Pretty print (Text indent)");
  funcItem[menuentry]._pFunc = prettyPrintText;
  ++menuentry;

  Report::strcpy(funcItem[menuentry]._itemName, L"Pretty print (attributes)");
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
  xmltoolsoptions.useAnnotations = (::GetPrivateProfileInt(sectionName, L"useAnnotations", 0, iniFilePath) == 1);
  xmltoolsoptions.annotationStyle = ::GetPrivateProfileInt(sectionName, L"annotationStyle", 34, iniFilePath);

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
      if (notifyCode->modificationType & (SC_MOD_INSERTTEXT | SC_MOD_DELETETEXT)) {
        dbgln(Report::str_format("NPP Event: SCN_MODIFIED [%d]", notifyCode->modificationType).c_str());
        clearAnnotations();
      }
      break;
    }
    case NPPN_FILEOPENED:
      dbgln("NPP Event: NPPN_FILEOPENED");
      TCHAR filename[MAX_PATH];
      ::SendMessage(nppData._nppHandle, NPPM_GETFULLPATHFROMBUFFERID, notifyCode->nmhdr.idFrom, reinterpret_cast<LPARAM>(filename));
      dbg("  bufferID: "); dbgln(std::to_string(static_cast<unsigned long long>(notifyCode->nmhdr.idFrom)).c_str());
      dbg("  filename: "); dbgln(filename);
      break;
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

// tricky way of centering text on given line
// we get the contol size and estimate the number of lines based on text height
// then we force centering by going to wanted line +- nlines/2
void centerOnLine(HWND view, size_t line, size_t ofs) {
  try {
    RECT rect;
    GetClientRect(view, &rect);
    int height = (int) ::SendMessage(view, SCI_TEXTHEIGHT, line, NULL);
    int nlines_2 = (rect.bottom - rect.top) / (2 * height);
    int last = (int) ::SendMessage(view, SCI_GETMAXLINESTATE, NULL, NULL);

    // force uncollapse target line and ensure line is visible
    ::SendMessage(view, SCI_ENSUREVISIBLE, line - 1, NULL);
    ::SendMessage(view, SCI_SETFIRSTVISIBLELINE, line - 1, NULL);

    // center on line
    if (line - 1 + nlines_2 > last) {
      // line is on the end of document
      ::SendMessage(view, SCI_SHOWLINES, line - 1, line + ofs); // force surround lines visibles if folded
    } else {
     ::SendMessage(view, SCI_GOTOLINE, line - 1 - nlines_2, 0);
     ::SendMessage(view, SCI_GOTOLINE, line - 1 + nlines_2, 0);
    }

    ::SendMessage(view, SCI_GOTOLINE, line - 1, 0);
  } catch (...) {}
}

void displayXMLError(std::wstring wmsg, HWND view, size_t line, size_t filepos) {
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

    // display error as an annotation

    //char* styles = new char[wmsg.length()];
    //memset(styles, xmltoolsoptions.annotationStyle, wmsg.length());
    //memset(styles, 0, wmsg.find(L"\r\n")+1);

    switch (encoding) {
      case uniCookie:
      case uniUTF8:
      case uni16BE:
      case uni16LE:
        // utf-8
        ::SendMessage(view, SCI_ANNOTATIONSETTEXT, line - 1, reinterpret_cast<LPARAM>(Report::ucs2ToUtf8(wmsg.c_str()).c_str()));
        break;
      default:
        // ansi
        ::SendMessage(view, SCI_ANNOTATIONSETTEXT, line - 1, reinterpret_cast<LPARAM>(Report::ws2s(wmsg).c_str()));
        break;
    }

    //::SendMessage(view, SCI_ANNOTATIONSETSTYLES, line - 1, reinterpret_cast<LPARAM>(styles));
    ::SendMessage(view, SCI_ANNOTATIONSETSTYLE, line - 1, xmltoolsoptions.annotationStyle);
    ::SendMessage(view, SCI_ANNOTATIONSETVISIBLE, 2, NULL);
    hasAnnotations = true;

    centerOnLine(view, line, std::count(wmsg.begin(), wmsg.end(), '\n'));

    if (filepos > 0) {
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
    displayXMLError(Report::str_format(L"%s - line %d, pos %d: \r\n%s", szDesc, line, linepos, bstrReason), view, line, filepos);
  } else {
    displayXMLError(Report::str_format(L"XML Parsing error - line %d, pos %d: \r\n%s", line, linepos, bstrReason), view, line, filepos);
  }

CleanUp:
  SysFreeString(bstrReason);
}

void clearAnnotations(HWND view) {
  if (view == NULL) {
    int currentEdit;
    ::SendMessage(nppData._nppHandle, NPPM_GETCURRENTSCINTILLA, 0, (LPARAM)&currentEdit);
    view = getCurrentHScintilla(currentEdit);
  }
  if (hasAnnotations && xmltoolsoptions.useAnnotations) {
    ::SendMessage(view, SCI_ANNOTATIONCLEARALL, NULL, NULL);
    hasAnnotations = false;
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

  /*
  xmlDocPtr doc;
  xmlNodePtr rootnode;
  xmlSchemaPtr schema;
  xmlSchemaValidCtxtPtr vctxt;
  xmlSchemaParserCtxtPtr pctxt;
  xmlDtdPtr dtdPtr;
  bool doFreeDTDPtr = false;
  bool xsdValidation = false;
  bool dtdValidation = false;

  pXmlResetLastError();
  //updateProxyConfig();
  doc = pXmlReadMemory(data, currentLength, "noname.xml", NULL, getFlags());

  delete [] data;
  data = NULL;

  if (doc == NULL) {
    xmlErrorPtr err = pXmlGetLastError();

    if (err != NULL) {
      if (err->line > 0) {
        ::SendMessage(hCurrentEditView, SCI_GOTOLINE, err->line-1, 0);
      }
      wchar_t* tmpmsg = Report::castChar(err->message);
      Report::_printf_err(L"XML Parsing error at line %d: \r\n%s", err->line, tmpmsg);
      delete[] tmpmsg;
    } else {
      Report::_printf_err(L"Failed to parse document");
    }

    abortValidation = true;
  }

  // 2. is xml is valid...
  if (!abortValidation) {
    // 2.1. try to get schema or dtd in document
    rootnode = pXmlDocGetRootElement(doc);
    if (rootnode == NULL) {
      Report::_printf_err(L"Empty XML document");
    } else {
      // 2.1.a. search for attributes "xsi:noNamespaceSchemaLocation" or "xsi:schemaLocation" in root tag
      // expected root tag sample:
      //   <descript xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xsi:noNamespaceSchemaLocation="Descript_Shema.xsd">
      xmlChar* propval = pXmlGetProp(rootnode, reinterpret_cast<const unsigned char*>("noNamespaceSchemaLocation"));
      if (propval == NULL) {
        propval = pXmlGetProp(rootnode, reinterpret_cast<const unsigned char*>("schemaLocation"));
        if (propval != NULL) {
          xml_schema = std::string(reinterpret_cast<const char*>(propval));
          xml_schema = xml_schema.substr(1+xml_schema.find_last_of(' '));
          xsdValidation = true;
        }
      } else {
        xml_schema = std::string(reinterpret_cast<const char*>(propval));
        xsdValidation = true;
      }

      // 2.1.b search for DOCTYPE before root tag
      dtdPtr = doc->intSubset;
      if (dtdPtr) {
        std::string dtd_filename("");
        if (dtdPtr->SystemID) {
          dtd_filename = std::string(reinterpret_cast<const char*>((LPCTSTR)dtdPtr->SystemID));
        } else if (dtdPtr->ExternalID) {
          dtd_filename = std::string(reinterpret_cast<const char*>((LPCTSTR)dtdPtr->ExternalID));
        }

        if (dtdPtr->SystemID || dtdPtr->ExternalID) {
          dtdPtr = pXmlParseDTD(dtdPtr->ExternalID, dtdPtr->SystemID);
          doFreeDTDPtr = true;
        }

        if (dtdPtr == NULL) {
          wchar_t* tmpmsg = Report::castChar(dtd_filename.c_str());
          Report::_printf_err(L"Unable to load the DTD\r\n%s", tmpmsg);
          delete[] tmpmsg;
          abortValidation = true;
        } else {
          dtdValidation = true;
        }
      }
      propval = NULL;
    }

    // elements check: if we have both a DTD and a schema, we use the schema
    if (xsdValidation) dtdValidation = false;
  }

  if (!abortValidation) {
    // 2.2. if attribute is missing, let's ask to user to provide an xsd path
    if (xml_schema.length() == 0 && !dtdValidation) {
      if (pSelectFileDlg == NULL) {
          pSelectFileDlg = new CSelectFileDlg();
      }
      pSelectFileDlg->m_sSelectedFilename = Report::widen(lastXMLSchema).c_str();

      CStringW rootSample = "<";
      Report::appendToCString(&rootSample, rootnode->name, encoding);
      rootSample += "\r\n\txmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"";
      rootSample += "\r\n\txsi:noNamespaceSchemaLocation=\"XSD_FILE_PATH\">";

      pSelectFileDlg->m_sRootElementSample = rootSample;
      if (pSelectFileDlg->DoModal() == IDOK) {
        xml_schema = Report::narrow(std::wstring(pSelectFileDlg->m_sSelectedFilename).c_str());
      }
    }

    // 2.3.a. schema validation process
    if (xml_schema.length() > 0 && !dtdValidation) {
      // concat provided path wiht current path
      wchar_t destpath[MAX_PATH] = { '\0' };
      ::SendMessage(nppData._nppHandle, NPPM_GETCURRENTDIRECTORY, MAX_PATH, (LPARAM)destpath);
      std::string xml_absolute_schema = Report::narrow(destpath);
      xml_absolute_schema.append("\\");
      xml_absolute_schema.append(xml_schema);

      // if the file exists, let's use it, otherwise let's use the initial value (for the case we get an URL)
      if (PathFileExists(Report::widen(xml_absolute_schema).c_str()) == FALSE) {
        xml_absolute_schema = xml_schema;
      }

      if ((pctxt = pXmlSchemaNewParserCtxt(reinterpret_cast<const char*>(xml_absolute_schema.c_str()))) == NULL) {
        Report::_printf_err(L"Unable to initialize parser.");
      } else {
        // load content of xml schema
        schema = pXmlSchemaParse(pctxt);

        if (schema == NULL) {
          xmlErrorPtr err;
          err = pXmlGetLastError();

          if (err != NULL) {
            wchar_t* tmpmsg = Report::castChar(err->message);
            if (err->line > 0) {
              ::SendMessage(hCurrentEditView, SCI_GOTOLINE, err->line-1, 0);
              Report::_printf_err(L"Unable to parse schema file. \r\nParsing error at line %d: \r\n%s", err->line, tmpmsg);
            } else {
              Report::_printf_err(L"Following error occurred during schema file parsing: \r\n%s", tmpmsg);
            }
            delete[] tmpmsg;
          } else {
            Report::_printf_err(L"Unable to parse schema file.");
          }
        } else {
          // create validation context
          if ((vctxt = pXmlSchemaNewValidCtxt(schema)) == NULL) {
            Report::_printf_err(L"Unable to create validation context.");
          } else {
            // handle validation errors
            Report::clearLog();
            Report::registerMessage("Validation of current file using XML schema:\r\n\r\n");
            pXmlSchemaSetValidErrors(vctxt, (xmlSchemaValidityErrorFunc) Report::registerError, (xmlSchemaValidityWarningFunc) Report::registerWarn, stderr);
            //pXmlSchemaValidateSetLocator(vctxt, xmlSchemaValidateStreamLocator, pctxt);

            // validation
            if (!pXmlSchemaValidateDoc(vctxt, doc)) {
              if (informIfNoError) Report::_printf_inf(L"XML Schema validation:\r\nXML is valid.");
            } else {
              CMessageDlg* msgdlg = new CMessageDlg();
              msgdlg->m_sMessage = Report::getLog().c_str();
              msgdlg->DoModal();
            }
          }

          // 2.4. free parser
          pXmlSchemaFree(schema);
          pXmlSchemaFreeValidCtxt(vctxt);
        }

        pXmlSchemaFreeParserCtxt(pctxt);
      }
    }

    // 2.3.b dtd validation process
    if (dtdPtr && dtdValidation) {
      xmlValidCtxtPtr vctxt;

      // create validation context
      if ((vctxt = pXmlNewValidCtxt())) {
        // show validation errors
        Report::clearLog();
        Report::registerMessage("Validation of current file using DTD:\r\n\r\n");
        vctxt->userData = (void *) stderr;
        vctxt->error = (xmlValidityErrorFunc) Report::registerError;
        vctxt->warning = (xmlValidityWarningFunc) Report::registerWarn;

        // validation
        if (pXmlValidateDtd(vctxt, doc, dtdPtr)) {
          if (pXmlValidateDtdFinal(vctxt, doc)) {
            if (informIfNoError) Report::_printf_inf(L"DTD validation:\r\nXML is valid.");
          } else {
            CMessageDlg* msgdlg = new CMessageDlg();
            msgdlg->m_sMessage = Report::getLog().c_str();
            msgdlg->DoModal();
          }
        } else {
          CMessageDlg* msgdlg = new CMessageDlg();
          msgdlg->m_sMessage = Report::getLog().c_str();
          msgdlg->DoModal();
        }
        // free memory
        pXmlFreeValidCtxt(vctxt);
      }
      if (doFreeDTDPtr) {
        pXmlFreeDtd(dtdPtr);
      }
    }
  }

  // 3. clean memory
  rootnode = NULL;
  pXmlFreeDoc(doc);

  // 4. save schema name for next call
  lastXMLSchema = xml_schema;
  */
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

    PathBuilder pPB;

    CHK_HR(CreateAndInitSAX(&pRdr));
    pRdr->putContentHandler(&pPB);
    pRdr->parse(varXML);

    nodepath = pPB.getPath();
    pRdr->Release();
    /*
    //updateProxyConfig();
    xmlDocPtr doc = pXmlReadMemory(str.c_str(), str.length(), "noname.xml", NULL, XML_PARSE_RECOVER | getFlags());
    str.clear();

    if (doc == NULL) return nodepath;

    UniMode encoding = Report::getEncoding(doc->encoding, NULL);
    xmlNodePtr cur_node = pXmlDocGetRootElement(doc);
    std::wstring attr(L"");

    while (cur_node != NULL && cur_node->last != NULL) {
      if (cur_node->type == XML_ELEMENT_NODE) {
        attr = L"";
        nodepath += L"/";
        if (cur_node->ns) {
          if (cur_node->ns->prefix != NULL) {
            Report::appendToWStdString(&nodepath, cur_node->ns->prefix, encoding);
            nodepath += L":";
          }
        }
        Report::appendToWStdString(&nodepath, cur_node->name, encoding);

        // count preceding siblings
        if (preciseXPath) {
          int pos = 1;
          xmlNodePtr n = cur_node->prev;
          while (n) {
            if (n->type == XML_ELEMENT_NODE && pXmlStrEqual(n->name, cur_node->name)) ++pos;
            n = n->prev;
          }
          nodepath += Report::str_format(L"[%d]", pos).c_str();
        }

        // get last attribute
        if (cursorInAttribute) {
          xmlAttrPtr a = cur_node->properties;
          while (a) {
            if (a->type == XML_ATTRIBUTE_NODE) {
              attr = Report::widen(a->name);
              a = a->next;
            }
          }
        }
      }
      cur_node = cur_node->last;
    }

    if (cursorInAttribute && attr.length() > 0) {
      nodepath += Report::str_format(L"/@%s", attr.c_str()).c_str();
    }
    pXmlFreeDoc(doc);*/
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

void prettyPrint(bool autoindenttext, bool addlinebreaks) {
  dbgln("prettyPrint()");

  int docIterator = initDocIterator();
  while (hasNextDoc(&docIterator)) {
    int currentEdit, currentLength, isReadOnly;
    ::SendMessage(nppData._nppHandle, NPPM_GETCURRENTSCINTILLA, 0, (LPARAM)&currentEdit);
    HWND hCurrentEditView = getCurrentHScintilla(currentEdit);

    isReadOnly = (int) ::SendMessage(hCurrentEditView, SCI_GETREADONLY, 0, 0);
    if (isReadOnly) return;

    char *data = NULL;

    // count the < and > signs; > are ignored if tagsignlevel <= 0. This prevent indent errors if text or attributes contain > sign.
    int tagsignlevel = 0;
    // some state variables
    bool in_comment = false, in_header = false, in_attribute = false, in_nodetext = false, in_cdata = false, in_text = false;

    // some counters
    std::string::size_type curpos = 0, strlength = 0, prevspecchar, nexwchar_t, deltapos, tagend, startprev, endnext;
    long xmllevel = 0;
    // some char value (pc = previous char, cc = current char, nc = next char, nnc = next next char)
    char pc, cc, nc, nnc;

    int tabwidth = (int) ::SendMessage(hCurrentEditView, SCI_GETTABWIDTH, 0, 0);
    bool usetabs = (bool) ::SendMessage(hCurrentEditView, SCI_GETUSETABS, 0, 0);
    if (tabwidth <= 0) tabwidth = 4;

    bool isclosingtag;

    // use the selection
    long selstart = 0, selend = 0;
    // désactivé : le fait de prettyprinter que la sélection pose problème pour l'indentation
    // il faudrait calculer l'indentation de la 1re ligne de sélection, mais l'indentation
    // de cette ligne n'est peut-être pas correcte. On pourrait la déterminer en récupérant
    // le path de la banche sélectionnée...
    selstart = (long) ::SendMessage(hCurrentEditView, SCI_GETSELECTIONSTART, 0, 0);
    selend = (long) ::SendMessage(hCurrentEditView, SCI_GETSELECTIONEND, 0, 0);

    std::string str("");
    std::string eolchar;
    int eolmode;
    Report::getEOLChar(hCurrentEditView, &eolmode, &eolchar);

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

    // Check de la syntaxe (disabled)
/*
    if (FALSE) {
      //updateProxyConfig();
      xmlDocPtr doc = pXmlReadMemory(data, currentLength, "noname.xml", NULL, getFlags());
      if (doc == NULL) {
        xmlErrorPtr err;
        err = pXmlGetLastError();

        if (err != NULL) {
          if (err->line > 0) {
            ::SendMessage(hCurrentEditView, SCI_GOTOLINE, err->line-1, 0);
          }
          wchar_t* tmpmsg = Report::castChar(err->message);
          Report::_printf_err(L"Errors detected in content. Please correct them before applying pretty print.\nLine %d: \r\n%s", err->line, tmpmsg);
          delete[] tmpmsg;
        } else {
          Report::_printf_err(L"Errors detected in content. Please correct them before applying pretty print.");
        }

        delete [] data;
        return;
      }

      pXmlFreeDoc(doc);
    }
*/

    str += data;
    delete[] data;
    data = NULL;

    // Proceed to first pass if break adds are enabled
    if (addlinebreaks) {
      while (curpos < str.length() && (curpos = str.find_first_of("<>",curpos)) != std::string::npos) {
        cc = str.at(curpos);

        if (cc == '<' && curpos < str.length()-4 && !str.compare(curpos,4,"<!--")) {
          // Let's skip the comment
          curpos = str.find("-->",curpos+1)+2;
          // Adds a line break after comment if required
          nexwchar_t = str.find_first_not_of(" \t",curpos+1);
          if (nexwchar_t != std::string::npos && str.at(nexwchar_t) == '<') {
            str.insert(++curpos,eolchar);
          }
        } else if (cc == '<' && curpos < str.length()-9 && !str.compare(curpos,9,"<![CDATA[")) {
          // Let's skip the CDATA block
          curpos = str.find("]]>",curpos+1)+2;
          // Adds a line break after CDATA if required
          nexwchar_t = str.find_first_not_of(" \t",curpos+1);
          if (nexwchar_t != std::string::npos && str.at(nexwchar_t) == '<') {
            str.insert(++curpos,eolchar);
          }
        } else if (cc == '>') {
          // Let's see if '>' is a end tag char (it might also be simple text)
          // To perform test, we search last of "<>". If it is a '>', current '>' is
          // simple text, if not, it is a end tag char. '>' text chars are ignored.
          prevspecchar = str.find_last_of("<>", curpos - 1);
          if (prevspecchar != std::string::npos) {
            // let's see if our '>' is in attribute
            std::string::size_type nextt = str.find_first_of("\"'", prevspecchar + 1);
            if (str.at(prevspecchar) == '>' && (nextt == std::string::npos || nextt > curpos)) {
              // current > is simple text, in text node
              ++curpos;
              continue;
            }
            nextt = str.find_first_of("<>", curpos + 1);
            if (nextt != std::string::npos && str.at(nextt) == '>') {
              // current > is text in attribute
              ++curpos;
              continue;
            }

            // We have found a '>' char and we are in a tag, let's see if it's an opening or closing one
            isclosingtag = ( (curpos > 0 && str.at(curpos - 1) == '/') || str.at(prevspecchar+1) == '/');

            nexwchar_t = str.find_first_not_of(" \t", curpos + 1);
            deltapos = nexwchar_t - curpos;
            // Test below identifies a <x><y> case and excludes <x>abc<y> case; in second case, we won't add line break
            if (nexwchar_t != std::string::npos && str.at(nexwchar_t) == '<' && curpos < str.length() - (deltapos + 1)) {
              // we compare previous and next tags; if they are same, we don't add line break
              startprev = str.rfind("<", curpos);
              endnext = str.find(">", nexwchar_t);

              if (startprev != std::string::npos && endnext != std::string::npos && curpos > startprev && endnext > nexwchar_t) {
                tagend = str.find_first_of(" />", startprev + 1);
                std::string tag1(str.substr(startprev + 1, tagend - startprev - 1));
                tagend = str.find_first_of(" />", nexwchar_t + 2);
                std::string tag2(str.substr(nexwchar_t + 2, tagend - nexwchar_t - 2));
                if (strcmp(tag1.c_str(), tag2.c_str()) || isclosingtag) {
                  // Case of "<data><data>..." -> add a line break between tags
                  str.insert(++curpos, eolchar);
                } else if (str.at(nexwchar_t + 1) == '/' && !isclosingtag && nexwchar_t == curpos + 1) {
                  // Case of "<data id="..."></data>" -> replace by "<data id="..."/>"
                  str.replace(curpos, endnext - curpos, "/");
                }
              }
            }
          }
        }

        ++curpos;           // go to next char
      }
/*
      while (curpos < str.length()-2 && (curpos = str.find("><",curpos)) != std::string::npos) {
        // we compare previous and next tags; if they are same, we don't add line break
        startprev = str.rfind("<",curpos);
        endnext = str.find(">",curpos+1);

        if (startprev != std::string::npos &&
            endnext != std::string::npos &&
            curpos > startprev &&
            endnext > curpos+1 &&
            strcmp(str.substr(startprev+1,curpos-startprev-1).c_str(),
                   str.substr(curpos+3,endnext-curpos-3).c_str()))
          str.insert(++curpos,"\n");

        ++curpos;// go to next char
      }
*/

      // reinitialize cursor pos for second pass
      curpos = 0;
    }

    // Proceed to reformatting (second pass)
    prevspecchar = std::string::npos;
    std::string sep("<>\"'");
    char attributeQuote = '\0';
    sep += eolchar;
    strlength = str.length();
     while (curpos < strlength && (curpos = str.find_first_of(sep,curpos)) != std::string::npos) {
      if (!Report::isEOL(str, strlength, (unsigned int) curpos, eolmode)) {
        if (curpos < strlength-4 && !str.compare(curpos,4,"<!--")) {
          in_comment = true;
        }
        if (curpos < strlength-9 && !str.compare(curpos,9,"<![CDATA[")) {
          in_cdata = true;
        } else if (curpos < strlength-2 && !str.compare(curpos,2,"<?")) {
          in_header = true;
        } else if (!in_comment && !in_cdata && !in_header &&
                   curpos < strlength && (str.at(curpos) == '\"' || str.at(curpos) == '\'')) {
          if (in_attribute && attributeQuote != '\0' && str.at(curpos) == attributeQuote && prevspecchar != std::string::npos && str.at(prevspecchar) == '<') {
            // attribute end
            in_attribute = false;
            attributeQuote = '\0';  // store the attribute quote char to detect the end of attribute
          } else if (!in_attribute) {
            std::string::size_type x = str.find_last_not_of(" \t", curpos-1);
            std::string::size_type tagbeg = str.find_last_of("<>", x-1);
            std::string::size_type tagend = str.find_first_of("<>", curpos+1);
            if (x != std::string::npos && tagbeg != std::string::npos && tagend != std::string::npos &&
                str.at(x) == '=' && str.at(tagbeg) == '<' && str.at(tagend) == '>') {
              in_attribute = true;
              attributeQuote = str.at(curpos);  // store the attribute quote char to detect the end of attribute
            }
          }
        }
      }

      if (!in_comment && !in_cdata && !in_header) {
        if (curpos > 0) {
          pc = str.at(curpos-1);
        } else {
          pc = ' ';
        }

        cc = str.at(curpos);

        if (curpos < strlength-1) {
          nc = str.at(curpos+1);
        } else {
          nc = ' ';
        }

        if (curpos < strlength-2) {
          nnc = str.at(curpos+2);
        } else {
          nnc = ' ';
        }

        // managing of case where '>' char is present in text content
        in_text = false;
        if (cc == '>') {
          std::string::size_type tmppos = str.find_last_of("<>\"'", curpos - 1);
          // skip attributes which can contain > char
          while (tmppos != std::string::npos && (str.at(tmppos) == '\"' || str.at(tmppos) == '\'')) {
            tmppos = str.find_last_of("=", tmppos - 1);
            tmppos = str.find_last_of("<>\"'", tmppos - 1);
          }
          in_text = (tmppos != std::string::npos && str.at(tmppos) == '>');
        }

        if (cc == '<') {
          prevspecchar = curpos++;
          ++tagsignlevel;
          in_nodetext = false;
          if (nc != '/' && (nc != '!' || nnc == '[')) {
            xmllevel += 2;
          }
        } else if (cc == '>' && !in_attribute && !in_text) {
          // > are ignored inside attributes
          if (pc != '/' && pc != ']') {
            --xmllevel;
            in_nodetext = true;
          } else {
            xmllevel -= 2;
          }

          if (xmllevel < 0) {
            xmllevel = 0;
          }
          --tagsignlevel;
          prevspecchar = curpos++;
        } else if (Report::isEOL(cc, nc, eolmode)) {
          if (eolmode == SC_EOL_CRLF) {
            ++curpos; // skipping \n of \r\n
          }

          // \n are ignored inside attributes
          nexwchar_t = str.find_first_not_of(" \t",++curpos);

          bool skipformat = false;
          if (!autoindenttext && nexwchar_t != std::string::npos) {
            cc = str.at(nexwchar_t);
            skipformat = (cc != '<' && cc != '\r' && cc != '\n');
          }
          if (nexwchar_t >= curpos && xmllevel >= 0 && !skipformat) {
            if (nexwchar_t < 0) {
              nexwchar_t = curpos;
            }
            size_t delta = 0;
            str.erase(curpos,nexwchar_t-curpos);
            strlength = str.length();

            if (curpos < strlength) {
              cc = str.at(curpos);
              // we set delta = 1 if we technically can + if we are in a text or inside an attribute
              if (xmllevel > 0 && curpos < strlength-1 && ( (cc == '<' && str.at(curpos+1) == '/') || in_attribute) ) {
                delta = 1;
              } else if (cc == '\n' || cc == '\r') {
                delta = xmllevel;
              }
            }

            if (usetabs) {
              str.insert(curpos,xmllevel-delta,'\t');
            } else {
              str.insert(curpos,tabwidth*(xmllevel-delta),' ');
            }
            strlength = str.length();
          }
        } else {
          ++curpos;
        }
      } else {
        if (in_comment && curpos > 1 && !str.compare(curpos-2,3,"-->")) {
          in_comment = false;
        } else if (in_cdata && curpos > 1 && !str.compare(curpos-2,3,"]]>")) {
          in_cdata = false;
        } else if (in_header && curpos > 0 && !str.compare(curpos-1,2,"?>")) {
          in_header = false;
        }
        ++curpos;
      }
    }

    // Send formatted string to scintilla
    if (selend > selstart) {
      ::SendMessage(hCurrentEditView, SCI_REPLACESEL, 0, reinterpret_cast<LPARAM>(str.c_str()));
    } else {
      ::SendMessage(hCurrentEditView, SCI_SETTEXT, 0, reinterpret_cast<LPARAM>(str.c_str()));
    }

    str.clear();

    // Put scroll at the left of the view
    ::SendMessage(hCurrentEditView, SCI_SETXOFFSET, 0, 0);
  }
}

void prettyPrintXML() {
  dbgln("prettyPrintXML()");

  prettyPrint(false, false);
}

void prettyPrintXMLBreaks() {
  dbgln("prettyPrintXMLBreaks()");

  prettyPrint(false, true);
}

void prettyPrintText() {
  dbgln("prettyPrintText()");

  prettyPrint(true, false);
}

void prettyPrintAttributes() {
  #ifdef __XMLTOOLS_DEBUG__
    Report::_printf_inf("prettyPrint()");
  #endif

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
  std::string::size_type curpos = sellength;

  while (curpos != std::string::npos && (curpos = str.rfind("&quot;", curpos)) != std::string::npos) {
    if (curpos != std::string::npos) {
      str.replace(curpos, strlen("&quot;"), "\"");
      sellength = sellength - strlen("&quot;") + strlen("\"");
    }
    --curpos;
  }
  curpos = sellength;
  while (curpos != std::string::npos && (curpos = str.rfind("&lt;", curpos)) != std::string::npos) {
    if (curpos != std::string::npos) {
      str.replace(curpos, strlen("&lt;"), "<");
      sellength = sellength - strlen("&lt;") + strlen("<");
    }
    --curpos;
  }
  curpos = sellength;
  while (curpos != std::string::npos && (curpos = str.rfind("&gt;", curpos)) != std::string::npos) {
    if (curpos != std::string::npos) {
      str.replace(curpos, strlen("&gt;"), ">");
      sellength = sellength - strlen("&gt;") + strlen(">");
    }
    --curpos;
  }
  curpos = sellength;
  while (curpos != std::string::npos && (curpos = str.rfind("&amp;", curpos)) != std::string::npos) {
    if (curpos != std::string::npos) {
      str.replace(curpos, strlen("&amp;"), "&");
      sellength = sellength - strlen("&amp;") + strlen("&");
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
  std::string::size_type curpos = sellength;

  while (curpos != std::string::npos && (curpos = str.rfind("&", curpos)) != std::string::npos) {
    if (curpos != std::string::npos) {
      str.replace(curpos, strlen("&"), "&amp;");
      sellength = sellength + strlen("&amp;") - strlen("&");
    }
    --curpos;
  }
  curpos = sellength;
  while (curpos != std::string::npos && (curpos = str.rfind("<", curpos)) != std::string::npos) {
    if (curpos != std::string::npos) {
      str.replace(curpos, strlen("<"), "&lt;");
      sellength = sellength + strlen("&lt;") - strlen("<");
    }
    --curpos;
  }
  curpos = sellength;
  while (curpos != std::string::npos && (curpos = str.rfind(">", curpos)) != std::string::npos) {
    if (curpos != std::string::npos) {
      str.replace(curpos, strlen(">"), "&gt;");
      sellength = sellength + strlen("&gt;") - strlen(">");
    }
    --curpos;
  }
  curpos = sellength;
  while (curpos != std::string::npos && (curpos = str.rfind("\"", curpos)) != std::string::npos) {
    if (curpos != std::string::npos) {
      str.replace(curpos, strlen("\""), "&quot;");
      sellength = sellength + strlen("&quot;") - strlen("\"");
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
