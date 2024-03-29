#include "StdAfx.h"
#include <sstream>
#include "XMLTools.h"
#include "PluginInterface.h"
#include "Report.h"
#include "nppHelpers.h"
#include "XmlParser.h"
#include "XmlFormater.h"
#include "SimpleXml.h"
#include "StringXml.h"

void sciDocPrettyPrintQuickXml(ScintillaDoc& doc) {
    ScintillaDoc::sciWorkText inText = doc.GetWorkText();
    if (inText.text == NULL) {
        return;
    }

    QuickXml::XmlFormaterParamsType params;
    params.indentChars = doc.Tab();
    params.eolChars = doc.EOL();
    params.maxIndentLevel = xmltoolsoptions.maxIndentLevel;
    params.ensureConformity = xmltoolsoptions.ensureConformity;
    params.autoCloseTags = xmltoolsoptions.ppAutoclose;
    params.indentAttributes = false;
    params.indentOnly = false;
    params.applySpacePreserve = xmltoolsoptions.applySpacePreserve;

    auto docclock_start = clock();

    QuickXml::XmlFormater formater(inText.text, inText.length, params);
    std::stringstream* outText = formater.prettyPrint();

    auto docclock_end = clock();

    {
        std::string txt;
        txt += "crunching => time taken: " + std::to_string(docclock_end - docclock_start) + " ms";
        dbgln(txt.c_str());
    }

    inText.FreeMemory();
    doc.SetWorkText(outText->str().c_str());
    outText->str(std::string());
    doc.SetScrollWidth(80);
}

void sciDocPrettyPrintSimpleXml(ScintillaDoc& doc) {
    ScintillaDoc::sciWorkText inText = doc.GetWorkText();
    if (inText.text == NULL) {
        return;
    }

    SimpleXml::PrettyPrintParms parms;
    parms.eol = doc.EOL();
    parms.tab = doc.Tab();
    parms.insertIndents = true;
    parms.insertNewLines = true;
    parms.removeWhitespace = true;
    parms.autocloseEmptyElements = xmltoolsoptions.ppAutoclose;
    if (xmltoolsoptions.maxIndentLevel > 0) {
        parms.maxElementDepth = xmltoolsoptions.maxIndentLevel;
    }
    parms.keepExistingBreaks = false;

    auto docclock_start = clock();

    std::function chunker = [&doc](size_t a, char* b, size_t c) { return doc.GetText((Sci_PositionCR)a, b, (Sci_PositionCR)c); };
    SimpleXml::ChunkedStream stream(1024 * 1024, chunker);
    auto prettyPrinter = SimpleXml::PrettyPrinter(stream, parms);
    prettyPrinter.Convert();

    auto prettyTextStream = prettyPrinter.Stream();
    auto docclock_end = clock();

    {
        std::string txt;
        txt += "crunching => time taken: " + std::to_string(docclock_end - docclock_start) + " ms";
        dbgln(txt.c_str());
    }

    // Send formatted string to scintilla
    std::string& outText = prettyTextStream->str();
    doc.SetWorkText(outText.c_str());
    outText.clear();
    doc.SetScrollWidth(80); // 80 is arbitrary
}

void sciDocPrettyPrintStringXml(ScintillaDoc& doc) {
    ScintillaDoc::sciWorkText inText = doc.GetWorkText();
    if (inText.text == NULL) {
        return;
    }

    StringXml::XmlFormaterParamsType params;
    params.indentChars = doc.Tab();
    params.eolChars = doc.EOL();
    params.autoCloseTags = xmltoolsoptions.ppAutoclose;

    auto docclock_start = clock();

    std::string str(inText.text, inText.length);
    StringXml::XmlFormater formater(&str, params);
    formater.prettyPrint();

    auto docclock_end = clock();

    {
        std::string txt;
        txt += "crunching => time taken: " + std::to_string(docclock_end - docclock_start) + " ms";
        dbgln(txt.c_str());
    }

    inText.FreeMemory();
    doc.SetWorkText(str.c_str());
    str.clear();
    doc.SetScrollWidth(80);
}

//-----------------------------------------------------------------------------------------------//

void sciDocPrettyPrintQuickXmlAttr(ScintillaDoc& doc) {
    ScintillaDoc::sciWorkText inText = doc.GetWorkText();
    if (inText.text == NULL) {
        return;
    }

    QuickXml::XmlFormaterParamsType params;
    params.indentChars = doc.Tab();
    params.eolChars = doc.EOL();
    params.maxIndentLevel = xmltoolsoptions.maxIndentLevel;
    params.ensureConformity = xmltoolsoptions.ensureConformity;
    params.autoCloseTags = xmltoolsoptions.ppAutoclose;
    params.indentAttributes = true;
    params.indentOnly = false;
    params.applySpacePreserve = xmltoolsoptions.applySpacePreserve;

    auto docclock_start = clock();

    QuickXml::XmlFormater formater(inText.text, inText.length, params);
    std::stringstream* outText = formater.prettyPrint();

    auto docclock_end = clock();

    {
        std::string txt;
        txt += "crunching => time taken: " + std::to_string(docclock_end - docclock_start) + " ms";
        dbgln(txt.c_str());
    }

    inText.FreeMemory();
    doc.SetWorkText(outText->str().c_str());
    outText->str(std::string());
    doc.SetScrollWidth(80);
}

void sciDocPrettyPrintSimpleXmlAttr(ScintillaDoc& doc) {
    SimpleXml::PrettyPrintParms parms;
    parms.eol = doc.EOL();
    parms.tab = doc.Tab();
    parms.insertIndents = true;
    parms.insertNewLines = true;
    parms.removeWhitespace = true;
    parms.autocloseEmptyElements = xmltoolsoptions.ppAutoclose;
    if (xmltoolsoptions.maxIndentLevel > 0) {
        parms.maxElementDepth = xmltoolsoptions.maxIndentLevel;
    }
    parms.keepExistingBreaks = false;
    parms.indentAttributes = true;

    auto docclock_start = clock();
    std::function chunker = [&doc](size_t a, char* b, size_t c) { return doc.GetText((Sci_PositionCR)a, b, (Sci_PositionCR)c); };
    SimpleXml::ChunkedStream stream(1024 * 1024, chunker);
    auto prettyPrinter = SimpleXml::PrettyPrinter(stream, parms);
    prettyPrinter.Convert();

    auto prettyTextStream = prettyPrinter.Stream();
    auto docclock_end = clock();

    {
        std::string txt;
        txt += "crunching => time taken: " + std::to_string(docclock_end - docclock_start) + " ms";
        dbgln(txt.c_str());
    }

    // Send formatted string to scintilla
    std::string& outText = prettyTextStream->str();
    doc.SetWorkText(outText.c_str());
    outText.clear();
    doc.SetScrollWidth(80); // 80 is arbitrary
}

void sciDocPrettyPrintStringXmlAttr(ScintillaDoc& doc) {
    ScintillaDoc::sciWorkText inText = doc.GetWorkText();
    if (inText.text == NULL) {
        return;
    }

    StringXml::XmlFormaterParamsType params;
    params.indentChars = doc.Tab();
    params.eolChars = doc.EOL();
    params.autoCloseTags = xmltoolsoptions.ppAutoclose;

    auto docclock_start = clock();

    std::string str(inText.text, inText.length);
    StringXml::XmlFormater formater(&str, params);
    formater.prettyPrintAttr();

    auto docclock_end = clock();

    {
        std::string txt;
        txt += "crunching => time taken: " + std::to_string(docclock_end - docclock_start) + " ms";
        dbgln(txt.c_str());
    }

    inText.FreeMemory();
    doc.SetWorkText(str.c_str());
    str.clear();
    doc.SetScrollWidth(80);
}

//-----------------------------------------------------------------------------------------------//

void sciDocPrettyPrintQuickXml_IndentOnly(ScintillaDoc& doc) {
    ScintillaDoc::sciWorkText inText = doc.GetWorkText();
    if (inText.text == NULL) {
        return;
    }

    QuickXml::XmlFormaterParamsType params;
    params.indentChars = doc.Tab();
    params.eolChars = doc.EOL();
    params.maxIndentLevel = xmltoolsoptions.maxIndentLevel;
    params.ensureConformity = xmltoolsoptions.ensureConformity;
    params.autoCloseTags = xmltoolsoptions.ppAutoclose;
    params.indentAttributes = true;
    params.indentOnly = true;
    params.applySpacePreserve = xmltoolsoptions.applySpacePreserve;

    auto docclock_start = clock();

    QuickXml::XmlFormater formater(inText.text, inText.length, params);
    std::stringstream* outText = formater.prettyPrint();

    auto docclock_end = clock();

    {
        std::string txt;
        txt += "crunching => time taken: " + std::to_string(docclock_end - docclock_start) + " ms";
        dbgln(txt.c_str());
    }

    inText.FreeMemory();
    doc.SetWorkText(outText->str().c_str());
    outText->str(std::string());
    doc.SetScrollWidth(80);
}

void sciDocPrettyPrintSimpleXml_IndentOnly(ScintillaDoc& doc) {
    SimpleXml::PrettyPrintParms parms;
    parms.eol = doc.EOL();
    parms.tab = doc.Tab();
    parms.insertIndents = true;
    parms.insertNewLines = true;
    parms.removeWhitespace = true;
    parms.autocloseEmptyElements = xmltoolsoptions.ppAutoclose;
    if (xmltoolsoptions.maxIndentLevel > 0) {
        parms.maxElementDepth = xmltoolsoptions.maxIndentLevel;
    }
    parms.keepExistingBreaks = true;

    auto docclock_start = clock();

    std::function chunker = [&doc](size_t a, char* b, size_t c) { return doc.GetText((Sci_PositionCR)a, b, (Sci_PositionCR)c); };
    SimpleXml::ChunkedStream stream(1024 * 1024, chunker);
    auto prettyPrinter = SimpleXml::PrettyPrinter(stream, parms);
    prettyPrinter.Convert();

    auto prettyTextStream = prettyPrinter.Stream();
    auto docclock_end = clock();

    {
        std::string txt;
        txt += "crunching => time taken: " + std::to_string(docclock_end - docclock_start) + " ms";
        dbgln(txt.c_str());
    }

    // Send formatted string to scintilla
    std::string& outText = prettyTextStream->str();
    doc.SetWorkText(outText.c_str());
    outText.clear();
}

void sciDocPrettyPrintStringXml_IndentOnly(ScintillaDoc& doc) {
    ScintillaDoc::sciWorkText inText = doc.GetWorkText();
    if (inText.text == NULL) {
        return;
    }

    StringXml::XmlFormaterParamsType params;
    params.indentChars = doc.Tab();
    params.eolChars = doc.EOL();
    params.autoCloseTags = xmltoolsoptions.ppAutoclose;

    auto docclock_start = clock();

    std::string str(inText.text, inText.length);
    StringXml::XmlFormater formater(&str, params);
    formater.prettyPrintIndent();

    auto docclock_end = clock();

    {
        std::string txt;
        txt += "crunching => time taken: " + std::to_string(docclock_end - docclock_start) + " ms";
        dbgln(txt.c_str());
    }

    inText.FreeMemory();
    doc.SetWorkText(str.c_str());
    str.clear();
    doc.SetScrollWidth(80);
}

//-----------------------------------------------------------------------------------------------//

void sciDocLinearizeQuickXml(ScintillaDoc& doc) {
    ScintillaDoc::sciWorkText inText = doc.GetWorkText();
    if (inText.text == NULL) {
        return;
    }

    QuickXml::XmlFormaterParamsType params;
    params.indentChars = doc.Tab();
    params.eolChars = doc.EOL();
    params.maxIndentLevel = xmltoolsoptions.maxIndentLevel;
    params.ensureConformity = xmltoolsoptions.ensureConformity;
    params.autoCloseTags = xmltoolsoptions.ppAutoclose;
    params.indentAttributes = false;
    params.indentOnly = false;
    params.applySpacePreserve = xmltoolsoptions.applySpacePreserve;

    auto docclock_start = clock();

    QuickXml::XmlFormater formater(inText.text, inText.length, params);
    std::stringstream* outText = formater.linearize();

    auto docclock_end = clock();

    {
        std::string txt;
        txt += "crunching => time taken: " + std::to_string(docclock_end - docclock_start) + " ms";
        dbgln(txt.c_str());
    }

    inText.FreeMemory();
    doc.SetWorkText(outText->str().c_str());
    outText->str(std::string());
    doc.SetScrollWidth(80);
}

void sciDocLinearizeSimpleXml(ScintillaDoc& doc) {
    auto inText = doc.GetWorkText();
    if (inText.text == NULL)
        return;

    SimpleXml::PrettyPrintParms parms;
    parms.eol = "";
    parms.tab = "";
    parms.insertIndents = false;
    parms.insertNewLines = false;
    parms.removeWhitespace = true;
    parms.autocloseEmptyElements = xmltoolsoptions.ppAutoclose;
    parms.keepExistingBreaks = false;

    auto docclock_start = clock();

    SimpleXml::ChunkedStream s(inText.text, inText.length);
    auto prettyPrinter = SimpleXml::PrettyPrinter(s, parms);
    prettyPrinter.Convert();
    inText.FreeMemory();
    auto prettyTextStream = prettyPrinter.Stream();
    auto docclock_end = clock();

    {
        std::string txt;
        txt += "crunching => time taken: " + std::to_string(docclock_end - docclock_start) + " ms";
        dbgln(txt.c_str());
    }

    // Send formatted string to scintilla
    std::string& outText = prettyTextStream->str();
    doc.SetWorkText(outText.c_str());
    outText.clear();
}

void sciDocLinearizeStringXml(ScintillaDoc& doc) {
    ScintillaDoc::sciWorkText inText = doc.GetWorkText();
    if (inText.text == NULL) {
        return;
    }

    StringXml::XmlFormaterParamsType params;
    params.indentChars = doc.Tab();
    params.eolChars = doc.EOL();
    params.autoCloseTags = xmltoolsoptions.ppAutoclose;

    auto docclock_start = clock();

    std::string str(inText.text, inText.length);
    StringXml::XmlFormater formater(&str, params);
    formater.linearize();

    auto docclock_end = clock();

    {
        std::string txt;
        txt += "crunching => time taken: " + std::to_string(docclock_end - docclock_start) + " ms";
        dbgln(txt.c_str());
    }

    inText.FreeMemory();
    doc.SetWorkText(str.c_str());
    str.clear();
    doc.SetScrollWidth(80);
}

//-----------------------------------------------------------------------------------------------//

void sciDocTokenizeQuickXml(ScintillaDoc& doc) {
    ScintillaDoc::sciWorkText inText = doc.GetWorkText();
    if (inText.text == NULL) {
        return;
    }

    QuickXml::XmlFormaterParamsType params;
    params.indentChars = doc.Tab();
    params.eolChars = doc.EOL();
    params.maxIndentLevel = xmltoolsoptions.maxIndentLevel;
    params.ensureConformity = xmltoolsoptions.ensureConformity;
    params.autoCloseTags = xmltoolsoptions.ppAutoclose;
    params.indentAttributes = false;
    params.indentOnly = false;
    params.applySpacePreserve = xmltoolsoptions.applySpacePreserve;

    auto docclock_start = clock();

    QuickXml::XmlFormater formater(inText.text, inText.length, params);
    std::string outText = formater.debugTokens("\r\n", true);

    auto docclock_end = clock();

    {
        std::string txt;
        txt += "crunching => time taken: " + std::to_string(docclock_end - docclock_start) + " ms";
        dbgln(txt.c_str());
    }

    dbgln(outText.c_str(), DBG_LEVEL::DBG_ERROR);
    outText.clear();
}

//-----------------------------------------------------------------------------------------------//
extern LangType setAutoXMLType(bool force = FALSE);

void nppPrettyPrintXmlFast() {
    if (xmltoolsoptions.formatingEngine.compare(L"QuickXml") == 0) {
        nppMultiDocumentCommand(L"PrettyPrintFast (quickxml)", sciDocPrettyPrintQuickXml);
    }
    else if (xmltoolsoptions.formatingEngine.compare(L"StringXml") == 0) {
        nppMultiDocumentCommand(L"PrettyPrintFast (stringxml)", sciDocPrettyPrintStringXml);
    }
    else {
        nppMultiDocumentCommand(L"PrettyPrintFast (simplexml)", sciDocPrettyPrintSimpleXml);
    }

    setAutoXMLType();
}

void nppPrettyPrintXmlAttrFast() {
    if (xmltoolsoptions.formatingEngine.compare(L"QuickXml") == 0) {
        nppMultiDocumentCommand(L"PrettyPrintAttrFast (quickxml)", sciDocPrettyPrintQuickXmlAttr);
    }
    else if (xmltoolsoptions.formatingEngine.compare(L"StringXml") == 0) {
        nppMultiDocumentCommand(L"PrettyPrintAttrFast (stringxml)", sciDocPrettyPrintStringXmlAttr);
    }
    else {
        nppMultiDocumentCommand(L"PrettyPrintAttrFast (simplexml)", sciDocPrettyPrintSimpleXmlAttr);
    }

    setAutoXMLType();
}

void nppPrettyPrintXmlIndentOnlyFast() {
    if (xmltoolsoptions.formatingEngine.compare(L"QuickXml") == 0) {
        nppMultiDocumentCommand(L"PrettyPrintAttrFast (quickxml)", sciDocPrettyPrintQuickXml_IndentOnly);
    }
    else if (xmltoolsoptions.formatingEngine.compare(L"StringXml") == 0) {
        nppMultiDocumentCommand(L"PrettyPrintAttrFast (stringxml)", sciDocPrettyPrintStringXml_IndentOnly);
    }
    else {
        nppMultiDocumentCommand(L"PrettyPrintIndentOnlyFast", sciDocPrettyPrintSimpleXml_IndentOnly);
    }

    setAutoXMLType();
}

void nppLinearizeXmlFast() {
    if (xmltoolsoptions.formatingEngine.compare(L"QuickXml") == 0) {
        nppMultiDocumentCommand(L"LinearizeFast (quickxml)", sciDocLinearizeQuickXml);
    }
    else if (xmltoolsoptions.formatingEngine.compare(L"StringXml") == 0) {
        nppMultiDocumentCommand(L"LinearizeFast (stringxml)", sciDocLinearizeStringXml);
    }
    else {
        nppMultiDocumentCommand(L"LinearizeFast (simplexml)", sciDocLinearizeSimpleXml);
    }

    setAutoXMLType();
}

void nppTokenizeXmlFast() {
    nppMultiDocumentCommand(L"TokenizeFast (quickxml)", sciDocTokenizeQuickXml);
}