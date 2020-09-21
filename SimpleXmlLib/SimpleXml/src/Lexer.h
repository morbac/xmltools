#ifndef LEXER_HEADER_FILE_H
#define LEXER_HEADER_FILE_H


#include <cstddef>
#include <string.h>

#include "ChunkedStream.h"

namespace SimpleXml {
    enum class Token {
        InputEnd,
        Whitespace,
        Linebreak,
        ProcessingInstructionStart,
        ProcessingInstructionEnd,
        DeclStart,
        TagStart,
        TagEnd,
        SelfClosingTagEnd,
        ClosingTag,
        Name,
        Nmtoken,
        Text,
        Comment,
        CDSect,
        Eq,
        SystemLiteral,
        Unknown,
        None
    };

    class Lexer {
        typedef std::string_view string_view;
        typedef std::string string;

        Token currentToken;
        size_t currentTokenSize;
        
        bool _isInTag; // different interpretation of TEXT and non-TEXT

        const char *CDStart = "<![CDATA[";
        const char *CDEnd = "]]>";

        void handleTagStart();
        void handleInTag();
        void handleOutsideTag();

        bool readUntil(int start, const char* end, bool includeEnd);

        void findNext();

        size_t pos_row = 1;
        size_t pos_col = 1;

        void trackPosition(const char *data, size_t len);
        void trackPosition(size_t len);

        ChunkedStream* inputStream;
    public:

        Lexer(ChunkedStream &inputStream);
        void reset();

        struct Parms {
            bool registerLinebreaks = false;
            bool trackPosition = false;
        }parms;

        size_t currentLine() { return pos_row; }
        size_t currentColumn() { return pos_col; }

        bool readUntil(const char* end, bool includeEnd) {
            return readUntil(0, end, includeEnd);
        }

        // can be used for error recovery. 
        bool readUntilTagEndOrStart();

        bool tryReadWhitespace();
        bool tryReadName();
        bool tryReadNmtoken();

        bool Done() { return inputStream->eod(); }

        inline bool inTag() { return _isInTag; }
        void cancelInTag() { _isInTag = false; }

        inline static bool isWhitespace(char x) {
            return ((x) == 0x20 || (x) == 0x9);
        }

        inline static bool isLinebreak(char x) {
            return ((x) == 0xD || (x) == 0x0A);
        }

        inline static bool isIdentifier(Token token) {
            return token == Token::Name || token == Token::Nmtoken;
        }
        inline bool isTagStart(Token token) {
            return token == Token::TagStart || token == Token::ClosingTag || token == Token::ProcessingInstructionStart || token == Token::DeclStart;
        }
        inline bool isTagEnd(Token token) {
            return token == Token::TagEnd
                || token == Token::SelfClosingTagEnd
                || token == Token::ProcessingInstructionEnd;
        }

        Token peek() {
            findNext();
            return currentToken;
        }

        Token peekToken() {
            findNext();
            return currentToken;
        }

        bool peekMatch(const char* match) { return inputStream->peekMatch(match, strlen(match)); }

        // checks to see if the data is in a single chunk or not.
        bool isBroken() {
            return currentTokenSize > inputStream->available();
        }
        //make sure isBroken returns false before using this method
        inline const char* tokenStart() { return inputStream->begin(); }
        //make sure isBroken returns false before using this method
        string_view tokenData() { return string_view(inputStream->begin(), currentTokenSize); }

        size_t tokenSize() { return currentTokenSize; }
        inline char peekChar() {
            return inputStream->peekChar(0);
        }

        char readChar() {
            char c = inputStream->peekChar(0);
            if (inputStream->eod())
                return c;

            currentTokenSize = 0;
            inputStream->skip(1);
            
            if (parms.trackPosition) {
                if (c == '\n') {
                    pos_row++;
                    pos_col =1;
                }
                else {
                    pos_col++;
                }
            }
            return c;
        }

        void eatToken();

        void readTokenData(char* target);
        void readTokenData(string& s) {
            s.resize(currentTokenSize);
            readTokenData(&s[0]);
        }
        void readSystemLiteral(std::string& s);

        void writeTokenData(std::ostream& out);

    };
}

#endif