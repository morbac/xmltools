#include "StringXml.h"
#include "../../../Report.h"

namespace StringXml {
    static inline std::string& ltrim(std::string& str, const std::string& chars = "\t\n\v\f\r ") {
        str.erase(0, str.find_first_not_of(chars));
        return str;
    }

    static inline std::string& rtrim(std::string& str, const std::string& chars = "\t\n\v\f\r ") {
        str.erase(str.find_last_not_of(chars) + 1);
        return str;
    }

    static inline std::string& trim(std::string& str, const std::string& chars = "\t\n\v\f\r ") {
        return ltrim(rtrim(str, chars), chars);
    }

    XmlFormater::XmlFormater(std::string *str) {
        this->init(str, this->getDefaultParams());
    }

    XmlFormater::XmlFormater(std::string *str, XmlFormaterParamsType params) {
        this->init(str, params);
    }

    XmlFormater::~XmlFormater() {
        this->str = NULL;
    }

    void XmlFormater::init(std::string *str) {
        this->init(str, this->getDefaultParams());
    }

    void XmlFormater::init(std::string *str, XmlFormaterParamsType params) {
        this->str = str;
        this->params = params;
        this->reset();
    }

    void XmlFormater::reset() {
        this->indentLevel = 0;
        this->levelCounter = 0;
    }

    void XmlFormater::linearize() {
        std::string::size_type curpos = 0, nexwchar_t;
        bool enableInsert = false;

        while ((curpos = this->str->find_first_of(this->params.eolChars, curpos)) != std::string::npos) {
            nexwchar_t = this->str->find_first_not_of(this->params.eolChars, curpos);
            this->str->erase(curpos, nexwchar_t - curpos);

            // Let erase leading space chars on line
            if (curpos != std::string::npos && curpos < this->str->length()) {
                nexwchar_t = this->str->find_first_not_of(" \t", curpos);
                if (nexwchar_t != std::string::npos && nexwchar_t >= curpos) {
                    // And if the 1st char of next line is not '<' and last char of preceding
                    // line is not '>', then we consider we are in text content, then let put
                    // a space char
                    enableInsert = false;
                    if (curpos > 0 && this->str->at(nexwchar_t) != '<' && this->str->at(curpos - 1) != '>') {
                        enableInsert = true;
                        if (nexwchar_t > curpos) --nexwchar_t;
                    }

                    if (nexwchar_t > curpos) this->str->erase(curpos, nexwchar_t - curpos);
                    else if (enableInsert) this->str->insert(nexwchar_t, " ");
                }
            }
        }
    }

    void trimxml(std::string* str, std::string eolchar, bool breaklines, bool breaktags, bool indentOnly = false, const std::string& chars = "\t\n\v\f\r ") {
        bool in_tag = false, in_header = false;
        std::string tagname = "";
        char cc;
        std::string::size_type curpos = 0, lastpos = 0, lastlen = 0, lasteolpos = 0, tmppos;
        size_t eolcharlen = eolchar.length();
        size_t eolcharpos = eolchar.find('\n');

        size_t strlen = str->length();

        while (curpos < strlen && (curpos = str->find_first_of("<>\"'\n", curpos)) != std::string::npos) {
            switch (cc = str->at(curpos)) {
            case '<': {
                if (curpos < strlen - 4 && !str->compare(curpos, 4, "<!--")) {            // is comment start ?
                  // skip the comment
                    curpos = str->find("-->", curpos + 1) + 2;

                    // add line break if next non space char is "<"
                    if (breaklines) {
                        tmppos = str->find_first_not_of(chars, curpos + 1);
                        if (!indentOnly && tmppos != std::string::npos && str->at(tmppos) == '<' /*&& str->at(tmppos + 1) != '!'*/ && str->at(tmppos + 2) != '[') {
                            str->insert(curpos + 1, eolchar);
                        }
                    }
                }
                else if (curpos < strlen - 9 && !str->compare(curpos, 9, "<![CDATA[")) {       // is CDATA start ?
                  // skip the CDATA
                    curpos = str->find("]]>", curpos + 1) + 2;
                }
                else if (curpos < strlen - 2 && !str->compare(curpos, 2, "</")) {              // end tag (ex: "</sample>")
                    curpos = str->find(">", curpos + 1);

                    // trim space chars between tagname and > char
                    tmppos = str->find_last_not_of(chars, curpos - 1);
                    if (tmppos < curpos - 1) {
                        str->erase(tmppos + 1, curpos - tmppos - 1);
                        curpos = tmppos + 1;
                    }

                    // add line break if next non space char is "<" (but not if <![)
                    if (breaklines) {
                        tmppos = str->find_first_not_of(chars, curpos + 1);
                        if (!indentOnly && tmppos != std::string::npos && str->at(tmppos) == '<' /*&& str->at(tmppos + 1) != '!'*/ && str->at(tmppos + 2) != '[') {
                            str->insert(curpos + 1, eolchar);
                        }
                    }
                }
                else {
                    in_tag = true;
                    if (curpos < strlen - 2 && !str->compare(curpos, 2, "<?")) {
                        in_header = true;
                        ++curpos;
                    }

                    // skip the tag name
                    char endtag = (in_header ? '?' : '/');
                    tmppos = curpos;
                    curpos = str->find_first_of("\t\n\v\f\r ?/>", curpos + 1);
                    if (curpos != std::string::npos) {
                        tagname.clear();
                        tagname = str->substr(tmppos + 1, curpos - tmppos - 1);

                        tmppos = str->find_first_not_of("\t\n\v\f\r ", curpos);
                        if (tmppos != std::string::npos) {
                            // trim space before attribute or ">" char
                            str->erase(curpos, tmppos - curpos);
                            if (str->at(curpos) != '>' && str->at(curpos) != endtag) {
                                str->insert(curpos, " ");
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
                        bool is_closing = (curpos > 0 && str->at(curpos - 1) == '/');
                        tmppos = str->find_first_not_of(chars, curpos + 1);
                        if (!indentOnly && tmppos != std::string::npos && str->at(tmppos) == '<' && (str->at(tmppos + 1) != '/' || is_closing) && /*str->at(tmppos + 1) != '!' &&*/ str->at(tmppos + 2) != '[') {
                            str->insert(curpos + 1, eolchar);
                        }
                    }
                }
                break;
            }
            case '\"':
            case '\'': {
                if (in_tag) {
                    // trim spaces arround "=" char
                    tmppos = str->find_last_not_of("\t\n\v\f\r ", curpos - 1);
                    if (tmppos != std::string::npos && tmppos < curpos && str->at(tmppos) == '=') {
                        // remove spaces after "="
                        str->erase(tmppos + 1, curpos - tmppos - 1);
                        curpos = tmppos + 1;
                        // remove spaces before "="
                        tmppos = str->find_last_not_of("\t\n\v\f\r ", tmppos - 1);
                        if (tmppos != std::string::npos) {
                            str->erase(tmppos + 1, curpos - tmppos - 2);
                            curpos = tmppos + 2;
                        }
                    }
                    // skip attribute text
                    curpos = str->find(cc, curpos + 1);

                    // trim spaces after attribute
                    tmppos = str->find_first_not_of("\t\n\v\f\r ", curpos + 1);
                    if (tmppos != std::string::npos) {
                        char endtag = '/';
                        if (in_header) endtag = '?';

                        // add line break if not the last attribute
                        if (!indentOnly && breaktags && !in_header && str->at(tmppos) != '>' && str->at(tmppos) != endtag) {
                            str->insert(curpos + 1, eolchar);
                        }
                        else if (!breaktags) {
                            str->erase(curpos + 1, tmppos - curpos - 1);
                            if (str->at(curpos + 1) != '>' && str->at(curpos + 1) != endtag) {
                                str->insert(curpos + 1, " ");
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
                    tmppos = str->find_first_not_of("\t\n\v\f\r ", curpos + 1);
                    if (tmppos != std::string::npos) {
                        str->erase(curpos, tmppos - curpos);
                    }
                    if (str->at(curpos - 1) == '"' || str->at(curpos - 1) == '\'') {
                        str->insert(curpos, " ");
                        ++curpos;
                    }
                }
                else {  // = if (!in_tag || breaktags)
                    std::string tmp = str->substr(lasteolpos, curpos - lasteolpos);
                    tmp = trim(tmp);
                    str->replace(lasteolpos, curpos - lasteolpos, tmp);
                    curpos = lasteolpos + tmp.length();
                    lasteolpos = curpos;

                    if (!indentOnly) {
                        while (lasteolpos >= eolcharlen && !str->compare(lasteolpos - eolcharlen, eolcharlen, eolchar)) {
                            lasteolpos -= eolcharlen;
                        }
                    }

                    lasteolpos += eolcharlen;
                }

                curpos += (eolcharlen - 1);

                break;
            }
            }

            ++curpos;

            // inifinite loop protection
            strlen = str->length();
            if (curpos == lastpos && lastlen == strlen) {
                //dbgln("TRIM: INIFINITE LOOP DETECTED");
                break;
            }
            lastpos = curpos;
            lastlen = strlen;
        }

        if (lasteolpos < str->length()) {
            std::string tmp = str->substr(lasteolpos, str->length() - lasteolpos);
            str->replace(lasteolpos, str->length() - lasteolpos, trim(tmp));
        }
    }

    void XmlFormater::prettyPrint(bool autoindenttext, bool addlinebreaks, bool indentattributes, bool indentonly) {
        // some state variables
        std::string tagname = "";
        bool in_tag = false;

        // some counters
        std::string::size_type curpos = 0, lastpos = 0, lastlen = 0, tmppos, xmllevel = 0, tagnamelen = 0;
        // some char value (pc = previous char, cc = current char, nc = next char, nnc = next next char)
        char cc = '\0';

        size_t tabwidth = this->params.indentChars.size();
        bool usetabs = this->params.indentChars.find("\t") != std::string::npos;
        std::string eolchar = this->params.eolChars;
        size_t eolcharlen = eolchar.length();
        size_t eolcharpos = eolchar.find('\n');

        // first pass: trim lines
        trimxml(this->str, eolchar, true, indentattributes, indentonly);

        size_t strlen = this->str->size();

        // second pass: indentation
        while (curpos < strlen && (curpos = this->str->find_first_of("<>\"'\n", curpos)) != std::string::npos) {
            switch (cc = this->str->at(curpos)) {
            case '<': {
                if (curpos < strlen - 2 && !this->str->compare(curpos, 2, "<?")) {                   // is "<?xml ...?>" definition ?
                    // skip the comment
                    curpos = this->str->find("?>", curpos + 1) + 1;
                }
                else if (curpos < strlen - 4 && !this->str->compare(curpos, 4, "<!--")) {            // is comment start ?
                    // skip the comment
                    curpos = this->str->find("-->", curpos + 1) + 2;
                }
                else if (curpos < strlen - 9 && !this->str->compare(curpos, 9, "<![CDATA[")) {       // is CDATA start ?
                    // skip the CDATA
                    curpos = this->str->find("]]>", curpos + 1) + 2;
                }
                else if (curpos < strlen - 2 && !this->str->compare(curpos, 2, "</")) {              // end tag (ex: "</sample>")
                    curpos = this->str->find(">", curpos + 1);
                    if (xmllevel > 0) --xmllevel;
                }
                else {                                                                              // beg tag
                    in_tag = true;
                    ++xmllevel;

                    // skip the tag name
                    tmppos = this->str->find_first_of("\t\n\v\f\r />", curpos + 1);
                    if (tmppos != std::string::npos) {
                        // calculate tag name length
                        tagnamelen = tmppos - curpos - 1;
                        tagname.clear();
                        tagname = this->str->substr(curpos + 1, tagnamelen);

                        curpos = tmppos - 1;
                    }
                }
                break;
            }
            case '>': {
                if (in_tag) {
                    in_tag = false;
                    if (this->params.autoCloseTags && !this->str->compare(curpos + 1, 3 + tagname.length(), "</" + tagname + ">")) {
                        // let's replace <a></a> with <a/>
                        this->str->insert(curpos++, "/");
                        this->str->erase(curpos + 1, 3 + tagname.length());
                    }
                    if (curpos > 0 && !this->str->compare(curpos - 1, 1, "/")) {                             // auto-closing tag (ex: "<sample/>")
                        --xmllevel;
                    }
                }
                break;
            }
            case '\"':
            case '\'': {
                if (in_tag) {
                    // skip attribute text
                    curpos = this->str->find(cc, curpos + 1);
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
                    if (tmppos < strlen - 1 && this->str->at(tmppos) == '<' && this->str->at(tmppos + 1) == '/') {
                        delta = 1;
                    }

                    // apply indentation
                    if (usetabs) {
                        this->str->insert(curpos + eolcharlen, (xmllevel - delta), '\t');
                    }
                    else {
                        this->str->insert(curpos + eolcharlen, tabwidth * (xmllevel - delta), ' ');
                    }
                }

                curpos += eolcharlen;

                if (in_tag && indentattributes) {
                    // add indentation for attribute
                    this->str->insert(curpos, tagnamelen, ' ');
                }

                --curpos;

                break;
            }
            }

            ++curpos;

            // inifinite loop protection
            strlen = this->str->length();
            if (curpos == lastpos && lastlen == strlen) {
                //dbgln("PRETTYPRINT: INIFINITE LOOP DETECTED");
                break;
            }
            lastpos = curpos;
            lastlen = strlen;
        }
    }

    void XmlFormater::prettyPrint() {
        this->prettyPrint(false, true, false);
    }

    void XmlFormater::prettyPrintAttr() {
        this->prettyPrint(false, true, true);
    }

    void XmlFormater::prettyPrintIndent() {
        this->prettyPrint(false, true, false, true);
    }

    XmlFormaterParamsType XmlFormater::getDefaultParams() {
        XmlFormaterParamsType params;
        params.indentChars = "  ";
        params.eolChars = "\n";
        params.autoCloseTags = false;
        return params;
    }
}