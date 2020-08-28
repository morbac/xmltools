#include "StdAfx.h"
#include <sstream>
#include "XMLTools.h"
#include "PluginInterface.h"
#include "Report.h"
#include "nppHelpers.h"

#include "XmlPrettyPrinter.h"

void sciDocPrettyPrintXML(ScintillaDoc& doc) {
    auto inText = doc.GetWorkText();
    if (inText.text == NULL)
        return;

    PrettyPrintParms parms;
    parms.eol = doc.EOL();
    parms.tab = doc.Tab();
    parms.insertIndents = true;
    parms.insertNewLines = true;
    parms.removeWhitespace = true;
    parms.autocloseEmptyElements = xmltoolsoptions.ppAutoclose;
    if (xmltoolsoptions.maxElementDepth > 0)
        parms.maxElementDepth = xmltoolsoptions.maxElementDepth;

    auto docclock_start = clock();
    //std::stringstream* prettyTextStream = prettyPrintXml(inText.text, inText.length, parms);

    XmlPrettyPrinter prettyPrinter = XmlPrettyPrinter(inText.text, inText.length, parms);
    prettyPrinter.Convert();   
    auto prettyTextStream = prettyPrinter.Stream();

    delete inText.text;
    auto docclock_end = clock();

    {
        std::string txt;
        txt += "crunching => time taken: " + std::to_string(docclock_end - docclock_start) + " ms";
        dbgln(txt.c_str());
    }

    // Send formatted string to scintilla
    const std::string& outText = prettyTextStream->str();
    doc.SetWorkText(outText.c_str());
    //delete prettyTextStream;
}

void sciDocLinearizeXML(ScintillaDoc& doc) {
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

    auto docclock_start = clock();
    //std::stringstream* prettyTextStream = prettyPrintXml(inText.text, inText.length, parms);

    XmlPrettyPrinter prettyPrinter = XmlPrettyPrinter(inText.text, inText.length, parms);
    prettyPrinter.Convert();
    auto prettyTextStream = prettyPrinter.Stream();

    delete[] inText.text;
    auto docclock_end = clock();

    {
        std::string txt;
        txt += "crunching => time taken: " + std::to_string(docclock_end - docclock_start) + " ms";
        dbgln(txt.c_str());
    }

    // Send formatted string to scintilla
    const std::string& outText = prettyTextStream->str();
    doc.SetWorkText(outText.c_str());
    //delete prettyTextStream;
}

void nppPrettyPrintXmlFast() {
    nppDocumentCommand("PrettyPrintFast", sciDocPrettyPrintXML);
}

void nppLinearizeXmlFast() {
    nppDocumentCommand("LinearizeFast", sciDocLinearizeXML);
}