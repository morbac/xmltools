#pragma once

#include <sstream>

namespace StringXml {
    struct XmlFormaterParamsType {
        std::string indentChars = "\t";		// indentation char(s)
        std::string eolChars = "\n";		// end of line char(s)
        size_t maxIndentLevel = 255;		// max indent level (0 == unlimited)
        bool ensureConformity = true;		// make the formater respect conformity
        bool autoCloseTags = false;			// make the formater change tags like <a></a> into <a/>
        bool indentAttributes = false;		// make the formater display attributes on separated lines
        bool indentOnly = false;			// make the formater keep the existing linebreaks and only adjust indentation
    };

    class XmlFormater {
        XmlFormaterParamsType params;

        size_t indentLevel;                 // the real applied indent level
        size_t levelCounter;                // the level counter

        const char* srcText;        // pointer to original source text
        size_t srcLength;           // the original source text length
    public:
        XmlFormater(const char* data, size_t length);
        XmlFormater(const char* data, size_t length, XmlFormaterParamsType params);
        ~XmlFormater();

        void init(const char* data, size_t length);
        void init(const char* data, size_t length, XmlFormaterParamsType params);
        void reset();

        std::string linearize();
        std::string prettyPrint(bool autoindenttext, bool addlinebreaks, bool indentattributes, bool indentonly = false);

        XmlFormaterParamsType getDefaultParams();
    };
}