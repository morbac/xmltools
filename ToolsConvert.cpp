#include "StdAfx.h"
#include "XMLTools.h"
#include "nppHelpers.h"
#include "Report.h"

void convertText2XML() {
    dbgln("convertText2XML()");

    int currentEdit, isReadOnly;
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

    char* data = new char[sellength + 1];
    if (!data) return;  // allocation error, abort check
    memset(data, '\0', sellength + 1);

    ::SendMessage(hCurrentEditView, SCI_GETSELTEXT, 0, reinterpret_cast<LPARAM>(data));

    std::string str(data);
    delete[] data;
    data = NULL;
    std::string::size_type curpos;

    if (xmltoolsoptions.convertApos) {
        curpos = sellength;
        while (curpos != std::string::npos && (curpos = str.rfind("&apos;", curpos)) != std::string::npos) {
            if (curpos != std::string::npos) {
                str.replace(curpos, strlen("&apos;"), "'");
                sellength = sellength - strlen("&apos;") + strlen("'");
            }
            --curpos;
        }
    }

    if (xmltoolsoptions.convertQuote) {
        curpos = sellength;
        while (curpos != std::string::npos && (curpos = str.rfind("&quot;", curpos)) != std::string::npos) {
            if (curpos != std::string::npos) {
                str.replace(curpos, strlen("&quot;"), "\"");
                sellength = sellength - strlen("&quot;") + strlen("\"");
            }
            --curpos;
        }
    }

    if (xmltoolsoptions.convertLt) {
        curpos = sellength;
        while (curpos != std::string::npos && (curpos = str.rfind("&lt;", curpos)) != std::string::npos) {
            if (curpos != std::string::npos) {
                str.replace(curpos, strlen("&lt;"), "<");
                sellength = sellength - strlen("&lt;") + strlen("<");
            }
            --curpos;
        }
    }

    if (xmltoolsoptions.convertGt) {
        curpos = sellength;
        while (curpos != std::string::npos && (curpos = str.rfind("&gt;", curpos)) != std::string::npos) {
            if (curpos != std::string::npos) {
                str.replace(curpos, strlen("&gt;"), ">");
                sellength = sellength - strlen("&gt;") + strlen(">");
            }
            --curpos;
        }
    }

    if (xmltoolsoptions.convertAmp) {
        curpos = sellength;
        while (curpos != std::string::npos && (curpos = str.rfind("&amp;", curpos)) != std::string::npos) {
            if (curpos != std::string::npos) {
                str.replace(curpos, strlen("&amp;"), "&");
                sellength = sellength - strlen("&amp;") + strlen("&");
            }
            --curpos;
        }
    }

    // Replace the selection with new string
    ::SendMessage(hCurrentEditView, SCI_REPLACESEL, 0, reinterpret_cast<LPARAM>(str.c_str()));

    // Defines selection without scrolling
    ::SendMessage(hCurrentEditView, SCI_SETCURRENTPOS, selstart, 0);
    ::SendMessage(hCurrentEditView, SCI_SETANCHOR, selstart + sellength, 0);

    str.clear();
}

///////////////////////////////////////////////////////////////////////////////

void convertXML2Text() {
    dbgln("convertXML2Text()");

    int currentEdit, isReadOnly;
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

    char* data = new char[sellength + 1];
    if (!data) return;  // allocation error, abort check
    memset(data, '\0', sellength + 1);

    ::SendMessage(hCurrentEditView, SCI_GETSELTEXT, 0, reinterpret_cast<LPARAM>(data));

    std::string str(data);
    delete[] data;
    data = NULL;
    std::string::size_type curpos;

    if (xmltoolsoptions.convertAmp) {
        curpos = sellength;
        while (curpos != std::string::npos && (curpos = str.rfind("&", curpos)) != std::string::npos) {
            if (curpos != std::string::npos) {
                str.replace(curpos, strlen("&"), "&amp;");
                sellength = sellength + strlen("&amp;") - strlen("&");
            }
            --curpos;
        }
    }

    if (xmltoolsoptions.convertLt) {
        curpos = sellength;
        while (curpos != std::string::npos && (curpos = str.rfind("<", curpos)) != std::string::npos) {
            if (curpos != std::string::npos) {
                str.replace(curpos, strlen("<"), "&lt;");
                sellength = sellength + strlen("&lt;") - strlen("<");
            }
            --curpos;
        }
    }

    if (xmltoolsoptions.convertGt) {
        curpos = sellength;
        while (curpos != std::string::npos && (curpos = str.rfind(">", curpos)) != std::string::npos) {
            if (curpos != std::string::npos) {
                str.replace(curpos, strlen(">"), "&gt;");
                sellength = sellength + strlen("&gt;") - strlen(">");
            }
            --curpos;
        }
    }

    if (xmltoolsoptions.convertQuote) {
        curpos = sellength;
        while (curpos != std::string::npos && (curpos = str.rfind("\"", curpos)) != std::string::npos) {
            if (curpos != std::string::npos) {
                str.replace(curpos, strlen("\""), "&quot;");
                sellength = sellength + strlen("&quot;") - strlen("\"");
            }
            --curpos;
        }
    }

    if (xmltoolsoptions.convertApos) {
        curpos = sellength;
        while (curpos != std::string::npos && (curpos = str.rfind("'", curpos)) != std::string::npos) {
            if (curpos != std::string::npos) {
                str.replace(curpos, strlen("'"), "&apos;");
                sellength = sellength + strlen("&apos;") - strlen("'");
            }
            --curpos;
        }
    }

    // Replace the selection with new string
    ::SendMessage(hCurrentEditView, SCI_REPLACESEL, 0, reinterpret_cast<LPARAM>(str.c_str()));

    // Defines selection without scrolling
    ::SendMessage(hCurrentEditView, SCI_SETCURRENTPOS, selstart, 0);
    ::SendMessage(hCurrentEditView, SCI_SETANCHOR, selstart + sellength, 0);

    str.clear();
}