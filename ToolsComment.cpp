#include "StdAfx.h"
#include "XMLTools.h"
#include "nppHelpers.h"
#include "Report.h"
#include <string>
#include <stack>

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
            }
            else {
                errflag = -3;
                break;
            }
        }
        if (!str.compare(curpos, 3, "<![")) {
            std::string::size_type endvalpos = str.find("]**", curpos);
            if (endvalpos != std::string::npos) checkstack.push(atoi(str.substr(curpos + 3, endvalpos).c_str()));
        }
        if (!str.compare(curpos, 3, "**[")) {
            if (!checkstack.empty()) {
                std::string::size_type endvalpos = str.find("]>", curpos);
                if (endvalpos != std::string::npos && atoi(str.substr(curpos + 3, endvalpos).c_str()) != checkstack.top()) errflag = -2;
                else checkstack.pop();
            }
            else {
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
    size_t sellength = selend - selstart;

    if (selend <= selstart) {
        Report::_printf_err(L"Please select text to transform before you call the function.");
        return;
    }

    char* data = new char[sellength + sizeof(char)];
    if (!data) return;  // allocation error, abort check
    memset(data, '\0', sellength + sizeof(char));

    ::SendMessage(hCurrentEditView, SCI_GETSELTEXT, 0, reinterpret_cast<LPARAM>(data));

    while (sellength >= 0 && (data[sellength] == '\r' || data[sellength] == '\n')) {
        data[sellength--] = '\0';
    }

    std::string str(data);
    delete[] data;
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
            int endval = atoi(str.substr(curpos + 3, endvalpos).c_str());
            char tmpstr[64];
            sprintf(tmpstr, "<!{%d}**", endval + 1);
            str.replace(curpos, endvalpos - curpos + 3, tmpstr);
            sellength += (strlen(tmpstr) - (endvalpos - curpos + 3));
        }
        --curpos;
    }
    curpos = sellength;
    while (curpos != std::string::npos && (curpos = str.rfind("**{", curpos)) != std::string::npos) {
        if (curpos != std::string::npos) {
            size_t endvalpos = str.find("}>", curpos);
            int endval = atoi(str.substr(curpos + 3, endvalpos).c_str());
            char tmpstr[64];
            sprintf(tmpstr, "**{%d}>", endval + 1);
            str.replace(curpos, endvalpos - curpos + 2, tmpstr);
            sellength += (strlen(tmpstr) - (endvalpos - curpos + 2));
        }
        --curpos;
    }

    curpos = sellength;
    while (curpos != std::string::npos && (curpos = str.rfind("<!--", curpos)) != std::string::npos) {
        if (curpos != std::string::npos) {
            str.replace(curpos, 4, "<!{1}**");
            sellength += 3;
        }
        --curpos;
    }
    curpos = sellength;
    while (curpos != std::string::npos && (curpos = str.rfind("-->", curpos)) != std::string::npos) {
        if (curpos != std::string::npos) {
            str.replace(curpos, 3, "**{1}>");
            sellength += 3;
        }
        --curpos;
    }

    str.insert(0, "<!--"); sellength += 4;
    str.insert(sellength, "-->"); sellength += 3;

    // Replace the selection with new string
    ::SendMessage(hCurrentEditView, SCI_REPLACESEL, 0, reinterpret_cast<LPARAM>(str.c_str()));

    // Defines selection without scrolling
    ::SendMessage(hCurrentEditView, SCI_SETCURRENTPOS, selstart, 0);
    ::SendMessage(hCurrentEditView, SCI_SETANCHOR, selstart + sellength, 0);

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
    size_t sellength = selend - selstart;

    if (selend <= selstart) {
        Report::_printf_err(L"Please select text to transform before you call the function.");
        return;
    }

    char* data = new char[sellength + sizeof(char)];
    if (!data) return;  // allocation error, abort check
    memset(data, '\0', sellength + sizeof(char));

    ::SendMessage(hCurrentEditView, SCI_GETSELTEXT, 0, reinterpret_cast<LPARAM>(data));

    std::string str(data);
    delete[] data;
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
            str.erase(curpos, 3);
            sellength -= 3;
        }
        --curpos;
    }
    curpos = sellength;
    while (curpos != std::string::npos && (curpos = str.rfind("<!--", curpos)) != std::string::npos) {
        if (curpos != std::string::npos) {
            str.erase(curpos, 4);
            sellength -= 4;
        }
        --curpos;
    }

    curpos = sellength;
    while (curpos != std::string::npos && (curpos = str.rfind("<!{", curpos)) != std::string::npos) {
        if (curpos != std::string::npos) {
            size_t endvalpos = str.find("}**", curpos);
            int endval = atoi(str.substr(curpos + 3, endvalpos).c_str());
            if (endval > 1) {
                char tmpstr[64];
                sprintf(tmpstr, "<!{%d}**", endval - 1);
                str.replace(curpos, endvalpos - curpos + 3, tmpstr);
                sellength += (strlen(tmpstr) - (endvalpos - curpos + 3));
            }
            else {
                str.replace(curpos, endvalpos - curpos + 3, "<!--");
                sellength += (4 - (endvalpos - curpos + 3));
            }
        }
        --curpos;
    }
    curpos = sellength;
    while (curpos != std::string::npos && (curpos = str.rfind("**{", curpos)) != std::string::npos) {
        if (curpos != std::string::npos) {
            size_t endvalpos = str.find("}>", curpos);
            int endval = atoi(str.substr(curpos + 3, endvalpos).c_str());
            if (endval > 1) {
                char tmpstr[64];
                sprintf(tmpstr, "**{%d}>", endval - 1);
                str.replace(curpos, endvalpos - curpos + 2, tmpstr);
                sellength += (strlen(tmpstr) - (endvalpos - curpos + 2));
            }
            else {
                str.replace(curpos, endvalpos - curpos + 2, "-->");
                sellength += (3 - (endvalpos - curpos + 2));
            }
        }
        --curpos;
    }

    // Replace the selection with new string
    ::SendMessage(hCurrentEditView, SCI_REPLACESEL, 0, reinterpret_cast<LPARAM>(str.c_str()));

    // Defines selection without scrolling
    ::SendMessage(hCurrentEditView, SCI_SETCURRENTPOS, selstart, 0);
    ::SendMessage(hCurrentEditView, SCI_SETANCHOR, selstart + sellength, 0);

    str.clear();
}
