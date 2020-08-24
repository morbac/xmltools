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

    int maxElementDepth = 255;
    bool insertIndents = false;
    bool insertNewLines = false;
    bool removeWhitespace = false;
    bool autocloseEmptyElements = true;

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
    const char* prevTag = NULL;
    int prevTagLen = 0;

    #define tryCloseTag { if (tagIsOpen) { outText->write(">",1); tagIsOpen = false;} }
    // maxElementDepth to protect against unclosed tags in large files
    #define indent { tryCloseTag; if (parms.insertNewLines) {if (!indented && parms.insertIndents) for (int i = 0; i < xmllevel && i < parms.maxElementDepth; i++) outText->write(parms.tab.c_str(), parms.tab.length()); indented = true;}}
    #define newline { if (parms.insertNewLines) { outText->write(parms.eol.c_str(), parms.eol.length()); indented = false;}}

    while (curpos < endpos)
    {
        const char* startpos;

        // find first tag start
        for (startpos = curpos; curpos < endpos && *curpos != '<'; curpos++);

        // handle in-between data
        if (startpos != curpos) {
            bool originalTagOpen = tagIsOpen; // these 2 variables are only used in a special case: text not directly enclosed in a tag
            auto originalPos = outText->tellp();
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
            if (originalTagOpen == false)
            {
                auto newPos = outText->tellp();
                if (originalPos != newPos) // case of 'not in tag and extra data'.. make sure closing tag is on a new line
                    newline;
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
                int tagLen = static_cast<int>((end - 1) - (curpos + 2));
                if (parms.autocloseEmptyElements && prevTag != NULL && prevTagLen == tagLen && strncmp(curpos+2, prevTag, prevTagLen) == 0) {
                    tagIsOpen = false;
                    outText->write("/>", 2);
                }
                else {
                    tryCloseTag;
                    outText->write(curpos, end - startpos);
                }
            }
            else {
                indent;
                outText->write(curpos, end - startpos);
            }
            curpos = end;
            newline;

            continue;
        }

        if (tagIsOpen)
        {
            tryCloseTag
            newline
        }
        indent

        // find end of tag
        bool endFound = false;
        bool inAttributeValue = false;
        bool inWhiteSpace = false;
        bool selfClosing = false;
        const char* textstart = curpos; // includes the <

        prevTag = NULL;
        prevTagLen = 0;

        for (startpos = curpos; curpos < endpos; curpos++)
        {
#define emitTxt\
            if (prevTag == NULL) { prevTag = startpos + 1; prevTagLen = static_cast<int>(curpos - startpos) - 1; }\
            outText->write(textstart, curpos - textstart);

            char cc = *curpos;

            if (inAttributeValue) {
                if (cc == '"')
                    inAttributeValue = false;
            }
            else if (cc == '"') {
                inAttributeValue = true;
                if (textstart == NULL)
                    textstart = curpos;
            }
            else if (cc == '>') {
                endFound = true;
                if (curpos[-1] == '/')
                    selfClosing = true;

                if (textstart != NULL)
                {
                    if (selfClosing) { // no need for extra level, as it is self-closed
                        curpos++; // skip > BEFORE write
                        emitTxt
                        prevTag = NULL;
                        newline;
                    }
                    else {
                        emitTxt
                        curpos++; // skip > AFTER write
                        tagIsOpen = true;
                        xmllevel++;
                    }
                }
                break;
            }
            else if (isWhitespace(cc)) {
                if (textstart) {
                    emitTxt
                    textstart = NULL;
                }
                inWhiteSpace = true;
            }
            else {
                // so its text..
                if (inWhiteSpace) {
                    if (cc != '=') { // exception for: attribute = "value" ... this allows for the removal of the space after 'attribute'
                        outText->write(" ", 1);
                    }
                    inWhiteSpace = false;
                    textstart = curpos;
                }
            }
        }

        if (endFound) {
            continue;
        }
        else { // not found, abort (write remaining as is)
            outText->write(textstart, endpos - textstart);
            curpos = endpos;
        }

        // inifinite loop protection
        if (curpos == lastpos) {
            dbgln("PRETTYPRINT: INIFINITE LOOP DETECTED", DBG_LEVEL::DBG_ERROR);

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
    parms.autocloseEmptyElements = xmltoolsoptions.ppAutoclose;
    if (xmltoolsoptions.maxElementDepth > 0)
        parms.maxElementDepth = xmltoolsoptions.maxElementDepth;

    auto docclock_start = clock();
    std::stringstream* prettyTextStream = prettyPrintXml(inText.text, inText.length, parms);
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
    delete prettyTextStream;
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
    std::stringstream* prettyTextStream = prettyPrintXml(inText.text, inText.length, parms);
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
    delete prettyTextStream;
}

void nppPrettyPrintXmlFast() {
    nppDocumentCommand("PrettyPrintFast", sciDocPrettyPrintXML);
}

void nppLinearizeXmlFast() {
    nppDocumentCommand("LinearizeFast", sciDocLinearizeXML);
}