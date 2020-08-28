#include "StdAfx.h"
#include "XMLTools.h"
#include "Scintilla.h"
#include "nppHelpers.h"
#include "Report.h"

#include "XPathEvalDlg.h"

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

    char* data = new char[currentLength + sizeof(char)];
    if (!data) return nodepath;  // allocation error, abort check
    size_t size = (currentLength + sizeof(char)) * sizeof(char);
    memset(data, '\0', size);

    currentPos = long(::SendMessage(hCurrentEditView, SCI_GETCURRENTPOS, 0, 0));
    ::SendMessage(hCurrentEditView, SCI_GETTEXT, currentLength + sizeof(char), reinterpret_cast<LPARAM>(data));

    std::string str(data);
    delete[] data;
    data = NULL;

    // end tag pos
    std::string::size_type begpos = str.find_first_of("<");
    std::string::size_type endpos = str.find_last_of(">");

    // let's reach the end of current tag (if we are inside a tag)
    if (currentPos > begpos && currentPos <= endpos) {
        currentPos = str.find_last_of("\"<>", currentPos - 1) + 1;
        bool cursorInAttribute = false;

        // check if we are in a closing tag
        if (currentPos >= 1 && currentPos < currentLength - 2 * sizeof(char) && str.at(currentPos - 1) == '<' && str.at(currentPos) == '!' && str.at(currentPos + 1) == '-' && str.at(currentPos + 2) == '-') {  // check if in a comment
            return nodepath;
        }
        else if (currentPos >= 1 && str.at(currentPos - 1) == '<' && str.at(currentPos) == '/') {
            // if we are inside closing tag (inside </x>, let's go back before '<' char so we are inside node)
            --currentPos;
        }
        else {
            if (currentPos >= 2 && str.at(currentPos - 1) == '\"' && str.at(currentPos - 2) == '=') {
                cursorInAttribute = true;
                currentPos = str.find('\"', currentPos) + 1;
            }
            else {
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
