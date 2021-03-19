#include "StdAfx.h"
#include "Scintilla.h"
#include "PluginInterface.h"
#include "nppHelpers.h"
#include "Report.h"
#include "XMLTools.h"
#include "XmlWrapperInterface.h"
#include "MessageDlg.h"

#include <map>

std::map<LRESULT, bool> hasAnnotations;

void deleteAnnotations() {
    hasAnnotations.clear();
}

// tricky way of centering text on given vertical and horizontal position
// for vertical position, we get the contol size and estimate the number
// of lines based on text height then we force centering by going to wanted
// line +- nlines/2
// for horizontal position, we calculate the text width and check if it is
// out of the bounding box; then scroll horizontally if required
// @param view the view hwnd
// @param line the 1-based line position (first line is the line 1)
// @param hofs the height offset; when a vertical scroll is required, this offset
//             indicates how many additional line scroll are needed to avoid having
//             the line clamped at top or bottom line
// @param wofs the width offset; when an horizontal scroll is required, this offset
//             indicates how many additional columns scroll are needed to avoid having
//             the caret clamped at left or right border
// @param text : the line text begin; the text correspond to the left text part of line;
//               we need it as text to compute the text bounding box size (it might
//               differs depending on font type and size)
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
        if (line - 1 + nlines_2 > (size_t)last) {
            // line is on the end of document
            ::SendMessage(view, SCI_SHOWLINES, line - 1, line + hofs); // force surround lines visibles if folded
        }
        else {
            ::SendMessage(view, SCI_GOTOLINE, line - 1 - nlines_2, 0);
            ::SendMessage(view, SCI_GOTOLINE, line - 1 + nlines_2, 0);
        }

        ::SendMessage(view, SCI_GOTOLINE, line - 1, 0);

        // scroll horizontally to make column hofs visible
        size_t width = 0;
        if (text != NULL) {
            width = (size_t) ::SendMessage(view, SCI_TEXTWIDTH, 32, reinterpret_cast<LPARAM>(text));
        }
        else {
            char *tmpbuffer = new char[wofs + sizeof(char)];
            memset(tmpbuffer, ' ', wofs + sizeof(char));
            tmpbuffer[wofs] = '\0';
            width = (size_t) ::SendMessage(view, SCI_TEXTWIDTH, 32, reinterpret_cast<LPARAM>(tmpbuffer));
            delete[] tmpbuffer;
        }
        size_t margins = (size_t) ::SendMessage(view, SCI_GETMARGINS, 0, 0);
        size_t wmargin = 0;
        for (size_t i = 0; i < margins; ++i) {
            wmargin += (size_t) ::SendMessage(view, SCI_GETMARGINWIDTHN, i, 0);
        }

        if (width + wofs > ((size_t)rect.right - (size_t)rect.left - wmargin)) {
            // error message goes out of the bounding box
            size_t scroll = width + wofs - ((size_t)rect.right - (size_t)rect.left - wmargin);
            ::SendMessage(view, SCI_SETXOFFSET, (int)scroll, 0);
        }
    }
    catch (...) {}
}
void centerOnPosition(HWND view, size_t line, size_t col) {
    centerOnPosition(view, line, 3, col, NULL);
}

void displayXMLError(std::wstring wmsg, HWND view, size_t line, size_t linepos, size_t filepos, bool doCenterOnPos) {
    // clear final \r\n
    std::string::size_type p = wmsg.find_last_not_of(L"\r\n");
    if (p != std::string::npos && p < wmsg.length()) {
        wmsg.erase(p + 1, std::string::npos);
    }

    if (view == NULL) {
        int currentEdit;
        ::SendMessage(nppData._nppHandle, NPPM_GETCURRENTSCINTILLA, 0, (LPARAM)&currentEdit);
        view = getCurrentHScintilla(currentEdit);
    }

    LRESULT bufferid = ::SendMessage(nppData._nppHandle, NPPM_GETCURRENTBUFFERID, 0, 0);
    UniMode encoding = UniMode(::SendMessage(nppData._nppHandle, NPPM_GETBUFFERENCODING, bufferid, 0));

    if (xmltoolsoptions.errorDisplayMode.compare(L"Annotation") == 0) {
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
                    size_t tabwidth = (size_t) ::SendMessage(view, SCI_GETTABWIDTH, 0, 0);
                    if (tabwidth <= 0) tabwidth = 4;
                    for (i = 0; i < tabwidth; ++i) tabs += ' ';

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

        if (doCenterOnPos) {
            centerOnPosition(view, line, std::count(wmsg.begin(), wmsg.end(), '\n'), maxannotwidth, buffer);
        }

        if (buffer != NULL) delete[] buffer;
    }
    else {
        if (linepos != NULL) {
            if (doCenterOnPos) {
                centerOnPosition(view, line, linepos);
            }
            Report::_printf_err(Report::str_format(L"Line %d, pos %d:\r\n%s", line, linepos, wmsg.c_str()));
        }
        else {
            Report::_printf_err(wmsg);
        }
    }

    if (doCenterOnPos && filepos != NULL) {
        ::SendMessage(view, SCI_GOTOPOS, filepos - 1, 0);
    }
}

void displayXMLError(IXMLDOMParseError* pXMLErr, HWND view, const wchar_t* szDesc, bool doCenterOnPos) {
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

        displayXMLError(wmsg, view, line, linepos, (size_t)filepos + 1, doCenterOnPos);
    }

CleanUp:
    SysFreeString(bstrReason);
}

std::wstring formatXMLError(IXMLDOMParseError* pXMLErr) {
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

        if (line > 0 && linepos > 0) {
            wmsg = Report::str_format(L"Line %d, pos %d: %s", line, linepos, bstrReason);
        }
        else {
            wmsg = Report::str_format(L"%s", bstrReason);
        }
    }

CleanUp:
    SysFreeString(bstrReason);

    return wmsg;
}

void displayXMLErrors(IXMLDOMParseError* pXMLErr, HWND view, const wchar_t* szDesc) {
    HRESULT hr = S_OK;
    IXMLDOMParseError2* pTmpErr = NULL;
    IXMLDOMParseErrorCollection* pAllErrors = NULL;
    long length = 0;

    if (xmltoolsoptions.errorDisplayMode.compare(L"Dialog") == 0) {
        Report::clearLog();
        Report::registerMessage(szDesc);
        Report::registerMessage(L"");

        try {
            CHK_HR(((IXMLDOMParseError2*)pXMLErr)->get_allErrors(&pAllErrors));
            if (pAllErrors != NULL) {
                CHK_HR(pAllErrors->get_length(&length));
                for (long i = 0; i < length; ++i) {
                    CHK_HR(pAllErrors->get_next(&pTmpErr));
                    CHK_HR(pAllErrors->get_item(i, &pTmpErr));
                    Report::registerError(formatXMLError(pTmpErr).c_str());
                    SAFE_RELEASE(pTmpErr);
                }
            }
            else {
                Report::registerError(szDesc);
            }
        }
        catch (...) {
            Report::registerError(szDesc);
        }

        CMessageDlg* msgdlg = new CMessageDlg();
        msgdlg->m_sMessage = Report::getLog().c_str();
        msgdlg->DoModal();
    }
    else {
        try {
            CHK_HR(((IXMLDOMParseError2*)pXMLErr)->get_allErrors(&pAllErrors));
            if (pAllErrors != NULL) {
                CHK_HR(pAllErrors->get_length(&length));
                for (long i = 0; i < length; ++i) {
                    CHK_HR(pAllErrors->get_next(&pTmpErr));
                    CHK_HR(pAllErrors->get_item(i, &pTmpErr));
                    displayXMLError(pTmpErr, view, szDesc, i == 0);
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
    }

CleanUp:
    SAFE_RELEASE(pTmpErr);
    SAFE_RELEASE(pAllErrors);
}

void displayXMLErrors(std::vector<ErrorEntryType> errors, HWND view, const wchar_t* szDesc) {
    if (xmltoolsoptions.errorDisplayMode.compare(L"Dialog") == 0) {
        Report::clearLog();
        Report::registerMessage(szDesc);
        Report::registerMessage(L"");

        for (std::vector<ErrorEntryType>::iterator it = errors.begin(); it != errors.end(); ++it) {
            if ((*it).line < 0) {
                Report::registerError((*it).reason.c_str());
            }
            else {
                Report::registerError(Report::str_format(L"Line %d, pos %d: %s", (*it).line, (*it).linepos, (*it).reason.c_str()).c_str());
            }
        }

        CMessageDlg* msgdlg = new CMessageDlg();
        msgdlg->m_sMessage = Report::getLog().c_str();
        msgdlg->DoModal();
    }
    else {
        for (std::vector<ErrorEntryType>::iterator it = errors.begin(); it != errors.end(); ++it) {
            if ((*it).line < 0) {
                Report::_printf_err((*it).reason);
            }
            else {
                bool isFirst = (it == errors.begin());
                displayXMLError((*it).reason, view, (*it).line, (*it).linepos, (*it).filepos, isFirst);
            }
        }
    }
}

void previousError() {
    // @todo To be implemented
}

void nextError() {
    // @todo To be implemented
}

bool hasCurrentDocAnnotations() {
    if (xmltoolsoptions.errorDisplayMode.compare(L"Annotation") != 0) return false;
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
