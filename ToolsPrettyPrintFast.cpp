#include "StdAfx.h"
#include <sstream>
#include "XMLTools.h"
#include "PluginInterface.h"
#include "Report.h"
#include "nppHelpers.h"
#include "XmlParser.h"
#include "XmlFormater.h"
#include "SimpleXml.h"

using namespace SimpleXml;
using namespace QuickXml;

void sciDocPrettyPrintQuickXml(ScintillaDoc& doc) {
    ScintillaDoc::sciWorkText inText = doc.GetWorkText();
    if (inText.text == NULL) {
        return;
    }

    XmlFormaterParamsType params;
    params.indentChars = doc.Tab();
    params.eolChars = doc.EOL();
    params.maxIndentLevel = 255;
    params.enforceConformity = !xmltoolsoptions.trimTextWhitespace;
    params.autoCloseTags = xmltoolsoptions.ppAutoclose;
    params.indentAttributes = false;
    params.indentOnly = false;

    if (xmltoolsoptions.maxElementDepth > 0) {
        params.maxIndentLevel = xmltoolsoptions.maxElementDepth;
    }

    auto docclock_start = clock();

    XmlFormater formater(inText.text, inText.length, params);
    std::string& outText = formater.prettyPrint()->str();

    auto docclock_end = clock();

    {
        std::string txt;
        txt += "crunching => time taken: " + std::to_string(docclock_end - docclock_start) + " ms";
        dbgln(txt.c_str());
    }

    inText.FreeMemory();
    doc.SetWorkText(outText.c_str());
    doc.SetScrollWidth(80);
}

void sciDocPrettyPrintSimpleXml(ScintillaDoc& doc) {
    ScintillaDoc::sciWorkText inText = doc.GetWorkText();
    if (inText.text == NULL) {
        return;
    }

    PrettyPrintParms parms;
    parms.eol = doc.EOL();
    parms.tab = doc.Tab();
    parms.insertIndents = true;
    parms.insertNewLines = true;
    parms.removeWhitespace = xmltoolsoptions.trimTextWhitespace;
    parms.autocloseEmptyElements = xmltoolsoptions.ppAutoclose;
    if (xmltoolsoptions.maxElementDepth > 0) {
        parms.maxElementDepth = xmltoolsoptions.maxElementDepth;
    }
    parms.keepExistingBreaks = false;

    auto docclock_start = clock();

    std::function chunker = [&doc](size_t a, char* b, size_t c) { return doc.GetText((Sci_PositionCR)a, b, (Sci_PositionCR)c); };
    ChunkedStream stream(1024 * 1024, chunker);
    auto prettyPrinter = PrettyPrinter(stream, parms);
    prettyPrinter.Convert();

    auto prettyTextStream = prettyPrinter.Stream();
    auto docclock_end = clock();

    {
        std::string txt;
        txt += "crunching => time taken: " + std::to_string(docclock_end - docclock_start) + " ms";
        dbgln(txt.c_str());
    }

    // Send formatted string to scintilla
    const std::string& outText = prettyTextStream->str();
    doc.SetWorkText(outText.c_str());
    doc.SetScrollWidth(80); // 80 is arbitrary
}

void sciDocPrettyPrintQuickXmlAttr(ScintillaDoc& doc) {
    ScintillaDoc::sciWorkText inText = doc.GetWorkText();
    if (inText.text == NULL) {
        return;
    }

    XmlFormaterParamsType params;
    params.indentChars = doc.Tab();
    params.eolChars = doc.EOL();
    params.maxIndentLevel = 255;
    params.enforceConformity = !xmltoolsoptions.trimTextWhitespace;
    params.autoCloseTags = xmltoolsoptions.ppAutoclose;
    params.indentAttributes = true;
    params.indentOnly = false;

    if (xmltoolsoptions.maxElementDepth > 0) {
        params.maxIndentLevel = xmltoolsoptions.maxElementDepth;
    }

    auto docclock_start = clock();

    XmlFormater formater(inText.text, inText.length, params);
    std::string& outText = formater.prettyPrint()->str();

    auto docclock_end = clock();

    {
        std::string txt;
        txt += "crunching => time taken: " + std::to_string(docclock_end - docclock_start) + " ms";
        dbgln(txt.c_str());
    }

    inText.FreeMemory();
    doc.SetWorkText(outText.c_str());
    doc.SetScrollWidth(80);
}

void sciDocPrettyPrintSimpleXmlAttr(ScintillaDoc& doc) {

    PrettyPrintParms parms;
    parms.eol = doc.EOL();
    parms.tab = doc.Tab();
    parms.insertIndents = true;
    parms.insertNewLines = true;
    parms.removeWhitespace = xmltoolsoptions.trimTextWhitespace;
    parms.autocloseEmptyElements = xmltoolsoptions.ppAutoclose;
    if (xmltoolsoptions.maxElementDepth > 0) {
        parms.maxElementDepth = xmltoolsoptions.maxElementDepth;
    }
    parms.keepExistingBreaks = false;
    parms.indentAttributes = true;

    auto docclock_start = clock();
    std::function chunker = [&doc](size_t a, char* b, size_t c) { return doc.GetText((Sci_PositionCR)a, b, (Sci_PositionCR)c); };
    ChunkedStream stream(1024 * 1024, chunker);
    auto prettyPrinter = PrettyPrinter(stream, parms);
    prettyPrinter.Convert();

    auto prettyTextStream = prettyPrinter.Stream();
    auto docclock_end = clock();

    {
        std::string txt;
        txt += "crunching => time taken: " + std::to_string(docclock_end - docclock_start) + " ms";
        dbgln(txt.c_str());
    }

    // Send formatted string to scintilla
    const std::string& outText = prettyTextStream->str();
    doc.SetWorkText(outText.c_str());
    doc.SetScrollWidth(80); // 80 is arbitrary
}

void sciDocPrettyPrintSimpleXml_IndentOnly(ScintillaDoc& doc) {
    PrettyPrintParms parms;
    parms.eol = doc.EOL();
    parms.tab = doc.Tab();
    parms.insertIndents = true;
    parms.insertNewLines = true;
    parms.removeWhitespace = xmltoolsoptions.trimTextWhitespace;
    parms.autocloseEmptyElements = xmltoolsoptions.ppAutoclose;
    if (xmltoolsoptions.maxElementDepth > 0) {
        parms.maxElementDepth = xmltoolsoptions.maxElementDepth;
    }
    parms.keepExistingBreaks = true;

    auto docclock_start = clock();

    std::function chunker = [&doc](size_t a, char* b, size_t c) { return doc.GetText((Sci_PositionCR)a, b, (Sci_PositionCR)c); };
    SimpleXml::ChunkedStream stream(1024 * 1024, chunker);
    auto prettyPrinter = PrettyPrinter(stream, parms);
    prettyPrinter.Convert();

    auto prettyTextStream = prettyPrinter.Stream();
    auto docclock_end = clock();

    {
        std::string txt;
        txt += "crunching => time taken: " + std::to_string(docclock_end - docclock_start) + " ms";
        dbgln(txt.c_str());
    }

    // Send formatted string to scintilla
    const std::string& outText = prettyTextStream->str();
    doc.SetWorkText(outText.c_str());
}

void sciDocLinearizeQuickXml(ScintillaDoc& doc) {
    ScintillaDoc::sciWorkText inText = doc.GetWorkText();
    if (inText.text == NULL) {
        return;
    }

    XmlFormaterParamsType params;
    params.indentChars = doc.Tab();
    params.eolChars = doc.EOL();
    params.maxIndentLevel = 255;
    params.enforceConformity = !xmltoolsoptions.trimTextWhitespace;
    params.autoCloseTags = xmltoolsoptions.ppAutoclose;
    params.indentAttributes = false;
    params.indentOnly = false;

    if (xmltoolsoptions.maxElementDepth > 0) {
        params.maxIndentLevel = xmltoolsoptions.maxElementDepth;
    }

    auto docclock_start = clock();

    XmlFormater formater(inText.text, inText.length, params);
    std::string& outText = formater.linearize()->str();

    auto docclock_end = clock();

    {
        std::string txt;
        txt += "crunching => time taken: " + std::to_string(docclock_end - docclock_start) + " ms";
        dbgln(txt.c_str());
    }

    inText.FreeMemory();
    doc.SetWorkText(outText.c_str());
    doc.SetScrollWidth(80);
}

void sciDocLinearizeSimpleXml(ScintillaDoc& doc) {
    auto inText = doc.GetWorkText();
    if (inText.text == NULL)
        return;

    PrettyPrintParms parms;
    parms.eol = "";
    parms.tab = "";
    parms.insertIndents = false;
    parms.insertNewLines = false;
    parms.removeWhitespace = true;
    parms.autocloseEmptyElements = xmltoolsoptions.ppAutoclose;
    parms.keepExistingBreaks = false;

    auto docclock_start = clock();

    SimpleXml::ChunkedStream s(inText.text, inText.length);
    auto prettyPrinter = PrettyPrinter(s, parms);
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
    const std::string& outText = prettyTextStream->str();
    doc.SetWorkText(outText.c_str());
}

void nppPrettyPrintXmlFast() {
    if (xmltoolsoptions.formatingEngine.compare(L"QuickXml") == 0) {
        nppMultiDocumentCommand(L"PrettyPrintFast (quickxml)", sciDocPrettyPrintQuickXml);
    }
    else {
        nppMultiDocumentCommand(L"PrettyPrintFast (simplexml)", sciDocPrettyPrintSimpleXml);
    }
}

void nppPrettyPrintXmlAttrFast() {
    if (xmltoolsoptions.formatingEngine.compare(L"QuickXml") == 0) {
        nppMultiDocumentCommand(L"PrettyPrintAttrFast (quickxml)", sciDocPrettyPrintQuickXmlAttr);
    }
    else {
        nppMultiDocumentCommand(L"PrettyPrintAttrFast (simplexml)", sciDocPrettyPrintSimpleXmlAttr);
    }
}

void nppPrettyPrintXmlIndentOnlyFast() {
    nppMultiDocumentCommand(L"PrettyPrintIndentOnlyFast", sciDocPrettyPrintSimpleXml_IndentOnly);
}

void nppLinearizeXmlFast() {
    if (xmltoolsoptions.formatingEngine.compare(L"QuickXml") == 0) {
        nppMultiDocumentCommand(L"LinearizeFast (quickxml)", sciDocLinearizeQuickXml);
    }
    else {
        nppMultiDocumentCommand(L"LinearizeFast (simplexml)", sciDocLinearizeSimpleXml);
    }
}