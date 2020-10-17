#include "StdAfx.h"
#include "XMLTools.h"
#include "Scintilla.h"
#include "nppHelpers.h"
#include "Report.h"
#include "XmlFormater.h"

#include "XPathEvalDlg.h"

using namespace QuickXml;

std::wstring currentXPath(bool preciseXPath) {
    dbgln("currentXPath()");

    XmlFormater* formater = NULL;
    int currentEdit;
    std::string::size_type currentLength, currentPos;
    std::wstring nodepath(L"");

    ::SendMessage(nppData._nppHandle, NPPM_GETCURRENTSCINTILLA, 0, (LPARAM)&currentEdit);
    HWND hCurrentEditView = getCurrentHScintilla(currentEdit);
    currentLength = (int) ::SendMessage(hCurrentEditView, SCI_GETLENGTH, 0, 0);

    char* data = new char[currentLength + sizeof(char)];
    if (!data) return nodepath;  // allocation error, abort check
    size_t size = (currentLength + sizeof(char)) * sizeof(char);
    memset(data, '\0', size);

    currentPos = long(::SendMessage(hCurrentEditView, SCI_GETCURRENTPOS, 0, 0));
    ::SendMessage(hCurrentEditView, SCI_GETTEXT, currentLength + sizeof(char), reinterpret_cast<LPARAM>(data));

    formater = new XmlFormater(data, currentLength);
    nodepath = Report::widen(formater->currentPath(currentPos, true)->str());
    delete[] data;
    delete formater;

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

    if (pXPathEvalDlg == NULL) {
        pXPathEvalDlg = new CXPathEvalDlg(NULL, NULL);
        pXPathEvalDlg->Create(CXPathEvalDlg::IDD, NULL);
    }
    pXPathEvalDlg->ShowWindow(SW_SHOW);
}
