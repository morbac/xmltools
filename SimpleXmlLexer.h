#include <cstddef>
#include <string.h>

#include "SimpleXmlInlineString.h"
namespace SimpleXml {
    enum class Token {
        InputEnd,
        Whitespace,
        Linebreak,
        ProcessingInstructionStart,
        ProcessingInstructionEnd,
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

    struct Lexeme:InlineString {
        Token token;

        Lexeme(Token type, const char* text, const char *end):InlineString(text,end - text) {
            this->token = type;
        }

        Lexeme() {
            token = Token::None;
        }

        bool operator ==(const Lexeme& right) const {
            
            if (token != right.token)
                return false;

            return (InlineString)*this == (InlineString)right;
        }

    };

    class Lexer {
        const char* xml;
        const char* __curpos;
        const char* __endpos;

        Lexeme lexeme;

        bool isInTag; // different interpretation of TEXT and non-TEXT

        const char *CDStart = "<![CDATA[";
        const char *CDEnd = "]]>";

        void handleTagStart();
        void handleInTag();
        void handleOutsideTag();

        bool readUntil(int start, const char* end);

        void findNext();

    public:
        Lexer(const char* start, size_t len);
        Lexer(const char* start) :Lexer(start, strlen(start)) {}

        bool RegisterLinebreaks = true;

        bool readUntil(const char* end) {
            lexeme.token = Token::Unknown;
            return readUntil(0,end);
        }

        // Get text of last Lexeme peek-ed
        const char* Lexer::TokenText() { return lexeme.text(); }

        // Get size of last lexeme peek-ed
        size_t Lexer::TokenSize() { return lexeme.size();}

        bool Done() { return __curpos >= __endpos; }

        inline bool InTag() { return isInTag; }
        void CancelTag() { isInTag = false; }

        inline static bool IsWhitespace(char x) {
            return ((x) == 0x20 || (x) == 0x9);
        }
        inline static bool IsLinebreak(char x) {
            return ((x) == 0xD || (x) == 0x0A);
        }

        inline static bool IsIdentifier(Token token) {
            return token == Token::Name || token == Token::Nmtoken;
        }

        Lexeme get() {
            findNext();
            EatToken();
            return lexeme;
        }

        Lexeme peek() {
            findNext();
            return lexeme;
        }

        Token peek_token() {
            findNext();
            return lexeme.token;
        }

        inline bool IsTagStart(Token token) {
            return token == Token::TagStart || token == Token::ClosingTag || token == Token::ProcessingInstructionStart;
        }

        inline bool IsTagEnd(Token token) {
            return token == Token::TagEnd 
                || token == Token::SelfClosingTagEnd
                || token == Token::ProcessingInstructionEnd;
        }

    public:
        Token TryGetAttribute();
        inline void EatToken() { 
            if (lexeme.text() != NULL) {
                __curpos = lexeme.end();

                if (IsTagStart(lexeme.token))
                    isInTag = true;

                else if (IsTagEnd(lexeme.token))
                    isInTag = false;
            }
        }
    };
}
