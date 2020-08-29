#include "StdAfx.h"
#include "XMLTools.h"
#include "nppHelpers.h"
#include "Report.h"

void strreplaceall(std::string& str, const char* find, const char* replacement) {
    auto curpos = str.size();
    auto findSize = strlen(find);
    while (curpos != std::string::npos && (curpos = str.rfind(find, curpos)) != std::string::npos) {
        if (curpos != std::string::npos) {
            str.replace(curpos, findSize, replacement);
        }
        --curpos;
    }
}

void sciConvertText2XML(ScintillaDoc &doc) {
    auto seltext = doc.GetSelectedText();

    if (!seltext) {
        Report::_printf_err(L"Please select text to transform before you call the function.");
        return;
    }

    std::string str(seltext.text);
    seltext.FreeMemory();

    if (xmltoolsoptions.convertApos) {
        strreplaceall(str, "&apos;", "'");
    }

    if (xmltoolsoptions.convertQuote) {
        strreplaceall(str, "&quot;", "\"");
    }

    if (xmltoolsoptions.convertLt) {
        strreplaceall(str, "&lt;", "<");
    }

    if (xmltoolsoptions.convertGt) {
        strreplaceall(str, "&gt;", ">");
    }

    // &amp needs to be the last one
    if (xmltoolsoptions.convertAmp) {
        strreplaceall(str, "&amp;", "&");
    }

    // Replace the selection with new string
    doc.ReplaceSelection(str.c_str());

    // Defines selection without scrolling
    doc.SetSelectionPos(seltext.selstart);
    doc.SetAnchor(seltext.selstart + str.length());
}

void nppConvertText2XML() {
    nppDocumentCommand(L"convertText2XML", sciConvertText2XML);
}

///////////////////////////////////////////////////////////////////////////////

void sciConvertXML2Text(ScintillaDoc &doc) {
    auto seltext = doc.GetSelectedText();

    if (!seltext) {
        Report::_printf_err(L"Please select text to transform before you call the function.");
        return;
    }

    std::string str(seltext.text);
    seltext.FreeMemory();

    // &amp needs to be the first one
    if (xmltoolsoptions.convertAmp) {
        strreplaceall(str, "&", "&amp;");
    }

    if (xmltoolsoptions.convertApos) {
        strreplaceall(str, "'", "&apos;");
    }

    if (xmltoolsoptions.convertQuote) {
        strreplaceall(str, "\"", "&quot;");
    }

    if (xmltoolsoptions.convertLt) {
        strreplaceall(str, "<", "&lt;");
    }

    if (xmltoolsoptions.convertGt) {
        strreplaceall(str, ">", "&gt;");
    }

    // Replace the selection with new string
    doc.ReplaceSelection(str.c_str());

    // Defines selection without scrolling
    doc.SetSelectionPos(seltext.selstart);
    doc.SetAnchor(seltext.selstart + str.length());
}

void nppConvertXML2Text() {
    nppDocumentCommand(L"convertXML2Text", sciConvertXML2Text);
}