#include <cstddef>
#include <string.h>

namespace SimpleXml {
    enum class Token {
        InputEnd,
        Whitespace,
        Linebreak,
        ProcessingInstructionStart,
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

    struct Lexeme {
        Token token;
        const char* text;
        const char* end;

        Lexeme(Token type, const char* text, const char *textend) {
            this->token = type;
            this->text = text;
            this->end = textend;
        }
        Lexeme(Token type, const char* text) { // used for testing
            this->token = type;
            this->text = text;
            this->end = text + strlen(text);
        }
        Lexeme() {
            token = Token::None;
            text = end = NULL;
        }

        inline size_t size() {
            return end - text;
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

    public:
        const char* Lexer::TokenText() { return lexeme.text; }
        size_t Lexer::TokenSize() { return lexeme.size();}

        Lexer(const char* start, size_t len);
        Lexer(const char* start) :Lexer(start, strlen(start)) {}

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
            lexeme.token = FindNext();
            EatToken();
            return lexeme;
        }

        Lexeme peek() {
            auto token = FindNext();

            lexeme.token = token;

            return lexeme;
        }

        Token peek_token() {
            lexeme.token = FindNext();
            return lexeme.token;
        }

        inline bool IsTagStart(Token token) {
            return token == Token::TagStart || token == Token::ClosingTag || token == Token::ProcessingInstructionStart;
        }

        inline bool IsTagEnd(Token token) {
            return token == Token::TagEnd || token == Token::SelfClosingTagEnd;
        }

    private:
        Token FindNext();
    public:
        Token TryGetName();
        Token TryGetAttribute();
        inline void EatToken() { 
            __curpos = lexeme.end;

            if (IsTagStart(lexeme.token))
                isInTag = true;

            else if (IsTagEnd(lexeme.token))
                isInTag = false;
        }
    };
}
