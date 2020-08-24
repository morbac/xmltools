#include "StdAfx.h"
#include "nppHelpers.h"
#include "XMLTools.h"
#include "Report.h"
#include <string>

std::string& ltrim(std::string& str, const std::string& chars = "\t\n\v\f\r ") {
    str.erase(0, str.find_first_not_of(chars));
    return str;
}

std::string& rtrim(std::string& str, const std::string& chars = "\t\n\v\f\r ") {
    str.erase(str.find_last_not_of(chars) + 1);
    return str;
}

std::string& trim(std::string& str, const std::string& chars = "\t\n\v\f\r ") {
    return ltrim(rtrim(str, chars), chars);
}

std::string& trimxml(std::string& str, std::string eolchar, bool breaklines, bool breaktags, const std::string& chars = "\t\n\v\f\r ") {
    bool in_tag = false, in_header = false;
    std::string tagname = "";
    char cc;
    std::string::size_type curpos = 0, lastpos = 0, lastlen = 0, lasteolpos = 0, tmppos;
    size_t eolcharlen = eolchar.length();
    size_t eolcharpos = eolchar.find('\n');

    size_t strlen = str.length();

    while (curpos < strlen && (curpos = str.find_first_of("<>\"'\n", curpos)) != std::string::npos) {
        switch (cc = str.at(curpos)) {
        case '<': {
            if (curpos < strlen - 4 && !str.compare(curpos, 4, "<!--")) {            // is comment start ?
              // skip the comment
                curpos = str.find("-->", curpos + 1) + 2;

                // add line break if next non space char is "<"
                if (breaklines) {
                    tmppos = str.find_first_not_of(chars, curpos + 1);
                    if (tmppos != std::string::npos && str.at(tmppos) == '<' /*&& str.at(tmppos + 1) != '!'*/ && str.at(tmppos + 2) != '[') {
                        str.insert(curpos + 1, eolchar);
                    }
                }
            }
            else if (curpos < strlen - 9 && !str.compare(curpos, 9, "<![CDATA[")) {       // is CDATA start ?
              // skip the CDATA
                curpos = str.find("]]>", curpos + 1) + 2;
            }
            else if (curpos < strlen - 2 && !str.compare(curpos, 2, "</")) {              // end tag (ex: "</sample>")
                curpos = str.find(">", curpos + 1);

                // trim space chars between tagname and > char
                tmppos = str.find_last_not_of(chars, curpos - 1);
                if (tmppos < curpos - 1) {
                    str.erase(tmppos + 1, curpos - tmppos - 1);
                    curpos = tmppos + 1;
                }

                // add line break if next non space char is "<" (but not if <![)
                if (breaklines) {
                    tmppos = str.find_first_not_of(chars, curpos + 1);
                    if (tmppos != std::string::npos && str.at(tmppos) == '<' /*&& str.at(tmppos + 1) != '!'*/ && str.at(tmppos + 2) != '[') {
                        str.insert(curpos + 1, eolchar);
                    }
                }
            }
            else {
                in_tag = true;
                if (curpos < strlen - 2 && !str.compare(curpos, 2, "<?")) {
                    in_header = true;
                    ++curpos;
                }

                // skip the tag name
                char endtag = (in_header ? '?' : '/');
                tmppos = curpos;
                curpos = str.find_first_of("\t\n\v\f\r ?/>", curpos + 1);
                if (curpos != std::string::npos) {
                    tagname.clear();
                    tagname = str.substr(tmppos + 1, curpos - tmppos - 1);

                    tmppos = str.find_first_not_of("\t\n\v\f\r ", curpos);
                    if (tmppos != std::string::npos) {
                        // trim space before attribute or ">" char
                        str.erase(curpos, tmppos - curpos);
                        if (str.at(curpos) != '>' && str.at(curpos) != endtag) {
                            str.insert(curpos, " ");
                            ++curpos;
                        }
                    }
                    --curpos;
                }
            }
            break;
        }
        case '>': {
            if (in_tag) {
                in_tag = false;
                in_header = false;

                // add line break if next non space char is another opening tag (but not in case of <![)
                // exceptions:  <sample></sample>  is untouched
                //              <foo><bar/></foo>  becomes   <foo>
                //                                             <bar/>
                //                                           </foo>
                if (breaklines) {
                    bool is_closing = (curpos > 0 && str.at(curpos - 1) == '/');
                    tmppos = str.find_first_not_of(chars, curpos + 1);
                    if (tmppos != std::string::npos && str.at(tmppos) == '<' && (str.at(tmppos + 1) != '/' || is_closing) && /*str.at(tmppos + 1) != '!' &&*/ str.at(tmppos + 2) != '[') {
                        str.insert(curpos + 1, eolchar);
                    }
                }
            }
            break;
        }
        case '\"':
        case '\'': {
            if (in_tag) {
                // trim spaces arround "=" char
                tmppos = str.find_last_not_of("\t\n\v\f\r ", curpos - 1);
                if (tmppos != std::string::npos && tmppos < curpos && str.at(tmppos) == '=') {
                    // remove spaces after "="
                    str.erase(tmppos + 1, curpos - tmppos - 1);
                    curpos = tmppos + 1;
                    // remove spaces before "="
                    tmppos = str.find_last_not_of("\t\n\v\f\r ", tmppos - 1);
                    if (tmppos != std::string::npos) {
                        str.erase(tmppos + 1, curpos - tmppos - 2);
                        curpos = tmppos + 2;
                    }
                }
                // skip attribute text
                curpos = str.find(cc, curpos + 1);

                // trim spaces after attribute
                tmppos = str.find_first_not_of("\t\n\v\f\r ", curpos + 1);
                if (tmppos != std::string::npos) {
                    char endtag = '/';
                    if (in_header) endtag = '?';

                    // add line break if not the last attribute
                    if (breaktags && !in_header && str.at(tmppos) != '>' && str.at(tmppos) != endtag) {
                        str.insert(curpos + 1, eolchar);
                    }
                    else if (!breaktags) {
                        str.erase(curpos + 1, tmppos - curpos - 1);
                        if (str.at(curpos + 1) != '>' && str.at(curpos + 1) != endtag) {
                            str.insert(curpos + 1, " ");
                            ++curpos;
                        }
                    }
                }
            }
            break;
        }
        case '\n': {
            // trim line

            curpos -= eolcharpos;

            if (in_tag && !breaktags) {
                // we must remove line breaks
                tmppos = str.find_first_not_of("\t\n\v\f\r ", curpos + 1);
                if (tmppos != std::string::npos) {
                    str.erase(curpos, tmppos - curpos);
                }
                if (str.at(curpos - 1) == '"' || str.at(curpos - 1) == '\'') {
                    str.insert(curpos, " ");
                    ++curpos;
                }
            }
            else {  // = if (!in_tag || breaktags)
                std::string tmp = trim(str.substr(lasteolpos, curpos - lasteolpos));
                str.replace(lasteolpos, curpos - lasteolpos, tmp);
                curpos = lasteolpos + tmp.length();
                lasteolpos = curpos;

                while (lasteolpos >= eolcharlen && !str.compare(lasteolpos - eolcharlen, eolcharlen, eolchar)) {
                    lasteolpos -= eolcharlen;
                }

                lasteolpos += eolcharlen;
            }

            curpos += (eolcharlen - 1);

            break;
        }
        }

        ++curpos;

        // inifinite loop protection
        strlen = str.length();
        if (curpos == lastpos && lastlen == strlen) {
            dbgln("TRIM: INFINITE LOOP DETECTED", DBG_LEVEL::DBG_ERROR);
            break;
        }
        lastpos = curpos;
        lastlen = strlen;
    }

    if (lasteolpos < str.length()) {
        str.replace(lasteolpos, str.length() - lasteolpos, trim(str.substr(lasteolpos, str.length() - lasteolpos)));
    }

    return str;
}

void prettyPrint(ScintillaDoc& doc, bool autoindenttext, bool addlinebreaks, bool indentattributes) {

    auto tabchar = doc.Tab();
    auto usetabs = doc.UseTabs();
    auto tabwidth = tabchar.length();

    auto eolchar = doc.EOL();
    size_t eolcharlen = eolchar.length();
    size_t eolcharpos = eolchar.find('\n');

    auto inText = doc.GetWorkText();
    if (inText.text == NULL)
        return;

    auto docclock_start = clock();
    // process

    std::string str("");
    str += inText.text;
    delete[] inText.text;


    // some state variables
    std::string tagname = "";
    bool in_tag = false;

    // some counters
    std::string::size_type curpos = 0, lastpos = 0, lastlen = 0, tmppos, xmllevel = 0, tagnamelen = 0;
    // some char value (pc = previous char, cc = current char, nc = next char, nnc = next next char)
    char cc;

    // first pass: trim lines
    str = trimxml(str, eolchar, true, indentattributes);

    size_t strlen = str.length();

    // second pass: indentation
    while (curpos < strlen && (curpos = str.find_first_of("<>\"'\n", curpos)) != std::string::npos) {
        switch (cc = str.at(curpos)) {
        case '<': {
            if (curpos < strlen - 2 && !str.compare(curpos, 2, "<?")) {                   // is "<?xml ...?>" definition ?
                // skip the comment
                curpos = str.find("?>", curpos + 1) + 1;
            }
            else if (curpos < strlen - 4 && !str.compare(curpos, 4, "<!--")) {            // is comment start ?
                // skip the comment
                curpos = str.find("-->", curpos + 1) + 2;
            }
            else if (curpos < strlen - 9 && !str.compare(curpos, 9, "<![CDATA[")) {       // is CDATA start ?
                // skip the CDATA
                curpos = str.find("]]>", curpos + 1) + 2;
            }
            else if (curpos < strlen - 2 && !str.compare(curpos, 2, "</")) {              // end tag (ex: "</sample>")
                curpos = str.find(">", curpos + 1);
                if (xmllevel > 0) --xmllevel;
            }
            else {                                                                              // beg tag
                in_tag = true;
                ++xmllevel;

                // skip the tag name
                tmppos = str.find_first_of("\t\n\v\f\r />", curpos + 1);
                if (tmppos != std::string::npos) {
                    // calculate tag name length
                    tagnamelen = tmppos - curpos - 1;
                    tagname.clear();
                    tagname = str.substr(curpos + 1, tagnamelen);

                    curpos = tmppos - 1;
                }
            }
            break;
        }
        case '>': {
            if (in_tag) {
                in_tag = false;
                if (xmltoolsoptions.ppAutoclose && !str.compare(curpos + 1, 3 + tagname.length(), "</" + tagname + ">")) {
                    // let's replace <a></a> with <a/>
                    str.insert(curpos++, "/");
                    str.erase(curpos + 1, 3 + tagname.length());
                }
                if (curpos > 0 && !str.compare(curpos - 1, 1, "/")) {                             // auto-closing tag (ex: "<sample/>")
                    --xmllevel;
                }
            }
            break;
        }
        case '\"':
        case '\'': {
            if (in_tag) {
                // skip attribute text
                curpos = str.find(cc, curpos + 1);
            }
            break;
        }
        case '\n': {
            // fix indentation
            curpos -= eolcharpos;   // line break may have several chars

            if (xmllevel > 0) {
                // we apply a delta when next tag is a closing tag
                long delta = 0;
                tmppos = curpos + eolcharlen;
                if (tmppos < strlen - 1 && str.at(tmppos) == '<' && str.at(tmppos + 1) == '/') {
                    delta = 1;
                }

                // apply indentation
                if (usetabs) {
                    str.insert(curpos + eolcharlen, (xmllevel - delta), '\t');
                }
                else {
                    str.insert(curpos + eolcharlen, tabwidth * (xmllevel - delta), ' ');
                }
            }

            curpos += eolcharlen;

            if (in_tag && indentattributes) {
                // add indentation for attribute
                str.insert(curpos, tagnamelen, ' ');
            }

            --curpos;

            break;
        }
        }

        ++curpos;

        // inifinite loop protection
        strlen = str.length();
        if (curpos == lastpos && lastlen == strlen) {
            dbgln("PRETTYPRINT: INFINITE LOOP DETECTED", DBG_LEVEL::DBG_ERROR);
            break;
        }
        lastpos = curpos;
        lastlen = strlen;
    }

    // done 
    auto docclock_end = clock();

    std::string txt;
    txt += "crunching => time taken: " + std::to_string(docclock_end - docclock_start) + " ms";
    dbgln(txt.c_str());

    doc.SetWorkText(str.c_str());
    str.clear();
}


void nppPrettyPrintXMLInline(ScintillaDoc& doc) {
    prettyPrint(doc, false, true, false);
}

void nppPrettyPrintAttributesInline(ScintillaDoc& doc) {
    prettyPrint(doc, false, true, true);
}

void nppPrettyPrintXML() {
    dbgln("prettyPrintXML()");
    nppDocumentCommand("nppPrettyPrintXMLInline", nppPrettyPrintXMLInline);
}

void nppPrettyPrintAttributes() {
    dbgln("prettyPrintAttributes()");
    nppDocumentCommand("nppPrettyPrintAttributesInline", nppPrettyPrintAttributesInline);
}

///////////////////////////////////////////////////////////////////////////////


void sciDocLinearizeXMLInline(ScintillaDoc& doc) {
    auto inText = doc.GetWorkText();
    if (inText.text == NULL)
        return;

    std::string eolchar = doc.EOL();

    auto docclock_start = clock();
    // process
    std::string str(inText.text);
    delete[] inText.text;
    inText.text = NULL;

    std::string::size_type curpos = 0, nexwchar_t;
    bool enableInsert = false;

    while ((curpos = str.find_first_of(eolchar, curpos)) != std::string::npos) {
        nexwchar_t = str.find_first_not_of(eolchar, curpos);
        str.erase(curpos, nexwchar_t - curpos);

        // Let erase leading space chars on line
        if (curpos != std::string::npos && curpos < str.length()) {
            nexwchar_t = str.find_first_not_of(" \t", curpos);
            if (nexwchar_t != std::string::npos && nexwchar_t >= curpos) {
                // And if the 1st char of next line is not '<' and last char of preceding
                // line is not '>', then we consider we are in text content, then let put
                // a space char
                enableInsert = false;
                if (curpos > 0 && str.at(nexwchar_t) != '<' && str.at(curpos - 1) != '>') {
                    enableInsert = true;
                    if (nexwchar_t > curpos) --nexwchar_t;
                }

                if (nexwchar_t > curpos) str.erase(curpos, nexwchar_t - curpos);
                else if (enableInsert) str.insert(nexwchar_t, " ");
            }
        }
    }

    // done 
    auto docclock_end = clock();

    std::string txt;
    txt += "crunching => time taken: " + std::to_string(docclock_end - docclock_start) + " ms";
    dbgln(txt.c_str());

    // Send formatted string to scintilla
    doc.SetWorkText(str.c_str());
    str.clear();
}

void nppLinearizeXML() {
    nppDocumentCommand("linearizeXMLInline", sciDocLinearizeXMLInline);
}
