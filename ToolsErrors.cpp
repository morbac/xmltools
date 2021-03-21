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
std::vector<ErrorEntryDesc> xmlErrors;
size_t currentError = 0;

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
void centerOnPosition(HWND view, size_t line, size_t hofs, size_t wofs, size_t width) {
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
        if (width == 0) {
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

void highlightCurrentError() {
    if (xmlErrors.size() == 0) return;

    // @todo To be implemented
    ErrorEntryDesc err = xmlErrors[currentError];

    if (err.positioned) {
        centerOnPosition(err.view, err.line, err.numlines, err.maxannotwidth, err.width);

        if (err.filepos > 0) {
            ::SendMessage(err.view, SCI_GOTOPOS, err.filepos - 1, 0);
        }
    }
}

void firstError() {
    if (xmlErrors.size() == 0) return;

    currentError = 0;
    highlightCurrentError();
}

void previousError() {
    if (xmlErrors.size() == 0) return;

    if (currentError > 0) --currentError;
    highlightCurrentError();
}

void nextError() {
    if (xmlErrors.size() == 0) return;

    if (currentError < xmlErrors.size() - 1) ++currentError;
    highlightCurrentError();
}

void lastError() {
    if (xmlErrors.size() == 0) return;

    currentError = xmlErrors.size() - 1;
    highlightCurrentError();
}

void deleteAnnotations() {
    hasAnnotations.clear();
}

ErrorEntryDesc displayXMLError(std::wstring wmsg, HWND view, size_t line, size_t linepos, size_t filepos) {
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
    ErrorEntryDesc res;

    res.view = view;

    if (xmltoolsoptions.errorDisplayMode.compare(L"Annotation") == 0) {
        if (line == NULL) {
            line = (size_t) ::SendMessage(nppData._nppHandle, NPPM_GETCURRENTLINE, 0, 0) + 1;
        }

        size_t maxannotwidth = 0;
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
                    size_t tmpannotwidth;
                    while (pos != std::string::npos) {
                        tmpannotwidth = (size_t) ::SendMessage(view, SCI_TEXTWIDTH, 32, reinterpret_cast<LPARAM>(Report::castChar(wmsg.substr(oldpos, pos - oldpos), encoding).c_str()));
                        if (tmpannotwidth > maxannotwidth) maxannotwidth = tmpannotwidth;

                        pos += lstrlen(L"\r\n");
                        wmsg.insert(pos, spaces);
                        oldpos = pos + spaces.length();
                        pos = wmsg.find(L"\r\n", pos);
                    }

                    // calculate last line
                    tmpannotwidth = (size_t) ::SendMessage(view, SCI_TEXTWIDTH, 32, reinterpret_cast<LPARAM>(Report::castChar(wmsg.substr(oldpos, wmsg.length() - oldpos), encoding).c_str()));
                    if (tmpannotwidth > maxannotwidth) maxannotwidth = tmpannotwidth;

                    spaces.clear();
                }
            }
        }

        // let's see if another annotation is already present on line
        std::string wannot = "";
        int annotationLength = (int) ::SendMessage(view, SCI_ANNOTATIONGETTEXT, line - 1, NULL);
        if (annotationLength > 0) {
            char* annotationText = new char[annotationLength+1];
            memset(annotationText, '\0', annotationLength+1);
            ::SendMessage(view, SCI_ANNOTATIONGETTEXT, line - 1, reinterpret_cast<LPARAM>(annotationText));

            if (annotationText != NULL) {
                wannot.append(annotationText).append("\r\n");
                res.start = wannot.length();
                delete[] annotationText;
            }
        }

        std::string msg = Report::castChar(wmsg, encoding);
        res.length = msg.length();
        ::SendMessage(view, SCI_ANNOTATIONSETTEXT, line - 1, reinterpret_cast<LPARAM>((wannot + msg).c_str()));
        wannot.clear();

        ::SendMessage(view, SCI_ANNOTATIONSETSTYLE, line - 1, xmltoolsoptions.annotationStyle);
        ::SendMessage(view, SCI_ANNOTATIONSETVISIBLE, 1, NULL);

        hasAnnotations[::SendMessage(nppData._nppHandle, NPPM_GETCURRENTBUFFERID, 0, 0)] = true;

        res.positioned = TRUE;
        res.line = line;
        res.linepos = linepos;
        res.filepos = filepos;
        res.numlines = (size_t)std::count(wmsg.begin(), wmsg.end(), '\n');
        res.maxannotwidth = maxannotwidth;

        if (buffer != NULL) {
            res.width = (size_t) ::SendMessage(view, SCI_TEXTWIDTH, 32, reinterpret_cast<LPARAM>(buffer));
            delete[] buffer;
        }
    }
    else {
        if (linepos != NULL) {
            Report::_printf_err(Report::str_format(L"Line %d, pos %d:\r\n%s", line, linepos, wmsg.c_str()));

            res.positioned = TRUE;
            res.line = line;
            res.linepos = linepos;
            res.filepos = filepos;
            res.numlines = (size_t)std::count(wmsg.begin(), wmsg.end(), '\n');
            res.maxannotwidth = 0;
            res.width = 0;
        }
        else {
            Report::_printf_err(wmsg);

            res.positioned = FALSE;
            res.numlines = (size_t)std::count(wmsg.begin(), wmsg.end(), '\n');
        }
    }

    return res;
}

void displayXMLErrors(std::vector<ErrorEntryType> errors, HWND view, const wchar_t* szDesc) {
    if (xmltoolsoptions.errorDisplayMode.compare(L"Dialog") == 0) {
        Report::clearLog();
        Report::registerMessage(szDesc);
        Report::registerMessage(L"");

        for (std::vector<ErrorEntryType>::iterator it = errors.begin(); it != errors.end(); ++it) {
            if ((*it).line < 0) {
                registerError({ view, (*it).positioned, (*it).line, (*it).linepos, (*it).filepos, 1, 0, 0 });
                Report::registerError((*it).reason.c_str());
            }
            else {
                std::wstring text = (*it).reason;
                registerError({ view, (*it).positioned, (*it).line, (*it).linepos, (*it).filepos, 1, 0, 0 });
                Report::registerError(Report::str_format(L"Line %d, pos %d: %s", (*it).line, (*it).linepos, text.c_str()).c_str());
            }
        }

        CMessageDlg* msgdlg = new CMessageDlg();
        msgdlg->m_sMessage = Report::getLog().c_str();
        msgdlg->DoModal();
    }
    else {
        for (std::vector<ErrorEntryType>::iterator it = errors.begin(); it != errors.end(); ++it) {
            if ((*it).line < 0) {
                std::wstring text = (*it).reason;
                ErrorEntryDesc err = { view, FALSE, 0, 0, 0, (size_t) std::count(text.begin(), text.end(), '\n'), 0, 0 };
                registerError(err);
                Report::_printf_err(text);
            }
            else {
                registerError(displayXMLError((*it).reason, view, (*it).line, (*it).linepos, (*it).filepos));
            }
        }
    }

    firstError();
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

void clearErrors(HWND view) {
    currentError = 0;
    xmlErrors.clear();
    clearAnnotations(view);
}

void registerError(ErrorEntryDesc err) {
    xmlErrors.push_back(err);
}