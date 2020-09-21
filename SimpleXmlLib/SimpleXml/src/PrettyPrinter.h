#ifndef PRETTYPRINTER_HEADER_FILE_H
#define PRETTYPRINTER_HEADER_FILE_H


#include <sstream>
#include "Lexer.h"

namespace SimpleXml {

    struct PrettyPrintParms
    {
        // End of Line character
        std::string eol;

        // indent character
        std::string tab;

        // safeguard against incorrectly closed elements
        int maxElementDepth = 255;

        // add indents on new lines
        bool insertIndents = false;

        // insert new lines after each element tag
        bool insertNewLines = false;

        // remove whitespace from CDATA
        bool removeWhitespace = false;

        // do not trim CDATA
        bool keepStartEndWhitespace = false;
        
        // convert <tag></tag> into <tag/>
        bool autocloseEmptyElements = true;

        // do not remove existing newlines
        bool keepExistingBreaks = false;

        // when indenting, put each attribute on a separate line + indent
        bool indentAttributes = false;
    };

    class PrettyPrinter {
        std::stringstream outText;
        SimpleXml::Lexer lexer;
        PrettyPrintParms parms;

        bool inTag = false;
        int indentlevel = 0;
        bool indented = false;
        bool tagIsOpen = false;

        std::string prevTag;

        inline void WriteCloseTag();
        inline void TryCloseTag();
        inline void Indent();
        inline void TryIndent();
        inline void AddNewline();
        inline void StartNewElement();
        inline void WriteToken();
        inline void WriteEatToken();
        void Parse();
        bool ParseAttributes();

        void trimTextAndOutput(std::string_view is);
    public:
        PrettyPrinter(SimpleXml::ChunkedStream& s, PrettyPrintParms parms) :lexer(s), parms(parms) {
            lexer.parms.registerLinebreaks = parms.keepExistingBreaks;
        }

        std::stringstream* Stream() { return &outText; }

        bool Convert();
    };
}

#endif