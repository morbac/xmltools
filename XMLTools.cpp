// XMLTools.cpp : Defines the initialization routines for the DLL.
//

// notepad++
#include "StdAfx.h"
#include "XMLTools.h"
#include "PluginInterface.h"

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
#include <shlwapi.h>
#include <shlobj.h>
#include <stdlib.h>
#include <direct.h>
#include <assert.h>
#include <locale>
#include <algorithm>
#include <array>
#include <map>

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
//const wchar_t localConfFile[] = L"doLocalConf.xml";

NppData nppData;
CDebugDlg* debugdlg = new CDebugDlg();
HHOOK hook = NULL;

// Declaration of functionality (FuncItem) Array
std::wstring lastXMLSchema(L"");

std::map<LRESULT,bool> hasAnnotations;

// Here're the declaration my functions ///////////////////////////////////////
void initMenu();
void destroyMenu();

bool hasCurrentDocAnnotations();
void autoXMLCheck();
void autoValidation();
void closeXMLTag();
bool setAutoXMLType();
void howtoUse();
void updateProxyConfig();

int performXMLCheck(int informIfNoError);
void initializePlugin();
void savePluginParams();

///////////////////////////////////////////////////////////////////////////////
/*
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
*/
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
    destroyMenu();
}

void initializePlugin() {
  dbgln("initializePlugin()");
  /*
  dbg("Get plugin home dir... ");
  ::SendMessage(nppData._nppHandle, NPPM_GETPLUGINHOMEPATH, MAX_PATH, (LPARAM)pluginHomePath);
  PathAppend(pluginHomePath, L"\\XMLTools");
  dbgln(pluginHomePath);
  */
  dbg("Get plugin config dir... ");
  wchar_t pluginConfigPath[MAX_PATH];
  ::SendMessage(nppData._nppHandle, NPPM_GETPLUGINSCONFIGDIR, MAX_PATH, (LPARAM)pluginConfigPath);
  dbgln(pluginConfigPath);

  dbgln ("Reading configuration... ");
  config.Read(pluginConfigPath);

  initMenu();

  CoInitialize(NULL);

  dbgln("done.");

  updateProxyConfig();

  dbgln("Initialization finished.");
  dbgln("");
}

void savePluginParams() {
  dbgln("savePluginParams()");
  config.Write();
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

extern int nbFunc;
extern FuncItem funcItem[];

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
          /*
        ::CheckMenuItem(hMenu, funcItem[menuitemCheckXML]._cmdID, MF_BYCOMMAND | (config.doCheckXML?MF_CHECKED:MF_UNCHECKED));
        ::CheckMenuItem(hMenu, funcItem[menuitemValidation]._cmdID, MF_BYCOMMAND | (config.doValidation?MF_CHECKED:MF_UNCHECKED));
//      ::CheckMenuItem(hMenu, funcItem[menuitemPrettyPrint]._cmdID, MF_BYCOMMAND | (doPrettyPrint?MF_CHECKED:MF_UNCHECKED));
        ::CheckMenuItem(hMenu, funcItem[menuitemCloseTag]._cmdID, MF_BYCOMMAND | (config.doCloseTag?MF_CHECKED:MF_UNCHECKED));
//      ::CheckMenuItem(hMenu, funcItem[menuitemAutoIndent]._cmdID, MF_BYCOMMAND | (doAutoIndent?MF_CHECKED:MF_UNCHECKED));
//      ::CheckMenuItem(hMenu, funcItem[menuitemAttrAutoComplete]._cmdID, MF_BYCOMMAND | (doAttrAutoComplete?MF_CHECKED:MF_UNCHECKED));
        ::CheckMenuItem(hMenu, funcItem[menuitemAutoXMLType]._cmdID, MF_BYCOMMAND | (config.doAutoXMLType?MF_CHECKED:MF_UNCHECKED));
        ::CheckMenuItem(hMenu, funcItem[menuitemPreventXXE]._cmdID, MF_BYCOMMAND | (config.doPreventXXE?MF_CHECKED:MF_UNCHECKED));
        ::CheckMenuItem(hMenu, funcItem[menuitemAllowHuge]._cmdID, MF_BYCOMMAND | (config.doAllowHuge ? MF_CHECKED : MF_UNCHECKED));
        ::CheckMenuItem(hMenu, funcItem[menuitemPrettyPrintAllFiles]._cmdID, MF_BYCOMMAND | (config.doPrettyPrintAllOpenFiles?MF_CHECKED:MF_UNCHECKED));
        */
        if (hook) {
          UnhookWindowsHookEx(hook);
          hook = NULL;
        }
        else {
          hook = SetWindowsHookEx(WH_KEYBOARD, KeyboardProc, (HINSTANCE)GetCurrentModule(), ::GetCurrentThreadId());
        }

        //#ifdef DEBUG
          debugdlg->Create(CDebugDlg::IDD,NULL);
        //#endif
      }
    }
    case NPPN_FILEBEFORESAVE: {
      dbgln("NPP Event: NPPN_FILEBEFORESAVE");
      LangType docType;
      ::SendMessage(nppData._nppHandle, NPPM_GETCURRENTLANGTYPE, 0, (LPARAM)&docType);
      if (docType == L_XML) {
        // comme la validation XSD effectue également un check de syntaxe, on n'exécute
        // le autoXMLCheck() que si doValidation est FALSE et doCheckXML est TRUE.
        if (config.doValidation) autoValidation();
        else if (config.doCheckXML) autoXMLCheck();
      }
      break;
    }
    case SCN_CHARADDED: {
      dbgln("NPP Event: SCN_CHARADDED");
      LangType docType;
      ::SendMessage(nppData._nppHandle, NPPM_GETCURRENTLANGTYPE, 0, (LPARAM)&docType);
      if (config.doAutoXMLType && docType != L_XML) {
        if (setAutoXMLType()) {
          docType = L_XML;
        }
      }
      if (docType == L_XML) {
        // remarque: le closeXMLTag doit s'exécuter avant les autres
        if (config.doCloseTag && notifyCode->ch == '>') {
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
      if (config.doAutoXMLType) {
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

void optionsDlg() {
  dbgln("optionsDlg()");

  COptionsDlg* dlg = new COptionsDlg(NULL);
  if (dlg->DoModal() == IDOK) {
    savePluginParams();

    updateProxyConfig();
  }
}

void debugDlg() {
  debugdlg->ShowWindow(SW_SHOW);
}

void dbg(CStringW line) {
  //#ifdef DEBUG
    debugdlg->addLine(line);
  //#endif
}

void dbgln(CStringW line) {
  //#ifdef DEBUG
    debugdlg->addLine(line+"\r\n");
  //#endif
}

void updateProxyConfig() {
  dbgln("updateProxyConfig()");

  // proxy settings for libxml
  if (proxyoptions.status && proxyoptions.host.length() > 0) {
    std::wstring proxyurl(L"http://");
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

    proxyurl += proxyoptions.host;
    proxyurl += L":";
    proxyurl += proxyoptions.port;// std::to_string(static_cast<long long>(proxyoptions.port));

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
  std::wstring wmsg = L"";

  if (pXMLErr != NULL) {
    CHK_HR(pXMLErr->get_line(&line));
    CHK_HR(pXMLErr->get_linepos(&linepos));
    CHK_HR(pXMLErr->get_filepos(&filepos));
    CHK_HR(pXMLErr->get_reason(&bstrReason));

    if (szDesc != NULL) {
      if (line > 0 && linepos > 0) {
        wmsg = Report::str_format(L"%s - line %d, pos %d: \r\n%s", szDesc, line, linepos, bstrReason);
      }
      else {
        wmsg = Report::str_format(L"%s: \r\n%s", szDesc, bstrReason);
        line = 1;
        linepos = 1;
      }
    }
    else {
      if (line > 0 && linepos > 0) {
        wmsg = Report::str_format(L"XML Parsing error - line %d, pos %d: \r\n%s", line, linepos, bstrReason);
      }
      else {
        wmsg = Report::str_format(L"XML Parsing error: \r\n%s", bstrReason);
        line = 1;
        linepos = 1;
      }
    }

    displayXMLError(wmsg, view, line, linepos, filepos + 1);
  }

CleanUp:
  SysFreeString(bstrReason);
}

void displayXMLErrors(IXMLDOMParseError* pXMLErr, HWND view, const wchar_t* szDesc) {
  HRESULT hr = S_OK; 
  IXMLDOMParseError2* pTmpErr = NULL;
  IXMLDOMParseErrorCollection* pAllErrors = NULL;
  long length = 0;

  try {
    CHK_HR(((IXMLDOMParseError2*) pXMLErr)->get_allErrors(&pAllErrors));
    if (pAllErrors != NULL) {
      CHK_HR(pAllErrors->get_length(&length));
      for (long i = 0; i < length; ++i) {
        CHK_HR(pAllErrors->get_next(&pTmpErr));
        CHK_HR(pAllErrors->get_item(i, &pTmpErr));
        displayXMLError(pTmpErr, view, szDesc);
        SAFE_RELEASE(pTmpErr);
      }
    }
    else {
      displayXMLError(pXMLErr, view, szDesc);
    }
  }
  catch (...) {
    displayXMLError(pXMLErr, view, szDesc);
  }

CleanUp:
  SAFE_RELEASE(pTmpErr);
  SAFE_RELEASE(pAllErrors);
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
    displayXMLErrors(pXMLErr, hCurrentEditView, L"XML Parsing error");
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
  IXMLDOMParseError2* pXMLErr2 = NULL;
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
      if (pXMLDom->validate((IXMLDOMParseError**)&pXMLErr2) == S_FALSE) {
        displayXMLErrors(pXMLErr2, hCurrentEditView, L"XML Validation error");
      }
      else {
        Report::_printf_inf(L"No error detected.");
      }
    }
    else {
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
          }
          else {
            CHK_HR(pXD->get_parseError(&pXMLErr));
            displayXMLErrors(pXMLErr, hCurrentEditView, L"XML Validation error");
          }
        }
        else {
          Report::_printf_err(L"Invalid schema or missing namespace.");
        }
      }
      else {
        goto CleanUp;
      }
    }
  } else {
    CHK_HR(pXMLDom->get_parseError(&pXMLErr));
    displayXMLErrors(pXMLErr, hCurrentEditView, L"XML Validation error");
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


// The one and only CXMLToolsApp object

CXMLToolsApp* theApp = new CXMLToolsApp();
