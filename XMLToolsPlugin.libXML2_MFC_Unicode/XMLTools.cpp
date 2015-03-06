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
#include "LoadLibrary.h"

#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xmlschemas.h>
#include <libxml/globals.h>

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
const int TOTAL_FUNCS = 30;
int nbFunc = TOTAL_FUNCS;

NppData nppData;

unsigned long defFlags = XML_PARSE_NOENT | XML_PARSE_DTDLOAD;
unsigned long defFlagsNoXXE = 0;

// XML Loading status
int libloadstatus = -1;

// INI file support
wchar_t iniFilePath[MAX_PATH];
const wchar_t sectionName[] = L"XML Tools";

// Declaration of functionality (FuncItem) Array
FuncItem funcItem[TOTAL_FUNCS];
bool doCheckXML = false, doValidation = false, /*doPrettyPrint = false,*/ doCloseTag = false, doAutoIndent = false, doAttrAutoComplete = false, doAutoXMLType = false, doPreventXXE = true, doPrettyPrintAllOpenFiles = false;
int menuitemCheckXML = -1, menuitemValidation = -1, /*menuitemPrettyPrint = -1,*/ menuitemCloseTag = -1, menuitemAutoIndent = -1, menuitemAttrAutoComplete = -1, menuitemAutoXMLType = -1, menuitemPreventXXE = -1, menuitemPrettyPrintAllFiles = -1;
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

void aboutBox();
void howtoUse();

int performXMLCheck(int informIfNoError);
void savePluginParams();

///////////////////////////////////////////////////////////////////////////////

void registerShortcut(FuncItem *item, bool enableALT, bool enableCTRL, bool enableSHIFT, unsigned char key) {
  if (!item) return;
  item->_pShKey = new ShortcutKey();
  item->_pShKey->_isAlt = enableALT;
  item->_pShKey->_isCtrl = enableCTRL;
  item->_pShKey->_isShift = enableSHIFT;
  item->_pShKey->_key = key;
}

// get given lang as string (for debug purposes only)
char* getLangType(LangType lg) {
  if (lg == L_TEXT) return "L_TEXT";
  if (lg == L_PHP) return "L_PHP";
  if (lg == L_C) return "L_C";
  if (lg == L_CPP) return "L_CPP";
  if (lg == L_CS) return "L_CS";
  if (lg == L_OBJC) return "L_OBJC";
  if (lg == L_JAVA) return "L_JAVA";
  if (lg == L_RC) return "L_RC";
  if (lg == L_HTML) return "L_HTML";
  if (lg == L_XML) return "L_XML";
  if (lg == L_MAKEFILE) return "L_MAKEFILE";
  if (lg == L_PASCAL) return "L_PASCAL";
  if (lg == L_BATCH) return "L_BATCH";
  if (lg == L_INI) return "L_INI";
  if (lg == L_ASCII) return "L_ASCII";
  if (lg == L_USER) return "L_USER";
  if (lg == L_ASP) return "L_ASP";
  if (lg == L_SQL) return "L_SQL";
  if (lg == L_VB) return "L_VB";
  if (lg == L_JS) return "L_JS";
  if (lg == L_CSS) return "L_CSS";
  if (lg == L_PERL) return "L_PERL";
  if (lg == L_PYTHON) return "L_PYTHON";
  if (lg == L_LUA) return "L_LUA";
  if (lg == L_TEX) return "L_TEX";
  if (lg == L_FORTRAN) return "L_FORTRAN";
  if (lg == L_BASH) return "L_BASH";
  if (lg == L_FLASH) return "L_FLASH";
  if (lg == L_NSIS) return "L_NSIS";
  if (lg == L_TCL) return "L_TCL";
  if (lg == L_LISP) return "L_LISP";
  if (lg == L_SCHEME) return "L_SCHEME";
  if (lg == L_ASM) return "L_ASM";
  if (lg == L_DIFF) return "L_DIFF";
  if (lg == L_CAML) return "L_CAML";
  if (lg == L_ADA) return "L_ADA";
  if (lg == L_VERILOG) return "L_VERILOG";
  if (lg == L_MATLAB) return "L_MATLAB";
  if (lg == L_HASKELL) return "L_HASKELL";
  if (lg == L_INNO) return "L_INNO";
  if (lg == L_SEARCHRESULT) return "L_SEARCHRESULT";
  if (lg == L_CMAKE) return "L_CMAKE";
  if (lg == L_YAML) return "L_YAML";
  if (lg == L_EXTERNAL) return "L_EXTERNAL";
  
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
  wchar_t nppMainPath[MAX_PATH], appDataPath[MAX_PATH];
  GetModuleFileName(::GetModuleHandle(XMLTOOLS_DLLNAME), nppMainPath, sizeof(nppMainPath));
  
  // remove the module name : get plugins directory path
  PathRemoveFileSpec(nppMainPath);
  
  // cd .. : get npp executable path
  PathRemoveFileSpec(nppMainPath);
  
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
	Report::strcpy(iniFilePath, nppMainPath);
    PathAppend(iniFilePath, L"XMLToolsExt.ini");
  } else {
    Report::strcpy(iniFilePath, appDataPath);
    PathAppend(iniFilePath, L"Notepad++\\XMLToolsExt.ini");
  }

  // chargement de la librairie
  libloadstatus = loadLibXML(nppMainPath, appDataPath);
  if (libloadstatus < 0) nbFunc = 1;
  
  int menuentry = 0;
  for (int i = 0; i < nbFunc; ++i) {
    funcItem[i]._init2Check = false;
  }

  if (!libloadstatus) {
    Report::strcpy(funcItem[menuentry]._itemName, L"Enable XML syntax auto-check");
    funcItem[menuentry]._pFunc = insertXMLCheckTag;
    funcItem[menuentry]._init2Check = (::GetPrivateProfileInt(sectionName, L"doCheckXML", 0, iniFilePath) != 0);
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
    funcItem[menuentry]._init2Check = doCloseTag = (::GetPrivateProfileInt(sectionName, L"doCloseTag", 0, iniFilePath) != 0);
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
    ++menuentry;*/
  
    funcItem[menuentry++]._pFunc = NULL;  //----------------------------------------
  
    Report::strcpy(funcItem[menuentry]._itemName, L"Set XML type automatically");
    funcItem[menuentry]._pFunc = insertAutoXMLType;
    funcItem[menuentry]._init2Check = doAutoXMLType = (::GetPrivateProfileInt(sectionName, L"doAutoXMLType", 0, iniFilePath) != 0);
    doAutoXMLType = funcItem[menuentry]._init2Check;
    menuitemAutoXMLType = menuentry;
    ++menuentry;
  
    Report::strcpy(funcItem[menuentry]._itemName, L"Prevent XXE");
    funcItem[menuentry]._pFunc = togglePreventXXE;
    funcItem[menuentry]._init2Check = doPreventXXE = (::GetPrivateProfileInt(sectionName, L"doPreventXXE", 0, iniFilePath) != 0);
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
  
    /*Report::strcpy(funcItem[menuentry]._itemName, L"Enable auto pretty print (libXML) [experimental]");
    funcItem[menuentry]._pFunc = insertPrettyPrintTag;
    funcItem[menuentry]._init2Check = doPrettyPrint = (::GetPrivateProfileInt(sectionName, L"doPrettyPrint", 0, iniFilePath) != 0);
    doPrettyPrint = funcItem[menuentry]._init2Check;
    menuitemPrettyPrint = menuentry;
    ++menuentry;*/
  
    Report::strcpy(funcItem[menuentry]._itemName, L"Linarize XML");
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
  
    Report::strcpy(funcItem[menuentry]._itemName, L"About XML Tools");
    funcItem[menuentry]._pFunc = aboutBox;
    ++menuentry;
  } else {
    Report::strcpy(funcItem[menuentry]._itemName, L"How to install...");
    funcItem[menuentry]._pFunc = howtoUse;
    ++menuentry;
  }

  assert(menuentry == nbFunc);

  //Report::_printf_inf("menu entries: %d", menuentry);

  /*Report::_printf_inf("%s\ndoCheckXML: %d %d\ndoValidation: %d %d\ndoCloseTag: %d %d\ndoAutoXMLType: %d %d\ndoPreventXXE: %d %d\nisLocal: %d",
    iniFilePath,
    doCheckXML, funcItem[menuitemCheckXML]._init2Check,
    doValidation, funcItem[menuitemValidation]._init2Check,
    doCloseTag, funcItem[menuitemCloseTag]._init2Check,
    doAutoXMLType, funcItem[menuitemAutoXMLType]._init2Check,
    doPreventXXE, funcItem[menuitemPreventXXE]._init2Check,
    isLocal);*/
}

CXMLToolsApp::~CXMLToolsApp() {
  savePluginParams();

  // Don't forget to deallocate your shortcut here
  for (int i = 0; i < nbFunc; ++i) {
    if (funcItem[i]._pShKey) delete funcItem[i]._pShKey;
  }
}

void savePluginParams() {
  funcItem[menuitemCheckXML]._init2Check = doCheckXML;
  funcItem[menuitemValidation]._init2Check = doValidation;
  //funcItem[menuitemPrettyPrint]._init2Check = doPrettyPrint;
  funcItem[menuitemCloseTag]._init2Check = doCloseTag;
  //funcItem[menuitemAutoIndent]._init2Check = doAutoIndent;
  //funcItem[menuitemAttrAutoComplete]._init2Check = doAttrAutoComplete;
  funcItem[menuitemAutoXMLType]._init2Check = doAutoXMLType;
  funcItem[menuitemPreventXXE]._init2Check = doPreventXXE;
  funcItem[menuitemPrettyPrintAllFiles]._init2Check = doPrettyPrintAllOpenFiles;

  ::WritePrivateProfileString(sectionName, L"doCheckXML", doCheckXML?L"1":L"0", iniFilePath);
  ::WritePrivateProfileString(sectionName, L"doValidation", doValidation?L"1":L"0", iniFilePath);
  //::WritePrivateProfileString(sectionName, L"doPrettyPrint", doPrettyPrint?L"1":L"0", iniFilePath);
  ::WritePrivateProfileString(sectionName, L"doCloseTag", doCloseTag?L"1":L"0", iniFilePath);
  //::WritePrivateProfileString(sectionName, L"doAutoIndent", doAutoIndent?L"1":L"0", iniFilePath);
  //::WritePrivateProfileString(sectionName, L"doAttrAutoComplete", doAttrAutoComplete?L"1":L"0", iniFilePath);
  ::WritePrivateProfileString(sectionName, L"doAutoXMLType", doAutoXMLType?L"1":L"0", iniFilePath);
  ::WritePrivateProfileString(sectionName, L"doPreventXXE", doPreventXXE?L"1":L"0", iniFilePath);
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
      HMENU hMenu = ::GetMenu(nppData._nppHandle);
      if (hMenu) {
        ::CheckMenuItem(hMenu, funcItem[menuitemCheckXML]._cmdID, MF_BYCOMMAND | (doCheckXML?MF_CHECKED:MF_UNCHECKED));
        ::CheckMenuItem(hMenu, funcItem[menuitemValidation]._cmdID, MF_BYCOMMAND | (doValidation?MF_CHECKED:MF_UNCHECKED));
        //::CheckMenuItem(hMenu, funcItem[menuitemPrettyPrint]._cmdID, MF_BYCOMMAND | (doPrettyPrint?MF_CHECKED:MF_UNCHECKED));
        ::CheckMenuItem(hMenu, funcItem[menuitemCloseTag]._cmdID, MF_BYCOMMAND | (doCloseTag?MF_CHECKED:MF_UNCHECKED));
        //::CheckMenuItem(hMenu, funcItem[menuitemAutoIndent]._cmdID, MF_BYCOMMAND | (doAutoIndent?MF_CHECKED:MF_UNCHECKED));
        //::CheckMenuItem(hMenu, funcItem[menuitemAttrAutoComplete]._cmdID, MF_BYCOMMAND | (doAttrAutoComplete?MF_CHECKED:MF_UNCHECKED));
        ::CheckMenuItem(hMenu, funcItem[menuitemAutoXMLType]._cmdID, MF_BYCOMMAND | (doAutoXMLType?MF_CHECKED:MF_UNCHECKED));
        ::CheckMenuItem(hMenu, funcItem[menuitemPreventXXE]._cmdID, MF_BYCOMMAND | (doPreventXXE?MF_CHECKED:MF_UNCHECKED));
		    ::CheckMenuItem(hMenu, funcItem[menuitemPrettyPrintAllFiles]._cmdID, MF_BYCOMMAND | (doPrettyPrintAllOpenFiles?MF_CHECKED:MF_UNCHECKED));
      }
    }
    case NPPN_FILEBEFORESAVE: {
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
      LangType docType;
      ::SendMessage(nppData._nppHandle, NPPM_GETCURRENTLANGTYPE, 0, (LPARAM)&docType);
      if (docType == L_XML) {
        // remarque: le closeXMLTag doit s'exécuter avant les autres
        if (doCloseTag && notifyCode->ch == '>') closeXMLTag();
        //if (doAutoIndent && notifyCode->ch == '\n') tagAutoIndent();
        //if (doAttrAutoComplete && notifyCode->ch == '\"') attributeAutoComplete();
      }
      break;
    }
    case NPPN_BUFFERACTIVATED: {
      if (doAutoXMLType) {
        // si le fichier n'a pas de type défini et qu'il commence par "<?xml ", on lui attribue le type L_XML
        LangType docType = L_EXTERNAL;
        ::SendMessage(nppData._nppHandle, NPPM_GETCURRENTLANGTYPE, 0, (LPARAM)&docType);
        //Report::_printf_inf("%s", getLangType(docType));
        if (docType != L_XML) {
          setAutoXMLType();
        }
      }

	    /*if (doPrettyPrint) {
		  LangType docType = L_EXTERNAL;
        ::SendMessage(nppData._nppHandle, NPPM_GETCURRENTLANGTYPE, 0, (LPARAM)&docType);
        //Report::_printf_inf("%s", getLangType(docType));
        if (docType == L_XML) prettyPrintLibXML();
	    }*/
      break;
    }
  }
}

#ifdef UNICODE
	extern "C" __declspec(dllexport) BOOL isUnicode() {
		return TRUE;
	}
#endif //UNICODE

void insertXMLCheckTag() {
  #ifdef __XMLTOOLS_DEBUG__
    Report::_printf_inf("insertXMLCheckTag()");
  #endif
  doCheckXML = !doCheckXML;
  ::CheckMenuItem(::GetMenu(nppData._nppHandle), funcItem[menuitemCheckXML]._cmdID, MF_BYCOMMAND | (doCheckXML?MF_CHECKED:MF_UNCHECKED));
  savePluginParams();
}

void insertValidationTag() {
  #ifdef __XMLTOOLS_DEBUG__
    Report::_printf_inf("insertValidationTag()");
  #endif
  doValidation = !doValidation;
  ::CheckMenuItem(::GetMenu(nppData._nppHandle), funcItem[menuitemValidation]._cmdID, MF_BYCOMMAND | (doValidation?MF_CHECKED:MF_UNCHECKED));
  savePluginParams();
}
/*
void insertPrettyPrintTag() {
  #ifdef __XMLTOOLS_DEBUG__
    Report::_printf_inf("insertPrettyPrintTag()");
  #endif
  doPrettyPrint = !doPrettyPrint;
  ::CheckMenuItem(::GetMenu(nppData._nppHandle), funcItem[menuitemPrettyPrint]._cmdID, MF_BYCOMMAND | (doPrettyPrint?MF_CHECKED:MF_UNCHECKED));
  savePluginParams();
}*/

void insertXMLCloseTag() {
  #ifdef __XMLTOOLS_DEBUG__
    Report::_printf_inf("insertXMLCloseTag()");
  #endif
  doCloseTag = !doCloseTag;
  ::CheckMenuItem(::GetMenu(nppData._nppHandle), funcItem[menuitemCloseTag]._cmdID, MF_BYCOMMAND | (doCloseTag?MF_CHECKED:MF_UNCHECKED));
  savePluginParams();
}

bool tagAutoIndentWarningDisplayed = false;
void insertTagAutoIndent() {
  #ifdef __XMLTOOLS_DEBUG__
    Report::_printf_inf("insertTagAutoIndent()");
  #endif
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
  #ifdef __XMLTOOLS_DEBUG__
    Report::_printf_inf("insertAttributeAutoComplete()");
  #endif
  if (!insertAttributeAutoCompleteWarningDisplayed) {
    Report::_printf_inf(L"This function is in alpha state and might disappear in future release.");
    insertAttributeAutoCompleteWarningDisplayed = true;
  }
  
  doAttrAutoComplete = !doAttrAutoComplete;
  ::CheckMenuItem(::GetMenu(nppData._nppHandle), funcItem[menuitemAttrAutoComplete]._cmdID, MF_BYCOMMAND | (doAttrAutoComplete?MF_CHECKED:MF_UNCHECKED));
  savePluginParams();
}

void insertAutoXMLType() {
  #ifdef __XMLTOOLS_DEBUG__
    Report::_printf_inf("insertAutoXMLType()");
  #endif
  doAutoXMLType = !doAutoXMLType;
  ::CheckMenuItem(::GetMenu(nppData._nppHandle), funcItem[menuitemAutoXMLType]._cmdID, MF_BYCOMMAND | (doAutoXMLType?MF_CHECKED:MF_UNCHECKED));
  savePluginParams();
}

void togglePreventXXE() {
  #ifdef __XMLTOOLS_DEBUG__
    Report::_printf_inf("togglePreventXXE()");
  #endif
  doPreventXXE = !doPreventXXE;
  ::CheckMenuItem(::GetMenu(nppData._nppHandle), funcItem[menuitemPreventXXE]._cmdID, MF_BYCOMMAND | (doPreventXXE?MF_CHECKED:MF_UNCHECKED));
  savePluginParams();
}

void aboutBox() {
  #ifdef __XMLTOOLS_DEBUG__
    Report::_printf_inf("aboutBox()");
  #endif
  
  /*Report::_printf_inf(L"%s \r\n \r\n- libXML %s \r\n- libXSTL %s",
      TEXT(XMLTOOLS_ABOUTINFO), TEXT(LIBXML_DOTTED_VERSION), TEXT(LIBXSLT_DOTTED_VERSION));*/

  CAboutBoxDlg* dlg = new CAboutBoxDlg();
  dlg->DoModal();
}

void howtoUse() {
  #ifdef __XMLTOOLS_DEBUG__
    Report::_printf_inf("howtoUse()");
  #endif
  CHowtoUseDlg* dlg = new CHowtoUseDlg();
  dlg->DoModal();
}

///////////////////////////////////////////////////////////////////////////////

int performXMLCheck(int informIfNoError) {
  #ifdef __XMLTOOLS_DEBUG__
    Report::_printf_inf("performXMLCheck()");
  #endif
  int currentEdit, currentLength, res = 0;
  ::SendMessage(nppData._nppHandle, NPPM_GETCURRENTSCINTILLA, 0, (LPARAM)&currentEdit);
  HWND hCurrentEditView = getCurrentHScintilla(currentEdit);
  currentLength = (int) ::SendMessage(hCurrentEditView, SCI_GETLENGTH, 0, 0);

  char *data = new char[currentLength+1];
  if (!data) return -1;  // allocation error, abort check
  memset(data, '\0', currentLength+1);

  ::SendMessage(hCurrentEditView, SCI_GETTEXT, currentLength+1, reinterpret_cast<LPARAM>(data));
  
  pXmlResetLastError();
  xmlDocPtr doc = pXmlReadMemory(data, currentLength, "noname.xml", NULL, (doPreventXXE ? defFlagsNoXXE : defFlags));
  
  delete [] data;
  data = NULL;
  
  xmlErrorPtr err = pXmlGetLastError();

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
  
  pXmlFreeDoc(doc);

  if (errmsg.length() > 0) {
    Report::_printf_err(errmsg.c_str());
  } else if (nfomsg.length() > 0 && informIfNoError) {
    Report::_printf_inf(nfomsg.c_str());
  }

  return res;
}

void autoXMLCheck() {
  #ifdef __XMLTOOLS_DEBUG__
    Report::_printf_inf("autoXMLCheck()");
  #endif
  performXMLCheck(0);
}

void manualXMLCheck() {
  #ifdef __XMLTOOLS_DEBUG__
    Report::_printf_inf("manualXMLCheck()");
  #endif
  performXMLCheck(1);
}

///////////////////////////////////////////////////////////////////////////////

CSelectFileDlg* pSelectFileDlg = NULL;
void XMLValidation(int informIfNoError) {
  #ifdef __XMLTOOLS_DEBUG__
    Report::_printf_inf("XMLValidation()");
  #endif
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

  pXmlResetLastError();
  doc = pXmlReadMemory(data, currentLength, "noname.xml", NULL, (doPreventXXE ? defFlagsNoXXE : defFlags));

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

  // 2. Si le XMl est valide
  if (!abortValidation) {
    // 2.1. On essaie de retrouver le schéma ou la dtd das le document
    rootnode = pXmlDocGetRootElement(doc);
    if (rootnode == NULL) {
      Report::_printf_err(L"Empty XML document");
    } else {
      // 2.1.a. On recherche les attributs "xsi:noNamespaceSchemaLocation" ou "xsi:schemaLocation" dans la balise root
      // Exemple de balise root:
      //  <descript xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xsi:noNamespaceSchemaLocation="Descript_Shema.xsd">
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

    // Vérification des éléments: si on a à la fois une DTD et un schéma, on prend le schéma
    if (xsdValidation) dtdValidation = false;
  }
  
  if (!abortValidation) {
    // 2.2. Si l'attribut est absent, on demande à l'utilisateur de fournir le chemin du fichier XSD
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

    // 2.3.a. On procède à la validation par schéma
    if (xml_schema.length() > 0 && !dtdValidation) {
      // on concatène le chemin du document courant 
      wchar_t destpath[MAX_PATH] = { '\0' };
      ::SendMessage(nppData._nppHandle, NPPM_GETCURRENTDIRECTORY, MAX_PATH, (LPARAM)destpath);
      std::string xml_absolute_schema = Report::narrow(destpath);
      xml_absolute_schema.append("\\");
      xml_absolute_schema.append(xml_schema);

      // si le fichier existe, on le garde, sinon on reprend la valeur initiale (permet de gérer le cas où on reçoit une URL)
      if (PathFileExists(Report::widen(xml_absolute_schema).c_str()) == FALSE) {
        xml_absolute_schema = xml_schema;
      }

      if ((pctxt = pXmlSchemaNewParserCtxt(reinterpret_cast<const char*>(xml_absolute_schema.c_str()))) == NULL) {
        Report::_printf_err(L"Unable to initialize parser.");
      } else {
        // Chargement du contenu du XML Schema
        schema = pXmlSchemaParse(pctxt);

        if (schema == NULL) {
          xmlErrorPtr err;
          err = pXmlGetLastError();

          if (err != NULL) {
            if (err->line > 0) {
              ::SendMessage(hCurrentEditView, SCI_GOTOLINE, err->line-1, 0);
            }
			      wchar_t* tmpmsg = Report::castChar(err->message);  
            Report::_printf_err(L"Unable to parse schema file. \r\nParsing error at line %d: \r\n%s", err->line, tmpmsg);
			      delete[] tmpmsg;
          } else {
            Report::_printf_err(L"Unable to parse schema file.");
          }
        } else {
          // Création du contexte de validation
          if ((vctxt = pXmlSchemaNewValidCtxt(schema)) == NULL) {
            Report::_printf_err(L"Unable to create validation context.");
          } else {
            // Traitement des erreurs de validation
            Report::clearLog();
            Report::registerMessage("Validation of current file using XML schema:\r\n\r\n");
            pXmlSchemaSetValidErrors(vctxt, (xmlSchemaValidityErrorFunc) Report::registerError, (xmlSchemaValidityWarningFunc) Report::registerWarn, stderr);

            // Validation
            if (!pXmlSchemaValidateDoc(vctxt, doc)) {
              if (informIfNoError) Report::_printf_inf(L"XML Schema validation:\r\nXML is valid.");
            } else {
              CMessageDlg* msgdlg = new CMessageDlg();
              msgdlg->m_sMessage = Report::getLog().c_str();
              msgdlg->DoModal();
            }
          }

          // 2.4. On libère le parseur
          pXmlSchemaFree(schema);
          pXmlSchemaFreeValidCtxt(vctxt);
		    }

		    pXmlSchemaFreeParserCtxt(pctxt);
      }
    }

    // 2.3.b On procède à la validation par DTD
    if (dtdPtr && dtdValidation) {
      xmlValidCtxtPtr vctxt;

      // Création du contexte de validation
      if ((vctxt = pXmlNewValidCtxt())) {
        // Affichage des erreurs de validation
        Report::clearLog();
        Report::registerMessage("Validation of current file using DTD:\r\n\r\n");
        vctxt->userData = (void *) stderr;
        vctxt->error = (xmlValidityErrorFunc) Report::registerError;
        vctxt->warning = (xmlValidityWarningFunc) Report::registerWarn;

        // Validation
        if (pXmlValidateDtd(vctxt, doc, dtdPtr)) {
          if (informIfNoError) Report::_printf_inf(L"DTD validation:\r\nXML is valid.");
        } else {
          CMessageDlg* msgdlg = new CMessageDlg();
          msgdlg->m_sMessage = Report::getLog().c_str();
          msgdlg->DoModal();
        }
        // Libération de la mémoire
        pXmlFreeValidCtxt(vctxt);
      }
      if (doFreeDTDPtr) {
        pXmlFreeDtd(dtdPtr);
      }
    }
  }

  // 3. On libère la mémoire
  rootnode = NULL;
  pXmlFreeDoc(doc);

  // 4. On enregistre le nom du schéma utilisé pour le proposer la prochaine fois
  lastXMLSchema = xml_schema;
}

void autoValidation() {
  #ifdef __XMLTOOLS_DEBUG__
    Report::_printf_inf("autoValidation()");
  #endif
  XMLValidation(0);
}

void manualValidation() {
  #ifdef __XMLTOOLS_DEBUG__
    Report::_printf_inf("manualValidation()");
  #endif
  XMLValidation(1);
}

///////////////////////////////////////////////////////////////////////////////

void closeXMLTag() {
  #ifdef __XMLTOOLS_DEBUG__
    Report::_printf_inf("closeXMLTag()");
  #endif
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
        if (*pCur == '/') return;

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
  #ifdef __XMLTOOLS_DEBUG__
    Report::_printf_inf("tagAutoIndent()");
  #endif
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
  #ifdef __XMLTOOLS_DEBUG__
    Report::_printf_inf(L"attributeAutoComplete()");
  #endif
  Report::_printf_inf(L"attributeAutoComplete()");
}

///////////////////////////////////////////////////////////////////////////////

void setAutoXMLType() {
  #ifdef __XMLTOOLS_DEBUG__
    Report::_printf_inf("setAutoXMLType()");
  #endif

  int currentEdit;
  ::SendMessage(nppData._nppHandle, NPPM_GETCURRENTSCINTILLA, 0, (LPARAM)&currentEdit);
  HWND hCurrentEditView = getCurrentHScintilla(currentEdit);

  // on récupère les 6 premiers caractères du fichier
  char head[8] = { '\0' };
  ::SendMessage(hCurrentEditView, SCI_GETTEXT, 7, reinterpret_cast<LPARAM>(&head));
  ::SendMessage(hCurrentEditView, SCI_SETSEL, 0, 0);

  if (strlen(head) >= 6 && !strcmp(head, "<?xml ")) {
    LangType newType = L_XML;
    ::SendMessage(nppData._nppHandle, NPPM_SETCURRENTLANGTYPE, 0, (LPARAM) newType);
  }
}

///////////////////////////////////////////////////////////////////////////////

std::wstring currentXPath() {
  int currentEdit, currentLength;
  ::SendMessage(nppData._nppHandle, NPPM_GETCURRENTSCINTILLA, 0, (LPARAM)&currentEdit);
  HWND hCurrentEditView = getCurrentHScintilla(currentEdit);
  currentLength = (int) ::SendMessage(hCurrentEditView, SCI_GETLENGTH, 0, 0);
  
  std::wstring nodepath(L"");

  char *data = new char[currentLength+1];
  if (!data) return nodepath;  // allocation error, abort check
  size_t size = (currentLength+1)*sizeof(char);
  memset(data, '\0', size);

  std::string::size_type currentPos = long(::SendMessage(hCurrentEditView, SCI_GETCURRENTPOS, 0, 0));
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
    bool isinsideclosingtag = (currentPos > 0 && str.at(currentPos-1) == '<' && str.at(currentPos) == '/');

    if (isinsideclosingtag) {
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

    xmlDocPtr doc = pXmlReadMemory(str.c_str(), str.length(), "noname.xml", NULL, XML_PARSE_RECOVER | (doPreventXXE ? defFlagsNoXXE : defFlags));
    str.clear();
    
    if (doc == NULL) return nodepath;

    UniMode encoding = Report::getEncoding(doc->encoding, NULL);
    xmlNodePtr cur_node = pXmlDocGetRootElement(doc);

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
    pXmlFreeDoc(doc);
  }

  return nodepath;
}

void getCurrentXPath() {
  #ifdef __XMLTOOLS_DEBUG__
    Report::_printf_inf("getCurrentXPath()");
  #endif
  
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
  #ifdef __XMLTOOLS_DEBUG__
    Report::_printf_inf("evaluateXPath()");
  #endif
  if (pXPathEvalDlg == NULL) {
    pXPathEvalDlg = new CXPathEvalDlg(NULL, (doPreventXXE ? defFlagsNoXXE : defFlags));
    pXPathEvalDlg->Create(CXPathEvalDlg::IDD,NULL);
  }
  pXPathEvalDlg->ShowWindow(SW_SHOW);
}

#else
void evaluateXPath() {
  #ifdef __XMLTOOLS_DEBUG__
    Report::_printf_inf("evaluateXPath()");
  #endif
  Report::_printf_err("Function not available.");
}

#endif

///////////////////////////////////////////////////////////////////////////////

CXSLTransformDlg *pXSLTransformDlg = NULL;
void performXSLTransform() {
  #ifdef __XMLTOOLS_DEBUG__
    Report::_printf_inf("performXSLTransform()");
  #endif
  if (pXSLTransformDlg == NULL) {
    pXSLTransformDlg = new CXSLTransformDlg(NULL, (doPreventXXE ? defFlagsNoXXE : defFlags));
    pXSLTransformDlg->Create(CXSLTransformDlg::IDD,NULL);
  }
  pXSLTransformDlg->ShowWindow(SW_SHOW);
}

///////////////////////////////////////////////////////////////////////////////

void prettyPrint(bool autoindenttext, bool addlinebreaks) {
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
    // désactivé : le fait de prettyprinter que la sélection pose problème pour l'indentation
    // il faudrait calculer l'indentation de la 1re ligne de sélection, mais l'indentation
    // de cette ligne n'est peut-être pas correcte. On pourrait la déterminer en récupérant
    // le path de la banche sélectionnée...
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
    /*if (FALSE) {
      xmlDocPtr doc = pXmlReadMemory(data, currentLength, "noname.xml", NULL, (doPreventXXE ? defFlagsNoXXE : defFlags));
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
    }*/

    str += data;
    delete[] data;
    data = NULL;

    // Proceed to first pass if break adds are enabled
    if (addlinebreaks) {
      while (curpos < str.length() && (curpos = str.find_first_of("<>",curpos)) != std::string::npos) {
        cc = str.at(curpos);

        if (cc == '<' && curpos < str.length()-3 && !str.compare(curpos,4,"<!--")) {
          // Let's skip the comment
          curpos = str.find("-->",curpos+1);
        } else if (cc == '<' && curpos < str.length()-8 && !str.compare(curpos,9,"<![CDATA[")) {
          // Let's skip the CDATA block
          curpos = str.find("]]>",curpos+1);
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
                  //str.insert(++curpos,"#");
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
      }*/

      // reinitialize cursor pos for second pass
      curpos = 0;
    }

    // Proceed to reformatting (second pass)
    prevspecchar = std::string::npos;
    std::string sep("<>\"'");
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
        } else if (curpos < strlength && (!str.compare(curpos,1,"\"") || !str.compare(curpos,1,"'")) &&
                   prevspecchar != std::string::npos && str.at(prevspecchar) == '<') {
          in_attribute = !in_attribute;
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

    // Send formated string to scintilla
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
  #ifdef __XMLTOOLS_DEBUG__
    Report::_printf_inf("prettyPrintXML()");
  #endif
  prettyPrint(false, false);
}

void prettyPrintXMLBreaks() {
  #ifdef __XMLTOOLS_DEBUG__
    Report::_printf_inf("prettyPrintXMLBreaks()");
  #endif
  prettyPrint(false, true);
}

void prettyPrintText() {
  #ifdef __XMLTOOLS_DEBUG__
    Report::_printf_inf("prettyPrintText()");
  #endif
  prettyPrint(true, false);
}

void prettyPrintLibXML() {
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

    doc = pXmlReadMemory(data, currentLength, "noname.xml", NULL, (doPreventXXE ? defFlagsNoXXE : defFlags));
    if (doc == NULL) {
      Report::_printf_err(L"Errors detected in content. Please correct them before applying pretty print.");
      delete [] data;
      return;
    }

    xmlChar *mem;
    int numbytes;
    pXmlKeepBlanksDefault(0);
    pXmlThrDefIndentTreeOutput(1);

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
    if (indentString) pXmlThrDefTreeIndentString(indentString);

    /*
      ops->indent = 1;
      ops->indent_tab = 0;
      ops->indent_spaces = 2;
      ops->omit_decl = 0;
      ops->recovery = 0;
      ops->dropdtd = 0;
      ops->options = 0;
    */
    pXmlDocDumpFormatMemory(doc,&mem,&numbytes,1);

    // Send formated string to scintilla
    ::SendMessage(hCurrentEditView, SCI_SETTEXT, 0, reinterpret_cast<LPARAM>(mem));

    // Restore scrolling
    ::SendMessage(hCurrentEditView, SCI_LINESCROLL, 0, yOffset);
    ::SendMessage(hCurrentEditView, SCI_SETXOFFSET, xOffset, 0);

    pXmlFree(mem);
    pXmlFreeDoc(doc);
    if (indentString) free(indentString);

    delete [] data;
  }
}

///////////////////////////////////////////////////////////////////////////////

void linarizeXML() {
  #ifdef __XMLTOOLS_DEBUG__
    Report::_printf_inf("linarizeXML()");
  #endif

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

    // Send formated string to scintilla
    ::SendMessage(hCurrentEditView, SCI_SETTEXT, 0, reinterpret_cast<LPARAM>(str.c_str()));

    str.clear();
  }
}

void togglePrettyPrintAllFiles() {
  #ifdef __XMLTOOLS_DEBUG__
	  Report::_printf_inf("togglePrettyPrintAllFiles()");
  #endif
	doPrettyPrintAllOpenFiles = !doPrettyPrintAllOpenFiles;
	::CheckMenuItem(::GetMenu(nppData._nppHandle), funcItem[menuitemPrettyPrintAllFiles]._cmdID, MF_BYCOMMAND | (doPrettyPrintAllOpenFiles?MF_CHECKED:MF_UNCHECKED));
	savePluginParams();
}

int initDocIterator() {
  nbopenfiles1 = (int) ::SendMessage(nppData._nppHandle, NPPM_GETNBOPENFILES, 0, PRIMARY_VIEW);
  nbopenfiles2 = (int) ::SendMessage(nppData._nppHandle, NPPM_GETNBOPENFILES, 0, SECOND_VIEW);

  if (::SendMessage(nppData._nppHandle, NPPM_GETCURRENTDOCINDEX, 0, MAIN_VIEW) < 0) nbopenfiles1 = 0;
  if (::SendMessage(nppData._nppHandle, NPPM_GETCURRENTDOCINDEX, 0, SUB_VIEW) < 0) nbopenfiles2 = 0;

  //Report::_printf_inf(Report::str_format("%d %d",nbopenfiles1,nbopenfiles2));

  return 0;
}

bool hasNextDoc(int* iter) {
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
  #ifdef __XMLTOOLS_DEBUG__
    Report::_printf_inf("convertText2XML()");
  #endif
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
  #ifdef __XMLTOOLS_DEBUG__
    Report::_printf_inf("convertXML2Text()");
  #endif
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
  #ifdef __XMLTOOLS_DEBUG__
    Report::_printf_inf("validateSelectionForComment()");
  #endif
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
  #ifdef __XMLTOOLS_DEBUG__
    Report::_printf_inf("commentSelection()");
  #endif
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
  #ifdef __XMLTOOLS_DEBUG__
    Report::_printf_inf("uncommentSelection()");
  #endif
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
