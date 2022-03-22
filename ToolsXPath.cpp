#include "StdAfx.h"
#include "XMLTools.h"
#include "Scintilla.h"
#include "nppHelpers.h"
#include "Report.h"
#include "XmlFormater.h"

#include "XPathEvalDlg.h"

using namespace QuickXml;

std::wstring currentXPath(int xpathMode) {
    dbgln("currentXPath()");

    XmlFormater* formater = NULL;
    int currentEdit;
    std::wstring nodepath(L"");

    ::SendMessage(nppData._nppHandle, NPPM_GETCURRENTSCINTILLA, 0, (LPARAM)&currentEdit);
    HWND hCurrentEditView = getCurrentHScintilla(currentEdit);
    size_t currentLength = (size_t) ::SendMessage(hCurrentEditView, SCI_GETLENGTH, 0, 0);

    char* data = new char[currentLength + sizeof(char)];
    if (!data) return nodepath;  // allocation error, abort check
    size_t size = (currentLength + sizeof(char)) * sizeof(char);
    memset(data, '\0', size);

    size_t currentPos = size_t(::SendMessage(hCurrentEditView, SCI_GETCURRENTPOS, 0, 0));
    ::SendMessage(hCurrentEditView, SCI_GETTEXT, currentLength + sizeof(char), reinterpret_cast<LPARAM>(data));

    XmlFormaterParamsType params = XmlFormater::getDefaultParams();
    if ((xpathMode & XPATH_MODE_KEEPIDATTRIBUTE) != 0 && xmltoolsoptions.identityAttributes.length() > 0) {
        params.dumpIdAttributesName = xmltoolsoptions.dumpAttributeName;

        std::wstring temp;
        std::wstringstream wss(xmltoolsoptions.identityAttributes);
        while (std::getline(wss, temp, L';')) {
            params.identityAttribues.push_back(Report::ws2s(temp));
        }
    }
    formater = new XmlFormater(data, currentLength, params);
    nodepath = Report::utf8ToUcs2(formater->currentPath(currentPos, xpathMode)->str());
    delete[] data;
    delete formater;

    return nodepath;
}

void printCurrentXPathInStatusbar() {
    dbgln("printCurrentXPathInStatusbar()");

    int currentEdit;
    ::SendMessage(nppData._nppHandle, NPPM_GETCURRENTSCINTILLA, 0, (LPARAM)&currentEdit);
    HWND hCurrentEditView = getCurrentHScintilla(currentEdit);
    
    int xpathmode = XPATH_MODE_WITHNAMESPACE | XPATH_MODE_KEEPIDATTRIBUTE;
    if (xmltoolsoptions.printXPathIndex) {
        xpathmode |= XPATH_MODE_WITHNODEINDEX;
    }
   
    std::wstring tmp = currentXPath(xpathmode);
    if (tmp.length() == 0) tmp = L" ";  // empty value has no effect on NPPM_SETSTATUSBAR
    ::SendMessage(nppData._nppHandle, NPPM_SETSTATUSBAR, STATUSBAR_DOC_TYPE, (LPARAM) tmp.c_str());
}

void getCurrentXPath(bool precise) {
    dbgln("getCurrentXPath()");

    std::wstring nodepath(currentXPath(precise ? XPATH_MODE_WITHNAMESPACE : XPATH_MODE_BASIC));
    std::wstring tmpmsg(L"Current node cannot be resolved.");

    if (nodepath.length() > 0) {
        tmpmsg = nodepath;
        tmpmsg += L"\n\n(Path has been copied into clipboard)";

        ::OpenClipboard(NULL);
        ::EmptyClipboard();
        size_t size = (nodepath.length() + 1) * sizeof(wchar_t);
        HGLOBAL hClipboardData = GlobalAlloc(NULL, size);
        if (hClipboardData != NULL) {
            wchar_t* pchData = (wchar_t*)GlobalLock(hClipboardData);
            if (pchData != NULL) {
                memcpy(pchData, (wchar_t*)nodepath.c_str(), size);
                ::GlobalUnlock(hClipboardData);
            }
            ::SetClipboardData(CF_UNICODETEXT, hClipboardData);
        }
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

CXPathEvalDlg* pXPathEvalDlg = NULL;

void evaluateXPath() {
    dbgln("evaluateXPath()");

#if true
    if (pXPathEvalDlg == NULL) {
        pXPathEvalDlg = new CXPathEvalDlg(NULL, NULL);
    }
    pXPathEvalDlg->DoModal();
#else
    if (pXPathEvalDlg == NULL) {
        pXPathEvalDlg = new CXPathEvalDlg(NULL, NULL);
        pXPathEvalDlg->Create(CXPathEvalDlg::IDD, NULL);
    }
    pXPathEvalDlg->ShowWindow(SW_SHOW);
#endif
}
