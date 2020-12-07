#pragma once

#include <string>

namespace StringXml {
    struct XmlFormaterParamsType {
        std::string indentChars = "\t";		// indentation char(s)
        std::string eolChars = "\n";		// end of line char(s)
        bool autoCloseTags = false;			// make the formater change tags like <a></a> into <a/>
    };

    class XmlFormater {
        XmlFormaterParamsType params;

        size_t indentLevel;                 // the real applied indent level
        size_t levelCounter;                // the level counter

        std::string* str;        // pointer to original source text
    public:
        XmlFormater(std::string *str);
        XmlFormater(std::string *str, XmlFormaterParamsType params);
        ~XmlFormater();

        void init(std::string *str);
        void init(std::string *str, XmlFormaterParamsType params);
        void reset();

        void linearize();
        void prettyPrint(bool autoindenttext, bool addlinebreaks, bool indentattributes, bool indentonly = false);

        void prettyPrint();
        void prettyPrintAttr();
        void prettyPrintIndent();

        XmlFormaterParamsType getDefaultParams();
    };
}