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

    struct Lexeme {
        Token token;
        InlineString value;

        Lexeme(Token type, const char* text, const char* end) :value(text, end - text) {
            this->token = type;
        }

        Lexeme() {
            token = Token::None;
        }

        void clear() {
            value.clear();
            token = Token::None;
        }

        std::string string() { return value.string(); }

        bool operator ==(const char* right) const {
            return this->value == right;
        }

        bool operator ==(const Lexeme& right) const {
            
            if (token != right.token)
                return false;

            return value == right.value;
        }

        bool operator ==(const Token right) const {

            return token == right;
        }

        bool operator !=(const Token right) const {

            return token != right;
        }
    };

    class Lexer {
        const char* xml;
        const char* __curpos;
        const char* __endpos;

        Lexeme _lexeme;

        bool _isInTag; // different interpretation of TEXT and non-TEXT

        const char *CDStart = "<![CDATA[";
        const char *CDEnd = "]]>";

        void handleTagStart();
        void handleInTag();
        void handleOutsideTag();

        bool readUntil(int start, const char* end, bool includeEnd);

        void findNext();

        size_t pos_row = 0;
        size_t pos_col = 0;

    public:
        Lexer(const char* start, size_t len);
        Lexer(const char* start) :Lexer(start, strlen(start)) {}
        void reset();

        bool registerLinebreaks = true;
        bool trackPosition = false;

        size_t currentLine() { return pos_row; }
        size_t currentColumn() { return pos_col; }
        size_t currentOffset() { return __curpos - xml; }

        Lexeme readUntil(const char* end, bool includeEnd) {
            readUntil(0, end, includeEnd);
            return _lexeme;
        }

        Lexeme tryReadWhitespace() {
            if (isWhitespace(*__curpos) || (!registerLinebreaks && isLinebreak(*__curpos))) {
                auto pos = __curpos+1;
                while (pos < __endpos && (isWhitespace(*pos) || (!registerLinebreaks && isLinebreak(*pos)))) pos++;
                return _lexeme = Lexeme(Token::Whitespace, __curpos, pos);
            }
            return Lexeme(Token::Unknown, NULL, NULL);
        }

        Lexeme tryReadName();

        // Get text of last Lexeme peek-ed
        //[[deprecated("instead of peek_token, use peek to get the whole Lexeme. Or, use currentLexeme")]]
        const char* Lexer::TokenText() { return _lexeme.value.text(); }
        // Get size of last lexeme peek-ed
        //[[deprecated("instead of peek_token, use peek to get the whole Lexeme. Or, use currentLexeme")]]
        size_t Lexer::TokenSize() { return _lexeme.value.size();}

        const Lexeme currentLexeme() const { return _lexeme; }

        bool Done() { return __curpos >= __endpos; }

        inline bool inTag() { return _isInTag; }
        void cancelInTag() { _isInTag = false; }

        [[deprecated("Use isWhitespace() instead")]]
        inline static bool IsWhitespace(char x) {
            return ((x) == 0x20 || (x) == 0x9);
        }
        inline static bool isWhitespace(char x) {
            return ((x) == 0x20 || (x) == 0x9);
        }

        inline static bool isLinebreak(char x) {
            return ((x) == 0xD || (x) == 0x0A);
        }
        [[deprecated("Use isLinebreak() instead")]]
        inline static bool IsLinebreak(char x) {
            return ((x) == 0xD || (x) == 0x0A);
        }
        [[deprecated("Use isIdentifier() instead")]]
        inline static bool IsIdentifier(Token token) {
            return token == Token::Name || token == Token::Nmtoken;
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


        Lexeme get() {
            findNext();
            eatToken();
            return _lexeme;
        }
        Lexeme peek() {
            findNext();
            return _lexeme;
        }

        [[deprecated("please use peekToken()")]]
        Token peek_token() {
            findNext();
            return _lexeme.token;
        }
        Token peekToken() {
            findNext();
            return _lexeme.token;
        }

        char peekChar() {
            return *__curpos;
        }
        char readChar() {
            if (__curpos < __endpos) {
                auto c = *__curpos;
                if (trackPosition) {
                    if (c == '\n') {
                        pos_row++;
                        pos_col = 0;
                    }
                    else {
                        pos_col++;
                    }
                }
                __curpos++;
                return c;
            }
            return 0;
        }
        
        [[deprecated("Lexer keeps track of in/out tag, there is no need for this function. Use Lexer::peek() or Lexer::get()")]]
        Token TryGetAttribute();

        [[deprecated("please use eatToken")]]
        void EatToken();
        void eatToken();

    };
}
