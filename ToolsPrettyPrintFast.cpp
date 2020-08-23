#include "StdAfx.h"
#include <sstream>
#include "XMLTools.h"
#include "PluginInterface.h"
#include "Report.h"
#include "nppHelpers.h"

#define isWhitespace(x) ((x) == 0x20 || \
                          (x) == 0x9 || \
                          (x) == 0xD || \
                          (x) == 0x0A) // XML Standard paragraph 2.3 whitespace

struct PrettyPrintParms
{
    std::string eol;
    std::string tab;

    bool insertIndents = false;
    bool insertNewLines = false;
    bool removeWhitespace = false;

};

std::stringstream *prettyPrintXml(const char* text, long textLength, PrettyPrintParms parms) {
    const char* curpos = text, * endpos = text + textLength;
    const char* lastpos = NULL;
    std::stringstream* outText = new std::stringstream();

    // Since the text is nul-terminated, we do not need to do any end-of-string checks when comparing for the next character. 
    // In the case of the 'end of string', it will be 0 (valid memory position)

    int xmllevel = 0;
    bool indented = false;
    bool tagIsOpen = false; // we keep the tags open to be able to handle <foo>   </foo> => <foo/> conversion 

    #define tryCloseTag { if (tagIsOpen) { outText->write(">",1); tagIsOpen = false;} }
    // maxElementDepth to protect against unclosed tags in large files
    #define indent { tryCloseTag; if (parms.insertNewLines) {if (!indented && parms.insertIndents) for (int i = 0; i < xmllevel && i < xmltoolsoptions.maxElementDepth; i++) outText->write(parms.tab.c_str(), parms.tab.length()); indented = true;}}
    #define newline { if (parms.insertNewLines) { outText->write(parms.eol.c_str(), parms.eol.length()); indented = false;}}

    while (curpos < endpos)
    {
        const char* startpos;

        // find first tag start
        for (startpos = curpos; curpos < endpos && *curpos != '<'; curpos++);

        // handle in-between data
        if (startpos != curpos) {
            if (parms.removeWhitespace) {
                const char* endpos = curpos;
                bool inwhitespace = true;
                bool dirty = false;
                const char* textstart = NULL;

#define flush { if (textstart != NULL) {\
                auto len = curpos - textstart;  \
                if (!inwhitespace && len > 0) {\
                    indent; outText->write(textstart,len); \
                }\
                textstart=NULL;} }

                for (curpos = startpos; curpos < endpos; curpos++) {
                    if (isWhitespace(*curpos)) {
                        if (!inwhitespace) {
                            flush;
                            inwhitespace = true;
                        }
                    }
                    else { // not whitespace
                        if (inwhitespace) {
                            if (dirty)
                                outText->write(" ", 1);
                            textstart = curpos;
                            inwhitespace = false;
                            dirty = true;
                        }
                    }
                }
                flush
            }
            else {
                auto copyLength = curpos - startpos;
                tryCloseTag;
                outText->write(startpos, copyLength);
            }
        }

        startpos = curpos;
        // check data tags

        if (curpos[1] == '!') {
            tryCloseTag;

            if (curpos[2] == '-' && curpos[3] == '-') { // <!--

                const char* end = strstr(curpos + 4, "-->");
                if (end != NULL)
                    end += 3;
                else
                    end = endpos; // not found.. copy rest

                indent;
                outText->write(curpos, end - startpos);
                newline;
                curpos = end;
            }
            else if (0 == strncmp(curpos, "<![CDATA[", 9)) {
                const char* end = strstr(curpos + 9, "]]>");
                if (end != NULL)
                    end += 3;
                else
                    end = endpos; // not found.. copy rest

                indent;
                outText->write(curpos, end - startpos);
                newline;
                curpos = end;
            }
            else { // well... dont know... copy as is
                outText->write(curpos, 1);
                curpos++;
            }
            continue;
        }

        if (curpos[1] == '?')
            xmllevel--; // make sure we stay at 0

        if (curpos[1] == '/') { // </tag>
            const char* end = strchr(curpos + 2, '>');
            if (end == NULL)
                end = endpos;
            else
                end += 1;

            if (xmllevel > 0) xmllevel--;

            if (tagIsOpen) {
                tagIsOpen = false;
                outText->write("/>", 2);
            }
            else {
                indent;
                outText->write(curpos, end - startpos);
            }
            curpos = end;
            newline;

            continue;
        }

        tryCloseTag;

        // find end of tag
        bool inAttribute = false;
        bool endFound = false;
        bool inTag = true;
        bool inAttributeName = false;
        bool inAttributeValue = false;

        for (startpos = curpos; curpos < endpos; curpos++)
        {
            char cc = *curpos;
/*
            if (inTag)
            {
                if (isWhitespace(cc)) {
                    inTag = false;
                    outText->write(startpos, curpos - startpos);
                    startpos = curpos;
                }
            }
            else if (inAttributeName) {

            }
            else */if (inAttributeValue) {
                if (cc == '"')
                    inAttributeValue = false;
            }
            else if (cc == '"') {
                inAttributeValue = true;
            }
            else if (cc == '>') {
                endFound = true;
                break;
            }
        }

        if (endFound) {

            tryCloseTag;

            if (indented)
                newline;

            indent;

            if (curpos[-1] == '/') { // no need for extra level, as it is self-closed
                curpos++; // skip > BEFORE write
                outText->write(startpos, curpos - startpos);
                newline;
            }
            else {
                outText->write(startpos, curpos - startpos);
                curpos++; // skip > AFTER write
                tagIsOpen = true;
                xmllevel++;
            }
        }
        else { // not found, abort (write remaining as is)
            outText->write(startpos, endpos - startpos);
            curpos = endpos;
        }

        // inifinite loop protection
        if (curpos == lastpos) {
            dbgln("PRETTYPRINT: INIFINITE LOOP DETECTED");

            outText->write(curpos, endpos - curpos);
            break;
        }
        lastpos = curpos;
    }
    tryCloseTag;

    return outText;
}

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

    auto docclock_start = clock();
    std::stringstream* prettyTextStream = prettyPrintXml(inText.text, inText.length, parms);
    delete inText.text;
    auto docclock_end = clock();

    dbg("crunching");
    dbg(" => time taken: ");
    dbg(std::to_string(docclock_end - docclock_start).c_str());
    dbgln(" ms");

    // Send formatted string to scintilla
    const std::string& outText = prettyTextStream->str();
    doc.SetWorkText(outText.c_str());
    delete prettyTextStream;
}

void sciDocLinearizeXML(ScintillaDoc& doc) {
    auto inText = doc.GetWorkText();
    if (inText.text == NULL)
        return;

    PrettyPrintParms parms{};
    parms.eol = "";
    parms.tab = "";
    parms.insertIndents = false;
    parms.insertNewLines = false;
    parms.removeWhitespace = true;

    auto docclock_start = clock();
    std::stringstream* prettyTextStream = prettyPrintXml(inText.text, inText.length, parms);
    delete [] inText.text;
    auto docclock_end = clock();

    dbg("crunching");
    dbg(" => time taken: ");
    dbg(std::to_string(docclock_end - docclock_start).c_str());
    dbgln(" ms");

    // Send formatted string to scintilla
    const std::string& outText = prettyTextStream->str();
    doc.SetWorkText(outText.c_str());
    delete prettyTextStream;
}

void nppPrettyPrintXmlFast() {
    nppDocumentCommand("PrettyPrintFast", sciDocPrettyPrintXML);
}

void nppLinearizeXmlFast() {
    nppDocumentCommand("LinearizeFast", sciDocLinearizeXML);
}