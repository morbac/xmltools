// XMLTools.cpp : Defines the initialization routines for the DLL.
//

// notepad++
#include "StdAfx.h"
#include "XMLTools.h"
#include "PluginInterface.h"
#include "nppHelpers.h"

// dialogs
#include "MessageDlg.h"
#include "HowtoUseDlg.h"
#include "OptionsDlg.h"

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

NppData nppData;
HHOOK hook = NULL;

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

void deleteAnnotations();

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
  wchar_t pluginConfigPath[MAX_PATH] = { 0 };
  ::SendMessage(nppData._nppHandle, NPPM_GETPLUGINSCONFIGDIR, MAX_PATH, (LPARAM)pluginConfigPath);
  dbgln(pluginConfigPath);

  dbgln ("Reading configuration... ", DBG_LEVEL::DBG_INFO);
  config.Read(pluginConfigPath);

  initMenu();

  auto result = CoInitialize(NULL);
  if (result != S_OK && result != S_FALSE && result != RPC_E_CHANGED_MODE) {
      dbgln("CoInitialize failed", DBG_LEVEL::DBG_ERROR);
  }

  updateProxyConfig();

  dbgln("Initialization finished.", DBG_LEVEL::DBG_INFO);
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

//extern int nbFunc;
//extern FuncItem funcItem[];
extern std::vector<FuncItem> nppMenu;

// The getFuncsArray function gives Notepad++ plugins system the pointer FuncItem Array
// and the size of this array (the number of functions)
extern "C" __declspec(dllexport) FuncItem * getFuncsArray(int *nbF) {
    
    *nbF = static_cast<int>(nppMenu.size());
    return &nppMenu[0];
}

// For v.3.3 compatibility
extern "C" __declspec(dllexport) LRESULT messageProc(UINT Message, WPARAM wParam, LPARAM lParam) {
  return TRUE;
}

// If you don't need get the notification from Notepad++, just let it be empty.
extern "C" __declspec(dllexport) void beNotified(SCNotification *notifyCode) {
  switch (notifyCode->nmhdr.code) {
    case NPPN_READY: {
      dbgln("NPP Event: NPPN_READY");
      HMENU hMenu = ::GetMenu(nppData._nppHandle);
      if (hMenu) {
        if (hook) {
          UnhookWindowsHookEx(hook);
          hook = NULL;
        }
        else {
          hook = SetWindowsHookEx(WH_KEYBOARD, KeyboardProc, (HINSTANCE)GetCurrentModule(), ::GetCurrentThreadId());
        }

        createDebugDlg();
      }
    }
    case NPPN_FILEBEFORESAVE: {
      dbgln("NPP Event: NPPN_FILEBEFORESAVE");
      LangType docType;
      ::SendMessage(nppData._nppHandle, NPPM_GETCURRENTLANGTYPE, 0, (LPARAM)&docType);
      if (docType == LangType::L_XML) {
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
      if (config.doAutoXMLType && docType != LangType::L_XML) {
        if (setAutoXMLType()) {
          docType = LangType::L_XML;
        }
      }
      if (docType == LangType::L_XML) {
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
      TCHAR filename[MAX_PATH] = { 0 };
      ::SendMessage(nppData._nppHandle, NPPM_GETFULLPATHFROMBUFFERID, notifyCode->nmhdr.idFrom, reinterpret_cast<LPARAM>(filename));
      dbg("  bufferID: "); dbgln(std::to_string(static_cast<unsigned long long>(notifyCode->nmhdr.idFrom)).c_str());
      dbg("  filename: "); dbgln(filename);
      break;
    }
    case NPPN_BUFFERACTIVATED: {
      dbgln("NPP Event: NPPN_BUFFERACTIVATED");
      if (config.doAutoXMLType) {
        // si le fichier n'a pas de type défini et qu'il commence par "<?xml ", on lui attribue le type L_XML
        LangType docType = LangType::L_EXTERNAL;
        ::SendMessage(nppData._nppHandle, NPPM_GETCURRENTLANGTYPE, 0, (LPARAM)&docType);
        dbg("  Current langtype: "); dbgln(std::to_string(static_cast<unsigned long long>(docType)).c_str());
        //Report::_printf_inf("%s", getLangType(docType));
        if (docType != LangType::L_XML) {
          setAutoXMLType();
        }
      }
      break;
    }
    case NPPN_SHUTDOWN: {
      CoUninitialize();

      deleteAnnotations();

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
    proxyurl += std::to_wstring(proxyoptions.port);// std::to_string(static_cast<long long>(proxyoptions.port));

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

// The one and only CXMLToolsApp object

CXMLToolsApp* theApp = new CXMLToolsApp();
