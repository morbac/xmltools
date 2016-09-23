// XMLTools.cpp : Defines the initialization routines for the DLL.
//

// notepad++
#include "stdafx.h"
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

// libxml
//#include "LoadLibrary.h"
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xmlschemas.h>
#include <libxml/globals.h>
#include <libxml/nanohttp.h>
#include <libxml/xmlerror.h>
#include <libxml/parser.h>

// curl
#include <curl/curl.h>

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

unsigned long defFlags = XML_PARSE_NOENT | XML_PARSE_DTDLOAD;
unsigned long defFlagsNoXXE = 0;

// XML Loading status
int libloadstatus = 0;

// INI file support
wchar_t iniFilePath[MAX_PATH];
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
     doPrettyPrintAllOpenFiles = false,
     doCheckUpdates = true;

std::wstring dateOfNextCheckUpdates(L"19800101");
struct struct_proxyoptions proxyoptions;

int menuitemCheckXML = -1,
    menuitemValidation = -1,
    /*menuitemPrettyPrint = -1,*/
    menuitemCloseTag = -1,
    menuitemAutoIndent = -1,
    menuitemAttrAutoComplete = -1,
    menuitemAutoXMLType = -1,
    menuitemPreventXXE = -1,
    menuitemPrettyPrintAllFiles = -1,
    menuitemCheckUpdates = -1;

std::string lastXMLSchema("");

int nbopenfiles1, nbopenfiles2;

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

void setAutoXMLType();
void insertAutoXMLType();

void togglePreventXXE();

void prettyPrintXML();
void prettyPrintXMLBreaks();
void prettyPrintText();
void prettyPrintSelection();
void prettyPrintLibXML();
void prettyPrintAttributes();
//void insertPrettyPrintTag();
void linarizeXML();
void togglePrettyPrintAllFiles();
int initDocIterator();
bool hasNextDoc(int* iter);

void getCurrentXPath();
void evaluateXPath();

void performXSLTransform();

void convertXML2Text();
void convertText2XML();

void commentSelection();
void uncommentSelection();

void checkUpdates();
void toggleCheckUpdates();

void aboutBox();
void optionsDlg();
void howtoUse();
void updateProxyConfig();
void dbg(CString line);
void dbgln(CString line);
void debugDlg();

int performXMLCheck(int informIfNoError);
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
  dbg("libXML: "); dbgln(LIBXML_DOTTED_VERSION);
  dbg("libXSLT: "); dbgln(LIBXSLT_DOTTED_VERSION);

  dbg("Locating XMLToolsExt.ini... ");
  wchar_t nppMainPath[MAX_PATH], appDataPath[MAX_PATH], nppPluginsPath[MAX_PATH]; 
  GetModuleFileName(::GetModuleHandle(XMLTOOLS_DLLNAME), nppPluginsPath, sizeof(nppPluginsPath));
  PathRemoveFileSpec(nppPluginsPath);   // remove the module name: get npp plugins path
  
  Report::strcpy(nppMainPath, nppPluginsPath);
  PathRemoveFileSpec(nppMainPath);      // cd..: get npp executable path
  
  // Make localConf.xml path
  wchar_t localConfPath[MAX_PATH];
  Report::strcpy(localConfPath, nppMainPath);
  PathAppend(localConfPath, localConfFile);
  //Report::_printf_err("%s", localConfPath);
  
  // Test if localConf.xml exist
  bool isLocal = (PathFileExists(localConfPath) == TRUE);
  
  ITEMIDLIST *pidl;
  SHGetSpecialFolderLocation(NULL, CSIDL_APPDATA, &pidl);
  SHGetPathFromIDList(pidl, appDataPath);

  if (isLocal) {
    // try NPP plugins local configuration path (standard NPP location)
    Report::strcpy(iniFilePath, nppPluginsPath);
    PathAppend(iniFilePath, L"Config\\XMLToolsExt.ini");

    if (PathFileExists(iniFilePath) == FALSE) {
      // try NPP plugins path
      Report::strcpy(iniFilePath, nppPluginsPath);
      PathAppend(iniFilePath, L"XMLToolsExt.ini");

      if (PathFileExists(iniFilePath) == FALSE) {
        // try NPP main path
        Report::strcpy(iniFilePath, nppMainPath);
        PathAppend(iniFilePath, L"XMLToolsExt.ini");
      }
    }
  } else {
    // try NPP plugins remote configuration path (standard NPP UAC/AppData location)
    Report::strcpy(iniFilePath, appDataPath);
    PathAppend(iniFilePath, L"Notepad++\\XMLToolsExt.ini");
  }

  dbgln(iniFilePath);

  // chargement de la librairie
  dbg("Loading libraries... ");
  //libloadstatus = loadLibraries(nppMainPath, appDataPath);
  //if (libloadstatus < 0) nbFunc = 1;

  int menuentry = 0;
  for (int i = 0; i < nbFunc; ++i) {
    funcItem[i]._init2Check = false;
  }

  if (!libloadstatus) {
    dbgln("OK");

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
    registerShortcut(funcItem+menuentry, true, true, true, 'M');
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
  
    funcItem[menuentry++]._pFunc = NULL;  //----------------------------------------
  
    Report::strcpy(funcItem[menuentry]._itemName, L"Pretty print (XML only)");
    funcItem[menuentry]._pFunc = prettyPrintXML;
    ++menuentry;
  
    Report::strcpy(funcItem[menuentry]._itemName, L"Pretty print (XML only - with line breaks)");
    registerShortcut(funcItem+menuentry, true, true, true, 'B');
    funcItem[menuentry]._pFunc = prettyPrintXMLBreaks;
    ++menuentry;
  
    Report::strcpy(funcItem[menuentry]._itemName, L"Pretty print (Text indent)");
    funcItem[menuentry]._pFunc = prettyPrintText;
    ++menuentry;
  
    Report::strcpy(funcItem[menuentry]._itemName, L"Pretty print (libXML) [experimental]");
    funcItem[menuentry]._pFunc = prettyPrintLibXML;
    ++menuentry;
  
    Report::strcpy(funcItem[menuentry]._itemName, L"Pretty print (attributes)");
    registerShortcut(funcItem+menuentry, true, true, true, 'A');
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
    Report::strcpy(funcItem[menuentry]._itemName, L"Linarize XML");
    registerShortcut(funcItem+menuentry, true, true, true, 'L');
    funcItem[menuentry]._pFunc = linarizeXML;
    ++menuentry;

    Report::strcpy(funcItem[menuentry]._itemName, L"Apply to all open files");
    funcItem[menuentry]._pFunc = togglePrettyPrintAllFiles;
    funcItem[menuentry]._init2Check = (::GetPrivateProfileInt(sectionName, L"doPrettyPrintAllOpenFiles", 0, iniFilePath) != 0);
    doPrettyPrintAllOpenFiles = funcItem[menuentry]._init2Check;
    menuitemPrettyPrintAllFiles = menuentry;
    ++menuentry;
  
    funcItem[menuentry++]._pFunc = NULL;  //----------------------------------------
  
    Report::strcpy(funcItem[menuentry]._itemName, L"Current XML Path");
    registerShortcut(funcItem+menuentry, true, true, true, 'P');
    funcItem[menuentry]._pFunc = getCurrentXPath;
    ++menuentry;
  
    Report::strcpy(funcItem[menuentry]._itemName, L"Evaluate XPath expression");
    funcItem[menuentry]._pFunc = evaluateXPath;
    ++menuentry;
  
    funcItem[menuentry++]._pFunc = NULL;  //----------------------------------------
  
    Report::strcpy(funcItem[menuentry]._itemName, L"XSL Transformation");
    funcItem[menuentry]._pFunc = performXSLTransform;
    ++menuentry;
  
    funcItem[menuentry++]._pFunc = NULL;  //----------------------------------------
  
    Report::strcpy(funcItem[menuentry]._itemName, L"Convert selection XML to text (<> => &&lt;&&gt;)");
    funcItem[menuentry]._pFunc = convertXML2Text;
    ++menuentry;
  
    Report::strcpy(funcItem[menuentry]._itemName, L"Convert selection text to XML (&&lt;&&gt; => <>)");
    funcItem[menuentry]._pFunc = convertText2XML;
    ++menuentry;
  
    funcItem[menuentry++]._pFunc = NULL;  //----------------------------------------
  
    Report::strcpy(funcItem[menuentry]._itemName, L"Comment selection");
    registerShortcut(funcItem+menuentry, true, true, true, 'C');
    funcItem[menuentry]._pFunc = commentSelection;
    ++menuentry;
  
    Report::strcpy(funcItem[menuentry]._itemName, L"Uncomment selection");
    registerShortcut(funcItem+menuentry, true, true, true, 'R');
    funcItem[menuentry]._pFunc = uncommentSelection;
    ++menuentry;
  
    funcItem[menuentry++]._pFunc = NULL;  //----------------------------------------

    Report::strcpy(funcItem[menuentry]._itemName, L"Check for plugin updates on startup");
    funcItem[menuentry]._pFunc = toggleCheckUpdates;
	  funcItem[menuentry]._init2Check = (::GetPrivateProfileInt(sectionName, L"doCheckUpdates", 1, iniFilePath) != 0);
	  doCheckUpdates = funcItem[menuentry]._init2Check;
	  menuitemCheckUpdates = menuentry;
	  ++menuentry;
  
    Report::strcpy(funcItem[menuentry]._itemName, L"Options...");
    funcItem[menuentry]._pFunc = optionsDlg;
    ++menuentry;

    #ifdef _DEBUG
      Report::strcpy(funcItem[menuentry]._itemName, L"Debug window...");
      funcItem[menuentry]._pFunc = debugDlg;
      ++menuentry;
    #endif
  
    Report::strcpy(funcItem[menuentry]._itemName, L"About XML Tools");
    funcItem[menuentry]._pFunc = aboutBox;
    ++menuentry;

    // Load proxy settings in ini file
    proxyoptions.status = (::GetPrivateProfileInt(sectionName, L"proxyEnabled", 0, iniFilePath) == 1);
    ::GetPrivateProfileString(sectionName, L"proxyHost", L"192.168.0.1", proxyoptions.host, 255, iniFilePath);
    proxyoptions.port = ::GetPrivateProfileInt(sectionName, L"proxyPort", 8080, iniFilePath);
    ::GetPrivateProfileString(sectionName, L"proxyUser", L"", proxyoptions.username, 255, iniFilePath);
    ::GetPrivateProfileString(sectionName, L"proxyPass", L"", proxyoptions.password, 255, iniFilePath);

    updateProxyConfig();
  } else {
    dbgln("ERROR");

    Report::strcpy(funcItem[menuentry]._itemName, L"How to install...");
    funcItem[menuentry]._pFunc = howtoUse;
    ++menuentry;
  }

  assert(menuentry == nbFunc);

  //Report::_printf_inf("menu entries: %d", menuentry);

/*
  Report::_printf_inf("%s\ndoCheckXML: %d %d\ndoValidation: %d %d\ndoCloseTag: %d %d\ndoAutoXMLType: %d %d\ndoPreventXXE: %d %d\nisLocal: %d",
    iniFilePath,
    doCheckXML, funcItem[menuitemCheckXML]._init2Check,
    doValidation, funcItem[menuitemValidation]._init2Check,
    doCloseTag, funcItem[menuitemCloseTag]._init2Check,
    doAutoXMLType, funcItem[menuitemAutoXMLType]._init2Check,
    doPreventXXE, funcItem[menuitemPreventXXE]._init2Check,
    isLocal);
*/

  dbgln("Initialization finished.");
}

CXMLToolsApp::~CXMLToolsApp() {
  savePluginParams();

  // Don't forget to de-allocate your shortcut here
  for (int i = 0; i < nbFunc; ++i) {
    if (funcItem[i]._pShKey) delete funcItem[i]._pShKey;
  }
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
  funcItem[menuitemPrettyPrintAllFiles]._init2Check = doPrettyPrintAllOpenFiles;
  funcItem[menuitemCheckUpdates]._init2Check = doCheckUpdates;

  ::WritePrivateProfileString(sectionName, L"doCheckXML", doCheckXML?L"1":L"0", iniFilePath);
  ::WritePrivateProfileString(sectionName, L"doValidation", doValidation?L"1":L"0", iniFilePath);
  //::WritePrivateProfileString(sectionName, L"doPrettyPrint", doPrettyPrint?L"1":L"0", iniFilePath);
  ::WritePrivateProfileString(sectionName, L"doCloseTag", doCloseTag?L"1":L"0", iniFilePath);
  //::WritePrivateProfileString(sectionName, L"doAutoIndent", doAutoIndent?L"1":L"0", iniFilePath);
  //::WritePrivateProfileString(sectionName, L"doAttrAutoComplete", doAttrAutoComplete?L"1":L"0", iniFilePath);
  ::WritePrivateProfileString(sectionName, L"doAutoXMLType", doAutoXMLType?L"1":L"0", iniFilePath);
  ::WritePrivateProfileString(sectionName, L"doPreventXXE", doPreventXXE?L"1":L"0", iniFilePath);
  ::WritePrivateProfileString(sectionName, L"doPrettyPrintAllOpenFiles", doPrettyPrintAllOpenFiles?L"1":L"0", iniFilePath);
  ::WritePrivateProfileString(sectionName, L"doCheckUpdates", doCheckUpdates?L"1":L"0", iniFilePath);

  ::WritePrivateProfileString(sectionName, L"dateOfNextCheck", dateOfNextCheckUpdates.c_str(), iniFilePath);
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
}

// The getName function tells Notepad++ plugins system its name
extern "C" __declspec(dllexport) const wchar_t * getName() {
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
  if (libloadstatus != 0) return;

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
        ::CheckMenuItem(hMenu, funcItem[menuitemPrettyPrintAllFiles]._cmdID, MF_BYCOMMAND | (doPrettyPrintAllOpenFiles?MF_CHECKED:MF_UNCHECKED));
        ::CheckMenuItem(hMenu, funcItem[menuitemCheckUpdates]._cmdID, MF_BYCOMMAND | (doCheckUpdates?MF_CHECKED:MF_UNCHECKED));

        checkUpdates();
        #ifdef DEBUG
          debugdlg->Create(CDebugDlg::IDD,NULL);
          debugDlg();
        #endif
      }
    }
    case NPPN_FILEBEFORESAVE: {
      dbgln("NPP Event: NPPN_FILEBEFORESAVE");
      LangType docType;
      ::SendMessage(nppData._nppHandle, NPPM_GETCURRENTLANGTYPE, 0, (LPARAM)&docType);
      if (docType == L_XML) {
        // comme la validation XSD effectue �galement un check de syntaxe, on n'ex�cute
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
      if (docType == L_XML) {
        // remarque: le closeXMLTag doit s'ex�cuter avant les autres
        if (doCloseTag && notifyCode->ch == '>') closeXMLTag();
        //if (doAutoIndent && notifyCode->ch == '\n') tagAutoIndent();
        //if (doAttrAutoComplete && notifyCode->ch == '\"') attributeAutoComplete();
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
        // si le fichier n'a pas de type d�fini et qu'il commence par "<?xml ", on lui attribue le type L_XML
        LangType docType = L_EXTERNAL;
        ::SendMessage(nppData._nppHandle, NPPM_GETCURRENTLANGTYPE, 0, (LPARAM)&docType);
        dbg("  Current langtype: "); dbgln(std::to_string(static_cast<unsigned long long>(docType)).c_str());
        //Report::_printf_inf("%s", getLangType(docType));
        if (docType != L_XML) {
          setAutoXMLType();
        }
/*
        if (doPrettyPrint) {
	      LangType docType = L_EXTERNAL;
           ::SendMessage(nppData._nppHandle, NPPM_GETCURRENTLANGTYPE, 0, (LPARAM)&docType);
           //Report::_printf_inf("%s", getLangType(docType));
           if (docType == L_XML) prettyPrintLibXML();
        }
*/
      }
      break;
    }
    /*default: {
      dbg("NPP Event: "); dbgln(std::to_string(notifyCode->nmhdr.code).c_str());
    }*/
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

size_t curlWriteData(void *ptr, size_t size, size_t nmemb, void *stream) {
  size_t pos = 0;
  size_t total = size * nmemb;
  char* tptr = (char*)ptr;

  // Don't allow result strings over 1k
  if (reinterpret_cast<std::wstring*>(stream)->size() > 1024)
    return 0;

  while (pos < total)	{
    reinterpret_cast<std::wstring*>(stream)->push_back(*tptr);
    tptr++;
    ++pos;
  }

  return total;
}

std::wstring getLatestVersion(const char* url) {
  CURL *curl;

  curl= curl_easy_init();
  if (curl) {
    std::wstring result;
    //curl_easy_setopt(curl, CURLOPT_FRESH_CONNECT, TRUE);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "XMLTools");
    curl_easy_setopt(curl, CURLOPT_URL, url);

    if (proxyoptions.status && wcslen(proxyoptions.host) > 0) {
		  curl_easy_setopt(curl, CURLOPT_PROXY, Report::wchar2char(proxyoptions.host));
      curl_easy_setopt(curl, CURLOPT_PROXYPORT, proxyoptions.port);

/*
    if (wcslen(proxyoptions.username) > 0) {
      curl_easy_setopt(curl, CURLOPT_PROXYAUTH, CURLAUTH_ANY);
      curl_easy_setopt(curl, CURLOPT_PROXYUSERNAME, Report::wchar2char(proxyoptions.username));
      if (wcslen(proxyoptions.password) > 0) {
        curl_easy_setopt(curl, CURLOPT_PROXYPASSWORD, Report::wchar2char(proxyoptions.password));
      }
    }
*/
	} else {
	  curl_easy_setopt(curl, CURLOPT_PROXY, NULL);
	  curl_easy_setopt(curl, CURLOPT_PROXYPORT, NULL);
//    curl_easy_setopt(curl, CURLOPT_PROXYUSERNAME, NULL);
//    curl_easy_setopt(curl, CURLOPT_PROXYUSERPWD, NULL);
//    curl_easy_setopt(curl, CURLOPT_PROXYAUTH, CURLAUTH_NONE);
    }
 
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curlWriteData);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &result);
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);

    CURLcode rescode = curl_easy_perform(curl);
    long httpCode = 0;
    curl_easy_getinfo(curl, CURLINFO_HTTP_CODE, &httpCode);

    curl_easy_cleanup(curl);

    if (httpCode != 200) {
      result = L"";
    }

    return result;
  }

  return L"";
}

struct version{
  version(std::wstring versionStr) {
    std::size_t p = versionStr.find_first_of(L"\r\n");
    if (p != std::string::npos) {
      swscanf(versionStr.substr(0, p).c_str(), L"%d.%d.%d", &major, &minor, &revision);
    } else {
      swscanf(versionStr.c_str(), L"%d.%d.%d", &major, &minor, &revision);
    }
  }
  bool operator < (const version &other) {
    if (major < other.major) return true;
    if (minor < other.minor) return true;
    if (revision < other.revision) return true;
    return false;
  }
  int major,minor,revision;
};

void checkUpdates() {
  dbgln("checkUpdates()");

  // Lets check if an online check is required
  wchar_t next[80];
  ::GetPrivateProfileString(sectionName, L"dateOfNextCheck", L"19800101", next, 80, iniFilePath);
  dateOfNextCheckUpdates = next;

  if (doCheckUpdates) {
    time_t now = time(0);
    struct tm*  tstruct = localtime(&now);

    wchar_t buf[80];
    wcsftime(buf, sizeof(buf), L"%Y%m%d", tstruct);

    if (wcscmp(next, buf) < 0) {
      // lets download the latest stable version number
      std::wstring latestavailable = getLatestVersion(LATEST_STABLE_URL);
      if (!latestavailable.empty()) {
        // compare current number and latest stable version
        if (version(XMLTOOLS_VERSION_NUMBER) < version(latestavailable.c_str())) {
          CMessageDlg* dlg = new CMessageDlg();
          std::wstring msg(L"A new release of XMLTools plugin is available on NPP Plugins project.\r\nCurrent installed version: ");
          msg += XMLTOOLS_VERSION_NUMBER;
          msg += L" (";
          msg += XMLTOOLS_VERSION_STATUS;
          msg += L")";
          msg += L"\r\nVersions available online on https://sourceforge.net/projects/npp-plugins/files/XML%20Tools/ :\r\n\r\n";
          msg += latestavailable;
          dlg->m_sMessage = msg.c_str();
          dlg->DoModal();
        }
      }

      // Compute next check date
      tstruct->tm_mday += NDAYS_BETWEEN_UPDATE_CHECK;
      mktime(tstruct);
      wcsftime(buf, sizeof(buf), L"%Y%m%d", tstruct);
      dateOfNextCheckUpdates = buf;

      savePluginParams();
    }
  }
}

void toggleCheckUpdates() {
  dbgln("toggleCheckUpdates()");

  doCheckUpdates = !doCheckUpdates;
  ::CheckMenuItem(::GetMenu(nppData._nppHandle), funcItem[menuitemCheckUpdates]._cmdID, MF_BYCOMMAND | (doCheckUpdates?MF_CHECKED:MF_UNCHECKED));
  savePluginParams();
}

void optionsDlg() {
  dbgln("optionsDlg()");

  COptionsDlg* dlg = new COptionsDlg(NULL, &proxyoptions);
  if (dlg->DoModal() == IDOK) {
    ::WritePrivateProfileString(sectionName, L"proxyEnabled", proxyoptions.status?L"1":L"0", iniFilePath);
    ::WritePrivateProfileString(sectionName, L"proxyHost", proxyoptions.host, iniFilePath);
    ::WritePrivateProfileString(sectionName, L"proxyPort", std::to_wstring(static_cast<long long>(proxyoptions.port)).c_str(), iniFilePath);
    ::WritePrivateProfileString(sectionName, L"proxyUser", proxyoptions.username, iniFilePath);
    ::WritePrivateProfileString(sectionName, L"proxyPass", proxyoptions.password, iniFilePath);

    updateProxyConfig();
  }
}

void debugDlg() {
  debugdlg->ShowWindow(SW_SHOW);
}

void dbg(CString line) {
  #ifdef DEBUG
    debugdlg->addLine(line);
  #endif
}

void dbgln(CString line) {
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
    //proxyurl += "/";
    xmlNanoHTTPScanProxy(proxyurl.c_str());  // http://toto:admin@127.0.0.1:8080
  } else {
    xmlNanoHTTPScanProxy(NULL);
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

int performXMLCheck(int informIfNoError) {
  dbgln("performXMLCheck()");

  int currentEdit, currentLength, res = 0;
  ::SendMessage(nppData._nppHandle, NPPM_GETCURRENTSCINTILLA, 0, (LPARAM)&currentEdit);
  HWND hCurrentEditView = getCurrentHScintilla(currentEdit);
  currentLength = (int) ::SendMessage(hCurrentEditView, SCI_GETLENGTH, 0, 0);

  char *data = new char[currentLength+1];
  if (!data) return -1;  // allocation error, abort check
  memset(data, '\0', currentLength+1);

  ::SendMessage(hCurrentEditView, SCI_GETTEXT, currentLength+1, reinterpret_cast<LPARAM>(data));
  
  xmlResetLastError();
  //updateProxyConfig();
  xmlDocPtr doc = xmlReadMemory(data, currentLength, "noname.xml", NULL, (doPreventXXE ? defFlagsNoXXE : defFlags));
  
  delete [] data;
  data = NULL;
  
  xmlErrorPtr err = xmlGetLastError();

  std::wstring nfomsg(L"No error detected.");
  std::wstring errmsg(L"");

  if (doc == NULL || err != NULL) {
    if (err != NULL) {
      if (err->line > 0) {
        ::SendMessage(hCurrentEditView, SCI_GOTOLINE, err->line-1, 0);
      }
      wchar_t* tmpmsg = Report::castChar(err->message);
      errmsg += Report::str_format(L"XML Parsing error at line %d: \r\n%s", err->line, tmpmsg);
      delete[] tmpmsg;
      res = 1;
    } else if (doc == NULL) {
      errmsg += L"Failed to parse document";
      res = 2;
    }
  }
  
  xmlFreeDoc(doc);

  if (errmsg.length() > 0) {
    Report::_printf_err(errmsg.c_str());
  } else if (nfomsg.length() > 0 && informIfNoError) {
    Report::_printf_inf(nfomsg.c_str());
  }

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

  // 0. On change le dossier courant
  wchar_t currenPath[MAX_PATH] = { '\0' };
  ::SendMessage(nppData._nppHandle, NPPM_GETCURRENTDIRECTORY, MAX_PATH, (LPARAM)currenPath);
  _chdir(Report::narrow(currenPath).data());

  // 1. On valide le XML
  bool abortValidation = false;
  std::string xml_schema("");
  int currentEdit, currentLength;
  ::SendMessage(nppData._nppHandle, NPPM_GETCURRENTSCINTILLA, 0, (LPARAM)&currentEdit);
  HWND hCurrentEditView = getCurrentHScintilla(currentEdit);
  currentLength = (int) ::SendMessage(hCurrentEditView, SCI_GETLENGTH, 0, 0);
  
  char *data = new char[currentLength+1];
  if (!data) return;  // allocation error, abort check
  memset(data, '\0', currentLength+1);

  ::SendMessage(hCurrentEditView, SCI_GETTEXT, currentLength+1, reinterpret_cast<LPARAM>(data));

  UniMode encoding = Report::getEncoding();
  
  xmlDocPtr doc;
  xmlNodePtr rootnode;
  xmlSchemaPtr schema;
  xmlSchemaValidCtxtPtr vctxt;
  xmlSchemaParserCtxtPtr pctxt;
  xmlDtdPtr dtdPtr;
  bool doFreeDTDPtr = false;
  bool xsdValidation = false;
  bool dtdValidation = false;

  xmlResetLastError();
  //updateProxyConfig();
  doc = xmlReadMemory(data, currentLength, "noname.xml", NULL, (doPreventXXE ? defFlagsNoXXE : defFlags));

  delete [] data;
  data = NULL;

  if (doc == NULL) {
    xmlErrorPtr err = xmlGetLastError();
    
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

  // 2. Si le XMl est valide
  if (!abortValidation) {
    // 2.1. On essaie de retrouver le sch�ma ou la dtd das le document
    rootnode = xmlDocGetRootElement(doc);
    if (rootnode == NULL) {
      Report::_printf_err(L"Empty XML document");
    } else {
      // 2.1.a. On recherche les attributs "xsi:noNamespaceSchemaLocation" ou "xsi:schemaLocation" dans la balise root
      // Exemple de balise root:
      //  <descript xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xsi:noNamespaceSchemaLocation="Descript_Shema.xsd">
      xmlChar* propval = xmlGetProp(rootnode, reinterpret_cast<const unsigned char*>("noNamespaceSchemaLocation"));
      if (propval == NULL) {
        propval = xmlGetProp(rootnode, reinterpret_cast<const unsigned char*>("schemaLocation"));
        if (propval != NULL) {
          xml_schema = std::string(reinterpret_cast<const char*>(propval));
          xml_schema = xml_schema.substr(1+xml_schema.find_last_of(' '));
          xsdValidation = true;
        }
      } else {
        xml_schema = std::string(reinterpret_cast<const char*>(propval));
        xsdValidation = true;
      }

      // 2.1.b On recheche un DOCTYPE avant la balise root
      dtdPtr = doc->intSubset;
      if (dtdPtr) {
        std::string dtd_filename("");
        if (dtdPtr->SystemID) {
          dtd_filename = std::string(reinterpret_cast<const char*>((LPCTSTR)dtdPtr->SystemID));
        } else if (dtdPtr->ExternalID) {
          dtd_filename = std::string(reinterpret_cast<const char*>((LPCTSTR)dtdPtr->ExternalID));
        }
        
        if (dtdPtr->SystemID || dtdPtr->ExternalID) {
          dtdPtr = xmlParseDTD(dtdPtr->ExternalID, dtdPtr->SystemID);
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

    // V�rification des �l�ments: si on a � la fois une DTD et un sch�ma, on prend le sch�ma
    if (xsdValidation) dtdValidation = false;
  }
  
  if (!abortValidation) {
    // 2.2. Si l'attribut est absent, on demande � l'utilisateur de fournir le chemin du fichier XSD
    if (xml_schema.length() == 0 && !dtdValidation) {
      if (pSelectFileDlg == NULL) {
          pSelectFileDlg = new CSelectFileDlg();
      }
      pSelectFileDlg->m_sSelectedFilename = Report::widen(lastXMLSchema).c_str();

      CString rootSample = "<";
	    Report::appendToCString(&rootSample, rootnode->name, encoding);
      rootSample += "\r\n\txmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"";
      rootSample += "\r\n\txsi:noNamespaceSchemaLocation=\"XSD_FILE_PATH\">";

      pSelectFileDlg->m_sRootElementSample = rootSample;
      if (pSelectFileDlg->DoModal() == IDOK) {
        xml_schema = Report::narrow(std::wstring(pSelectFileDlg->m_sSelectedFilename).c_str());
      }
    }

    // 2.3.a. On proc�de � la validation par sch�ma
    if (xml_schema.length() > 0 && !dtdValidation) {
      // on concat�ne le chemin du document courant 
      wchar_t destpath[MAX_PATH] = { '\0' };
      ::SendMessage(nppData._nppHandle, NPPM_GETCURRENTDIRECTORY, MAX_PATH, (LPARAM)destpath);
      std::string xml_absolute_schema = Report::narrow(destpath);
      xml_absolute_schema.append("\\");
      xml_absolute_schema.append(xml_schema);

      // si le fichier existe, on le garde, sinon on reprend la valeur initiale (permet de g�rer le cas o� on re�oit une URL)
      if (PathFileExists(Report::widen(xml_absolute_schema).c_str()) == FALSE) {
        xml_absolute_schema = xml_schema;
      }

      if ((pctxt = xmlSchemaNewParserCtxt(reinterpret_cast<const char*>(xml_absolute_schema.c_str()))) == NULL) {
        Report::_printf_err(L"Unable to initialize parser.");
      } else {
        // Chargement du contenu du XML Schema
        schema = xmlSchemaParse(pctxt);

        if (schema == NULL) {
          xmlErrorPtr err;
          err = xmlGetLastError();

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
          // Cr�ation du contexte de validation
          if ((vctxt = xmlSchemaNewValidCtxt(schema)) == NULL) {
            Report::_printf_err(L"Unable to create validation context.");
          } else {
            // Traitement des erreurs de validation
            Report::clearLog();
            Report::registerMessage("Validation of current file using XML schema:\r\n\r\n");
            xmlSchemaSetValidErrors(vctxt, (xmlSchemaValidityErrorFunc) Report::registerError, (xmlSchemaValidityWarningFunc) Report::registerWarn, stderr);

            // Validation
            if (!xmlSchemaValidateDoc(vctxt, doc)) {
              if (informIfNoError) Report::_printf_inf(L"XML Schema validation:\r\nXML is valid.");
            } else {
              CMessageDlg* msgdlg = new CMessageDlg();
              msgdlg->m_sMessage = Report::getLog().c_str();
              msgdlg->DoModal();
            }
          }

          // 2.4. On lib�re le parseur
          xmlSchemaFree(schema);
          xmlSchemaFreeValidCtxt(vctxt);
		    }

		    xmlSchemaFreeParserCtxt(pctxt);
      }
    }

    // 2.3.b On proc�de � la validation par DTD
    if (dtdPtr && dtdValidation) {
      xmlValidCtxtPtr vctxt;

      // Cr�ation du contexte de validation
      if ((vctxt = xmlNewValidCtxt())) {
        // Affichage des erreurs de validation
        Report::clearLog();
        Report::registerMessage("Validation of current file using DTD:\r\n\r\n");
        vctxt->userData = (void *) stderr;
        vctxt->error = (xmlValidityErrorFunc) Report::registerError;
        vctxt->warning = (xmlValidityWarningFunc) Report::registerWarn;

        // Validation
        if (xmlValidateDtd(vctxt, doc, dtdPtr)) {
          if (informIfNoError) Report::_printf_inf(L"DTD validation:\r\nXML is valid.");
        } else {
          CMessageDlg* msgdlg = new CMessageDlg();
          msgdlg->m_sMessage = Report::getLog().c_str();
          msgdlg->DoModal();
        }
        // Lib�ration de la m�moire
        xmlFreeValidCtxt(vctxt);
      }
      if (doFreeDTDPtr) {
        xmlFreeDtd(dtdPtr);
      }
    }
  }

  // 3. On lib�re la m�moire
  rootnode = NULL;
  xmlFreeDoc(doc);

  // 4. On enregistre le nom du sch�ma utilis� pour le proposer la prochaine fois
  lastXMLSchema = xml_schema;
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
  // au m�me niveau et il n'y a pas d'indentation � faire)
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

  int tabwidth = ::SendMessage(hCurrentEditView, SCI_GETTABWIDTH, 0, 0);
  int usetabs = ::SendMessage(hCurrentEditView, SCI_GETUSETABS, 0, 0);
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
      // on r�cup�re l'indentation actuelle
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

      // on a trouv� le <, il reste � ins�rer une indentation apr�s le curseur
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

void setAutoXMLType() {
  dbgln("setAutoXMLType()");

  int currentEdit;
  ::SendMessage(nppData._nppHandle, NPPM_GETCURRENTSCINTILLA, 0, (LPARAM)&currentEdit);
  HWND hCurrentEditView = getCurrentHScintilla(currentEdit);

  // on r�cup�re les 6 premiers caract�res du fichier
  char head[8] = { '\0' };
  ::SendMessage(hCurrentEditView, SCI_GETTEXT, 7, reinterpret_cast<LPARAM>(&head));

  if (strlen(head) >= 6 && !strcmp(head, "<?xml ")) {
    ::SendMessage(nppData._nppHandle, NPPM_SETCURRENTLANGTYPE, 0, (LPARAM) L_XML);
  }
}

///////////////////////////////////////////////////////////////////////////////

std::wstring currentXPath() {
  dbgln("currentXPath()");

  int currentEdit;
  std::string::size_type currentLength, currentPos;
  ::SendMessage(nppData._nppHandle, NPPM_GETCURRENTSCINTILLA, 0, (LPARAM)&currentEdit);
  HWND hCurrentEditView = getCurrentHScintilla(currentEdit);
  currentLength = (int) ::SendMessage(hCurrentEditView, SCI_GETLENGTH, 0, 0);
  
  std::wstring nodepath(L"");

  char *data = new char[currentLength+1];
  if (!data) return nodepath;  // allocation error, abort check
  size_t size = (currentLength+1)*sizeof(char);
  memset(data, '\0', size);

  currentPos = long(::SendMessage(hCurrentEditView, SCI_GETCURRENTPOS, 0, 0));
  ::SendMessage(hCurrentEditView, SCI_GETTEXT, currentLength+1, reinterpret_cast<LPARAM>(data));

  std::string str(data);
  delete [] data;
  data = NULL;

  // end tag pos
  std::string::size_type begpos = str.find_first_of("<");
  std::string::size_type endpos = str.find_last_of(">");

  // let's reach the end of current tag (if we are inside a tag)
  if (currentPos > begpos && currentPos <= endpos) {
    currentPos = str.find_last_of("<>", currentPos-1)+1;

    // check if we are in a closing tag
    if (currentPos >= 1 && currentPos < currentLength-2 && str.at(currentPos-1) == '<'  && str.at(currentPos) == '!' && str.at(currentPos+1) == '-' && str.at(currentPos+2) == '-') {  // check if in a comment
      return nodepath;
    } else if (currentPos >= 1 && str.at(currentPos-1) == '<' && str.at(currentPos) == '/') {
      // if we are inside closing tag (inside </x>, let's go back before '<' char so we are inside node)
      --currentPos;
    } else {
      // let's get the end of current tag or text
      currentPos = str.find_first_of("<>", currentPos);
      // if inside a auto-closing tag (ex. '<x/>'), let's go back before '/' char, the '>' is added before slash)
      if (currentPos > 0 && str.at(currentPos-1) == '/'
                         && str.at(currentPos)   == '>') --currentPos;
    }

    str.erase(currentPos);
    str += "><X>";

    //updateProxyConfig();
    xmlDocPtr doc = xmlReadMemory(str.c_str(), str.length(), "noname.xml", NULL, XML_PARSE_RECOVER | (doPreventXXE ? defFlagsNoXXE : defFlags));
    str.clear();

    if (doc == NULL) return nodepath;

    UniMode encoding = Report::getEncoding(doc->encoding, NULL);
    xmlNodePtr cur_node = xmlDocGetRootElement(doc);

    while (cur_node != NULL && cur_node->last != NULL) {
      if (cur_node->type == XML_ELEMENT_NODE) {
        nodepath += L"/";
        if (cur_node->ns) {
          if (cur_node->ns->prefix != NULL) {
            Report::appendToWStdString(&nodepath, cur_node->ns->prefix, encoding);
            nodepath += L":";
          }
        }
        Report::appendToWStdString(&nodepath, cur_node->name, encoding);
      }
      cur_node = cur_node->last;
    }
    xmlFreeDoc(doc);
  }

  return nodepath;
}

void getCurrentXPath() {
  dbgln("getCurrentXPath()");
  
  std::wstring nodepath(currentXPath());
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

///////////////////////////////////////////////////////////////////////////////

#if defined(LIBXML_XPATH_ENABLED) && defined(LIBXML_SAX1_ENABLED)

int  execute_xpath_expression(const xmlChar* xpathExpr, const xmlChar* nsList);
int  register_namespaces(xmlXPathContextPtr xpathCtx, const xmlChar* nsList);
void print_xpath_nodes(xmlNodeSetPtr nodes);
CXPathEvalDlg *pXPathEvalDlg = NULL;

void evaluateXPath() {
  dbgln("evaluateXPath()");

  if (pXPathEvalDlg == NULL) {
    pXPathEvalDlg = new CXPathEvalDlg(NULL, (doPreventXXE ? defFlagsNoXXE : defFlags));
    pXPathEvalDlg->Create(CXPathEvalDlg::IDD,NULL);
  }
  pXPathEvalDlg->ShowWindow(SW_SHOW);
}

#else
void evaluateXPath() {
  dbgln("evaluateXPath()");
  Report::_printf_err("Function not available.");
}

#endif

///////////////////////////////////////////////////////////////////////////////

CXSLTransformDlg *pXSLTransformDlg = NULL;
void performXSLTransform() {
  dbgln("performXSLTransform()");

  if (pXSLTransformDlg == NULL) {
    pXSLTransformDlg = new CXSLTransformDlg(NULL, (doPreventXXE ? defFlagsNoXXE : defFlags));
    pXSLTransformDlg->Create(CXSLTransformDlg::IDD,NULL);
  }
  pXSLTransformDlg->ShowWindow(SW_SHOW);
}

///////////////////////////////////////////////////////////////////////////////

void prettyPrint(bool autoindenttext, bool addlinebreaks) {
  dbgln("prettyPrint()");

  int docIterator = initDocIterator();
  while (hasNextDoc(&docIterator)) {
    int currentEdit, currentLength, isReadOnly, xOffset, yOffset;
    ::SendMessage(nppData._nppHandle, NPPM_GETCURRENTSCINTILLA, 0, (LPARAM)&currentEdit);
    HWND hCurrentEditView = getCurrentHScintilla(currentEdit);

    isReadOnly = (int) ::SendMessage(hCurrentEditView, SCI_GETREADONLY, 0, 0);
    if (isReadOnly) return;
    
    xOffset = (int) ::SendMessage(hCurrentEditView, SCI_GETXOFFSET, 0, 0);
    yOffset = (int) ::SendMessage(hCurrentEditView, SCI_GETFIRSTVISIBLELINE, 0, 0);

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

    int tabwidth = ::SendMessage(hCurrentEditView, SCI_GETTABWIDTH, 0, 0);
    int usetabs  = ::SendMessage(hCurrentEditView, SCI_GETUSETABS, 0, 0);
    if (tabwidth <= 0) tabwidth = 4;

    bool isclosingtag;

    // use the selection
    long selstart = 0, selend = 0;
    // d�sactiv� : le fait de prettyprinter que la s�lection pose probl�me pour l'indentation
    // il faudrait calculer l'indentation de la 1re ligne de s�lection, mais l'indentation
    // de cette ligne n'est peut-�tre pas correcte. On pourrait la d�terminer en r�cup�rant
    // le path de la banche s�lectionn�e...
	  selstart = ::SendMessage(hCurrentEditView, SCI_GETSELECTIONSTART, 0, 0);
	  selend = ::SendMessage(hCurrentEditView, SCI_GETSELECTIONEND, 0, 0);

    std::string str("");
    std::string eolchar;
    int eolmode;
    Report::getEOLChar(hCurrentEditView, &eolmode, &eolchar);

	  if (selend > selstart) {
		  currentLength = selend-selstart;
		  data = new char[currentLength+1];
		  if (!data) return;  // allocation error, abort check
		  memset(data, '\0', currentLength+1);
		  ::SendMessage(hCurrentEditView, SCI_GETSELTEXT, 0, reinterpret_cast<LPARAM>(data));
	  } else {
		  currentLength = (int) ::SendMessage(hCurrentEditView, SCI_GETLENGTH, 0, 0);
		  data = new char[currentLength+1];
		  if (!data) return;  // allocation error, abort check
		  memset(data, '\0', currentLength+1);
		  ::SendMessage(hCurrentEditView, SCI_GETTEXT, currentLength+1, reinterpret_cast<LPARAM>(data));
	  }
	
    // Check de la syntaxe (disabled)
/*
    if (FALSE) {
      //updateProxyConfig();
      xmlDocPtr doc = xmlReadMemory(data, currentLength, "noname.xml", NULL, (doPreventXXE ? defFlagsNoXXE : defFlags));
      if (doc == NULL) {
        xmlErrorPtr err;
        err = xmlGetLastError();

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

      xmlFreeDoc(doc);
    }
*/

    str += data;
    delete[] data;
    data = NULL;

    // Proceed to first pass if break adds are enabled
    if (addlinebreaks) {
      while (curpos < str.length() && (curpos = str.find_first_of("<>",curpos)) != std::string::npos) {
        cc = str.at(curpos);

        if (cc == '<' && curpos < str.length()-3 && !str.compare(curpos,4,"<!--")) {
          // Let's skip the comment
          curpos = str.find("-->",curpos+1)+2;
          // Adds a line break after comment if required
          nexwchar_t = str.find_first_not_of(" \t",curpos+1);
          if (nexwchar_t != std::string::npos && str.at(nexwchar_t) == '<') {
            str.insert(++curpos,eolchar);
          }
        } else if (cc == '<' && curpos < str.length()-8 && !str.compare(curpos,9,"<![CDATA[")) {
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
          prevspecchar = str.find_last_of("<>",curpos-1);
          if (prevspecchar != std::string::npos && str.at(prevspecchar) == '<') {
            // We have found a '>' char and we are in a tag, let's see if it's an opening or closing one
            isclosingtag = (curpos > 0 && str.at(curpos-1) == '/');

            nexwchar_t = str.find_first_not_of(" \t",curpos+1);
            deltapos = nexwchar_t-curpos;
            if (nexwchar_t != std::string::npos &&
                str.at(nexwchar_t) == '<' &&
                curpos < str.length()-(deltapos+1)) {
              // we compare previous and next tags; if they are same, we don't add line break
              startprev = str.rfind("<",curpos);
              endnext = str.find(">",nexwchar_t);

              if (startprev != std::string::npos && endnext != std::string::npos && curpos > startprev && endnext > nexwchar_t) {
                tagend = str.find_first_of(" />",startprev+1);
                std::string tag1(str.substr(startprev+1,tagend-startprev-1));
                tagend = str.find_first_of(" />",nexwchar_t+2);
                std::string tag2(str.substr(nexwchar_t+2,tagend-nexwchar_t-2));
                if (strcmp(tag1.c_str(),tag2.c_str()) || isclosingtag) {
                  // Case of "<data><data>..." -> add a line break between tags
                  str.insert(++curpos,eolchar);
                } else if (str.at(nexwchar_t+1) == '/' && !isclosingtag && nexwchar_t == curpos+1) {
                  // Case of "<data id="..."></data>" -> replace by "<data id="..."/>"
                  str.replace(curpos,endnext-curpos,"/");
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
      if (!Report::isEOL(str, strlength, curpos, eolmode)) {
        if (curpos < strlength-3 && !str.compare(curpos,4,"<!--")) {
          in_comment = true;
        }
        if (curpos < strlength-8 && !str.compare(curpos,9,"<![CDATA[")) {
          in_cdata = true;
        } else if (curpos < strlength-1 && !str.compare(curpos,2,"<?")) {
          in_header = true;
        } else if (curpos < strlength &&
                   (!str.compare(curpos,1,"\"") || !str.compare(curpos,1,"'")) &&
                   str.at(curpos) == attributeQuote &&
                   prevspecchar != std::string::npos &&
                   str.at(prevspecchar) == '<') {
          in_attribute = !in_attribute;
          attributeQuote = str.at(curpos);  // store the attribute quote char to detect the end of attribute
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
          std::string::size_type tmppos = str.find_last_of("<>", curpos-1);
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
            int delta = 0;
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
              str.insert(curpos,(xmllevel-delta),'\t');
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

    // Restore scrolling
    ::SendMessage(hCurrentEditView, SCI_LINESCROLL, 0, yOffset);
    ::SendMessage(hCurrentEditView, SCI_SETXOFFSET, xOffset, 0);
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

void prettyPrintLibXML() {
  dbgln("prettyPrint()");

  int docIterator = initDocIterator();
  while (hasNextDoc(&docIterator)) {
    int currentEdit, currentLength, isReadOnly, xOffset, yOffset;
    ::SendMessage(nppData._nppHandle, NPPM_GETCURRENTSCINTILLA, 0, (LPARAM)&currentEdit);
    HWND hCurrentEditView = getCurrentHScintilla(currentEdit);

    isReadOnly = (int) ::SendMessage(hCurrentEditView, SCI_GETREADONLY, 0, 0);
    if (isReadOnly) return;
    
    xOffset = (int) ::SendMessage(hCurrentEditView, SCI_GETXOFFSET, 0, 0);
    yOffset = (int) ::SendMessage(hCurrentEditView, SCI_GETFIRSTVISIBLELINE, 0, 0);

    currentLength = (int) ::SendMessage(hCurrentEditView, SCI_GETLENGTH, 0, 0);

    char *data = new char[currentLength+1];
    if (!data) return;  // allocation error, abort check
    memset(data, '\0', currentLength+1);

    ::SendMessage(hCurrentEditView, SCI_GETTEXT, currentLength+1, reinterpret_cast<LPARAM>(data));

    int tabwidth = ::SendMessage(hCurrentEditView, SCI_GETTABWIDTH, 0, 0);
    int usetabs = ::SendMessage(hCurrentEditView, SCI_GETUSETABS, 0, 0);
    if (tabwidth <= 0) tabwidth = 4;

    xmlDocPtr doc;

//  updateProxyConfig();
    doc = xmlReadMemory(data, currentLength, "noname.xml", NULL, (doPreventXXE ? defFlagsNoXXE : defFlags));
    if (doc == NULL) {
      Report::_printf_err(L"Errors detected in content. Please correct them before applying pretty print.");
      delete [] data;
      return;
    }

    xmlChar *mem;
    int numbytes;
    xmlKeepBlanksDefault(0);
    xmlThrDefIndentTreeOutput(1);

    char *indentString;
    if (usetabs) {
      indentString = (char*) malloc(2);
      indentString[0] = '\t';
      indentString[1] = 0;
    } else {
      indentString = (char*) malloc(tabwidth + 1);
      for (int i=0; i<tabwidth; i++) indentString[i] = ' ';
      indentString[tabwidth] = 0;
    }
    if (indentString) xmlThrDefTreeIndentString(indentString);

/*
    ops->indent = 1;
    ops->indent_tab = 0;
    ops->indent_spaces = 2;
    ops->omit_decl = 0;
    ops->recovery = 0;
    ops->dropdtd = 0;
    ops->options = 0;
*/
    xmlDocDumpFormatMemory(doc,&mem,&numbytes,1);

    // Send formatted string to scintilla
    ::SendMessage(hCurrentEditView, SCI_SETTEXT, 0, reinterpret_cast<LPARAM>(mem));

    // Restore scrolling
    ::SendMessage(hCurrentEditView, SCI_LINESCROLL, 0, yOffset);
    ::SendMessage(hCurrentEditView, SCI_SETXOFFSET, xOffset, 0);

    xmlFree(mem);
    xmlFreeDoc(doc);
    if (indentString) free(indentString);

    delete [] data;
  }
}

void prettyPrintAttributes() {
  #ifdef __XMLTOOLS_DEBUG__
    Report::_printf_inf("prettyPrint()");
  #endif

  int docIterator = initDocIterator();
  while (hasNextDoc(&docIterator)) {
    int currentEdit, currentLength, isReadOnly, xOffset, yOffset;
    ::SendMessage(nppData._nppHandle, NPPM_GETCURRENTSCINTILLA, 0, (LPARAM)&currentEdit);
    HWND hCurrentEditView = getCurrentHScintilla(currentEdit);

    isReadOnly = (int) ::SendMessage(hCurrentEditView, SCI_GETREADONLY, 0, 0);
    if (isReadOnly) return;
    
    xOffset = (int) ::SendMessage(hCurrentEditView, SCI_GETXOFFSET, 0, 0);
    yOffset = (int) ::SendMessage(hCurrentEditView, SCI_GETFIRSTVISIBLELINE, 0, 0);

    currentLength = (int) ::SendMessage(hCurrentEditView, SCI_GETLENGTH, 0, 0);

    char *data = new char[currentLength+1];
    if (!data) return;  // allocation error, abort check
    memset(data, '\0', currentLength+1);

    ::SendMessage(hCurrentEditView, SCI_GETTEXT, currentLength+1, reinterpret_cast<LPARAM>(data));

    int tabwidth = ::SendMessage(hCurrentEditView, SCI_GETTABWIDTH, 0, 0);
    int usetabs = ::SendMessage(hCurrentEditView, SCI_GETUSETABS, 0, 0);
    if (tabwidth <= 0) tabwidth = 4;

    xmlDocPtr doc;

    doc = xmlReadMemory(data, currentLength, "noname.xml", NULL, 0);
    if (doc == NULL) {
      Report::_printf_err(L"Errors detected in content. Please correct them before applying pretty print.");
      delete [] data;
      return;
    }
    std::string str = data;

    bool in_comment = false, in_header = false, in_attribute = false, in_nodetext = false, in_cdata = false;
    long curpos = 0, strlength = 0;
    std::string lineindent = "";
    char pc, cc, nc, nnc;
    int tagsignlevel = 0, nattrs = 0;

    std::string eolchar;
    int eolmode;
    Report::getEOLChar(hCurrentEditView, &eolmode, &eolchar);

    int prevspecchar = -1;
    while (curpos < (long)str.length() && (curpos = str.find_first_of("<>\"",curpos)) >= 0) {
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
            long attrpos = str.find_last_of(eolchar+"\t ", curpos-1)+1;
            if (!Report::isEOL(str, strlength, attrpos, eolmode)) {
              long spacpos = str.find_last_not_of(eolchar+"\t ", attrpos-1)+1;
              str.replace(spacpos, attrpos-spacpos, lineindent);
              curpos -= attrpos-spacpos;
              curpos += lineindent.length();
            }
          } else {
            long attrpos = str.find_last_of(eolchar+"\t ", curpos-1)+1;
            if (!Report::isEOL(str, strlength, attrpos, eolmode)) {
              long linestart = str.find_last_of(eolchar, attrpos-1)+1;
              lineindent = str.substr(linestart, attrpos-linestart);
              long lineindentlen = lineindent.length();
              for (long i = 0; i < lineindentlen; ++i) {
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
        if (in_comment && curpos > 1 && !str.compare(curpos-2,3,"-->")) in_comment = false;
        else if (in_cdata && curpos > 1 && !str.compare(curpos-2,3,"]]>")) in_cdata = false;
        else if (in_header && curpos > 0 && !str.compare(curpos-1,2,"?>")) in_header = false;
        ++curpos;
      }
    }

    // Send formatted string to scintilla
    ::SendMessage(hCurrentEditView, SCI_SETTEXT, 0, reinterpret_cast<LPARAM>(str.c_str()));

    // Restore scrolling
    ::SendMessage(hCurrentEditView, SCI_LINESCROLL, 0, yOffset);
    ::SendMessage(hCurrentEditView, SCI_SETXOFFSET, xOffset, 0);

    delete [] data;
  }
}

///////////////////////////////////////////////////////////////////////////////

void linarizeXML() {
  dbgln("linarizeXML()");

  int docIterator = initDocIterator();
  while (hasNextDoc(&docIterator)) {
    int currentEdit, currentLength;
    ::SendMessage(nppData._nppHandle, NPPM_GETCURRENTSCINTILLA, 0, (LPARAM)&currentEdit);
    HWND hCurrentEditView = getCurrentHScintilla(currentEdit);
    currentLength = (int) ::SendMessage(hCurrentEditView, SCI_GETLENGTH, 0, 0);

    char *data = new char[currentLength+1];
    if (!data) return;  // allocation error, abort check
    memset(data, '\0', currentLength+1);

    int currentPos = int(::SendMessage(hCurrentEditView, SCI_GETCURRENTPOS, 0, 0));

    ::SendMessage(hCurrentEditView, SCI_GETTEXT, currentLength+1, reinterpret_cast<LPARAM>(data));

    std::string eolchar;
    int eolmode;
    Report::getEOLChar(hCurrentEditView, &eolmode, &eolchar);

    std::string str(data);
    delete [] data;
    data = NULL;

    std::string::size_type curpos = 0, nexwchar_t;
    bool enableInsert = false;

    while ((curpos = str.find_first_of(eolchar, curpos)) != std::string::npos) {
      nexwchar_t = str.find_first_not_of(eolchar, curpos);
      str.erase(curpos, nexwchar_t-curpos);

      // Let erase leading space chars on line
      if (curpos != std::string::npos && curpos < str.length()) {
        nexwchar_t = str.find_first_not_of(" \t", curpos);
        if (nexwchar_t >= curpos) {
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
    ::SendMessage(hCurrentEditView, SCI_SETTEXT, 0, reinterpret_cast<LPARAM>(str.c_str()));

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

  int currentEdit, isReadOnly, xOffset, yOffset;
  ::SendMessage(nppData._nppHandle, NPPM_GETCURRENTSCINTILLA, 0, (LPARAM)&currentEdit);
  HWND hCurrentEditView = getCurrentHScintilla(currentEdit);
  
  isReadOnly = (int) ::SendMessage(hCurrentEditView, SCI_GETREADONLY, 0, 0);
  if (isReadOnly) return;
  
  xOffset = (int) ::SendMessage(hCurrentEditView, SCI_GETXOFFSET, 0, 0);
  yOffset = (int) ::SendMessage(hCurrentEditView, SCI_GETFIRSTVISIBLELINE, 0, 0);

  long selstart = ::SendMessage(hCurrentEditView, SCI_GETSELECTIONSTART, 0, 0);
  long selend = ::SendMessage(hCurrentEditView, SCI_GETSELECTIONEND, 0, 0);
  long sellength = selend-selstart;

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

  // Restore scrolling
  //::SendMessage(hCurrentEditView, SCI_LINESCROLL, 0, yOffset);
  //::SendMessage(hCurrentEditView, SCI_SETXOFFSET, xOffset, 0);

  str.clear();
}

///////////////////////////////////////////////////////////////////////////////

void convertXML2Text() {
  dbgln("convertXML2Text()");

  int currentEdit, isReadOnly, xOffset, yOffset;
  ::SendMessage(nppData._nppHandle, NPPM_GETCURRENTSCINTILLA, 0, (LPARAM)&currentEdit);
  HWND hCurrentEditView = getCurrentHScintilla(currentEdit);

  isReadOnly = (int) ::SendMessage(hCurrentEditView, SCI_GETREADONLY, 0, 0);
  if (isReadOnly) return;
  
  xOffset = (int) ::SendMessage(hCurrentEditView, SCI_GETXOFFSET, 0, 0);
  yOffset = (int) ::SendMessage(hCurrentEditView, SCI_GETFIRSTVISIBLELINE, 0, 0);

  long selstart = ::SendMessage(hCurrentEditView, SCI_GETSELECTIONSTART, 0, 0);
  long selend = ::SendMessage(hCurrentEditView, SCI_GETSELECTIONEND, 0, 0);
  long sellength = selend-selstart;

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

  // Restore scrolling
  //::SendMessage(hCurrentEditView, SCI_LINESCROLL, 0, yOffset);
  //::SendMessage(hCurrentEditView, SCI_SETXOFFSET, xOffset, 0);

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
  if (!checkstack.empty()) errflag = checkstack.size();

  return errflag;
}

void commentSelection() {
  dbgln("commentSelection()");

  long currentEdit, xOffset, yOffset;
  int isReadOnly;
  ::SendMessage(nppData._nppHandle, NPPM_GETCURRENTSCINTILLA, 0, (LPARAM)&currentEdit);
  HWND hCurrentEditView = getCurrentHScintilla(currentEdit);
  
  isReadOnly = (int) ::SendMessage(hCurrentEditView, SCI_GETREADONLY, 0, 0);
  if (isReadOnly) return;
  
  xOffset = (long) ::SendMessage(hCurrentEditView, SCI_GETXOFFSET, 0, 0);
  yOffset = (long) ::SendMessage(hCurrentEditView, SCI_GETFIRSTVISIBLELINE, 0, 0);
  long selstart = ::SendMessage(hCurrentEditView, SCI_GETSELECTIONSTART, 0, 0);
  long selend = ::SendMessage(hCurrentEditView, SCI_GETSELECTIONEND, 0, 0);
  long sellength = selend-selstart;
  
  if (selend <= selstart) {
    Report::_printf_err(L"Please select text to transform before you call the function.");
    return;
  }

  char *data = new char[sellength+1];
  if (!data) return;  // allocation error, abort check
  memset(data, '\0', sellength+1);

  ::SendMessage(hCurrentEditView, SCI_GETSELTEXT, 0, reinterpret_cast<LPARAM>(data));

  while (sellength >= 0 && (data[sellength] == '\r' || data[sellength] == '\n')) {
    data[sellength--] = '\0';
  }

  std::string str(data);
  delete [] data;
  data = NULL;

  int errflag = validateSelectionForComment(str, sellength);
  if (errflag) {
    wchar_t msg[512];
    swprintf(msg, 512, L"The current selection covers part only one portion of another comment.\nUncomment process may be not applicable.\n\nDo you want to continue ?", errflag);
    if (::MessageBox(nppData._nppHandle, msg, L"XML Tools plugin", MB_YESNO | MB_ICONASTERISK) == IDNO) {
      str.clear();
      return;
    }
  }

  std::string::size_type curpos = sellength;
  while (curpos != std::string::npos && (curpos = str.rfind("<!{", curpos)) != std::string::npos) {
    if (curpos != std::string::npos) {
      int endvalpos = str.find("}**", curpos);
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
      int endvalpos = str.find("}>", curpos);
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

  // Restore scrolling
  //::SendMessage(hCurrentEditView, SCI_LINESCROLL, 0, yOffset);
  //::SendMessage(hCurrentEditView, SCI_SETXOFFSET, xOffset, 0);

  str.clear();
}

///////////////////////////////////////////////////////////////////////////////

void uncommentSelection() {
  dbgln("uncommentSelection()");

  long currentEdit, xOffset, yOffset;
  int isReadOnly;
  ::SendMessage(nppData._nppHandle, NPPM_GETCURRENTSCINTILLA, 0, (LPARAM)&currentEdit);
  HWND hCurrentEditView = getCurrentHScintilla(currentEdit);
  
  isReadOnly = (int) ::SendMessage(hCurrentEditView, SCI_GETREADONLY, 0, 0);
  if (isReadOnly) return;
  
  xOffset = (long) ::SendMessage(hCurrentEditView, SCI_GETXOFFSET, 0, 0);
  yOffset = (long) ::SendMessage(hCurrentEditView, SCI_GETFIRSTVISIBLELINE, 0, 0);

  long selstart = ::SendMessage(hCurrentEditView, SCI_GETSELECTIONSTART, 0, 0);
  long selend = ::SendMessage(hCurrentEditView, SCI_GETSELECTIONEND, 0, 0);
  long sellength = selend-selstart;

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

  int errflag = validateSelectionForComment(str, sellength);
  if (errflag) {
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
      int endvalpos = str.find("}**", curpos);
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
      int endvalpos = str.find("}>", curpos);
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

  // Restore scrolling
  //::SendMessage(hCurrentEditView, SCI_LINESCROLL, 0, yOffset);
  //::SendMessage(hCurrentEditView, SCI_SETXOFFSET, xOffset, 0);

  str.clear();
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CXMLToolsApp object

CXMLToolsApp* theApp = new CXMLToolsApp();
