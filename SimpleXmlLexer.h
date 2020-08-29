#include <cstddef>
#include <string.h>

enum class Token {
    InputEnd,
    Whitespace,
    Linebreak,
    ClosingTag,
    ProcessingInstructionStart,
    TagStart,
    TagEnd,
    Name,
    SelfClosingTagEnd,
    Text,
    Comment,
    CData,
    None
};

class SimpleXmlLexer {
    const char* xml;
    const char* curpos;
    const char* endpos;

    const char* tokenStart = NULL;
    const char* tokenEnd = NULL;

public:
    const char* SimpleXmlLexer::TokenText() { return tokenStart; }
    size_t SimpleXmlLexer::TokenSize() { return tokenEnd - tokenStart; }

    SimpleXmlLexer(const char* start, size_t len);

    bool Done() { return curpos >= endpos; }

    inline static bool IsWhitespace(char x) {
        return ((x) == 0x20 || (x) == 0x9);
    }
    inline static bool IsLinebreak(char x) {
        return ((x) == 0xD || (x) == 0x0A);
    }

    Token FindNext();
    Token TryGetName();
    Token TryGetAttribute();
    inline void EatToken() { curpos = tokenEnd; }
};