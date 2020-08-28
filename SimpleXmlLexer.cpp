#include <cstddef>
#include <string.h>

#include "SimpleXmlLexer.h"


#define isWhitespace(x) ((x) == 0x20 || \
                          (x) == 0x9 || \
                          (x) == 0xD || \
                          (x) == 0x0A \
                    ) // XML Standard paragraph 2.3 whitespace

#define isTagStart(x) ((x=='<'))
#define isTagEnd(x) ((x=='>'))
//#define isValueStart(x) ((x=='"'))
//#define isValueEnd(x) ((x=='"'))
//#define isKeyValueSeparator(x) ((x=='='))

#define isNameStartChar(x) (isNameStartCharSimple(x) || isNameStartCharExtended(x))

#define isNameStartCharSimple(x) (\
    ((x) >= 'A' && (x) <= 'Z') || \
    ((x) >= 'a' && (x) <= 'z') || \
    (x) == '_' || \
    (x) == ':'\
)

#define isNameStartCharExtended(x) ( \
    ((x) >= 0xC0   && (x) <= 0xD6) || \
    ((x) >= 0xD8   && (x) <= 0xF6) || \
    ((x) >= 0x370  && (x) <= 0x37D) || \
    ((x) >= 0x37F  && (x) <= 0x1FFF) || \
    ((x) >= 0x200C && (x) <= 0x200D) || \
    ((x) >= 0x2070 && (x) <= 0x218F) || \
    ((x) >= 0x2C00 && (x) <= 0x2FEF) || \
    ((x) >= 0x3001 && (x) <= 0xD7FF) || \
    ((x) >= 0xF900 && (x) <= 0xFDCF) || \
    ((x) >= 0xFDF0 && (x) <= 0xFFFD) || \
    ((x) >= 0x10000 && (x)<= 0xEFFFF)  \
)

#define isNameChar(x) ( \
    isNameStartCharSimple(x) || \
    (x) == '-' || \
    (x) == '.' || \
    ((x)>='0' && (x)<='9') || \
    (x) == 0xB7 || \
    ((x) >= 0x0300 && (x) <= 0x036F) || \
    ((x) >= 0x203F && (x) <= 0x2040) || \
    isNameStartCharExtended(x) \
)

SimpleXmlLexer::SimpleXmlLexer(const char* start, size_t len) {
    xml = start;
    curpos = xml;
    endpos = xml + len;
}


Token SimpleXmlLexer::FindNext() {
    if (curpos == endpos)
        return Token::InputEnd;

    tokenStart = curpos;

    if (isTagStart(*curpos)) {
        // check for comment, CDATA, 
        if (curpos[1] == '!') {
            if (curpos[2] == '-' && curpos[3] == '-') { // <!--
                tokenEnd = strstr(curpos + 4, "-->");
                return Token::Comment;
            }

            if (0 == strncmp(curpos, "<![CDATA[", 9)) {
                tokenEnd = strstr(curpos + 9, "]]>");
                if (tokenEnd == NULL)
                    tokenEnd = endpos; // not found.. copy rest

                return Token::CData;
            }

            // resolve unknown ! tag.. just accept that it is a regular tag... and hope for the best
        }

        if (curpos[1] == '/') { // closing 
            tokenEnd = curpos + 2;
            return Token::ClosingTag;
        }
        if (curpos[1] == '?') {
            tokenEnd = curpos + 2;
            return Token::ProcessingInstructionStart;
        }

        tokenEnd = curpos + 1;
        return Token::TagStart;
    }
    if (isTagEnd(*curpos)) {
        tokenEnd = curpos + 1;
        return Token::TagEnd;
    }

    if (*curpos == '/' && isTagEnd(curpos[1])) {
        tokenEnd = curpos + 2;
        return Token::SelfClosingTagEnd;
    }

    // has to be data

    auto pos = curpos;
    bool whitespace = true;
    for (; pos < endpos && !isTagStart(*pos); pos++) {
        if (!IsWhitespace(*pos)) {
            whitespace = false;
        }
    }
    tokenEnd = pos;
    if (whitespace)
        return Token::Whitespace;
    else
        return Token::Text;
}

Token SimpleXmlLexer::TryGetName() {
    if (!isNameStartChar(*curpos))
        return Token::None;

    tokenStart = curpos;

    auto pos = curpos + 1;
    for (; pos < endpos && isNameChar(*pos); pos++);
    tokenEnd = pos;

    return Token::Name;
}

Token SimpleXmlLexer::TryGetAttribute() {
    auto pos = curpos;
    char lastChar;

    tokenStart = curpos;
    bool inAttrVal = false;
    char attrBoundary;

    if (IsWhitespace(*pos)) {
        for (pos++; pos < endpos && IsWhitespace(*pos); pos++);
        tokenEnd = pos;
        return Token::Whitespace;
    }

    for (; pos < endpos; pos++) {
        if (inAttrVal) {
            if (*pos == attrBoundary) {
                inAttrVal = false;
            }
        }
        else if (*pos == '"' || *pos == '\'') {
            inAttrVal = true;
            attrBoundary = *pos;
        }
        else if (IsWhitespace(*pos))
            break;
        else if (*pos == '/' || *pos == '>')
            break;
        lastChar = *pos;
    }
    tokenEnd = pos;
    if (TokenSize() == 0) {
        if (pos[0] == '/' && pos[1] == '>') {
            tokenEnd = pos + 2;
            return Token::SelfClosingTagEnd;
        }
        if (pos[0] == '>') {
            tokenEnd = pos + 1;
            return Token::TagEnd;
        }
        return Token::None;
    }

    return Token::Text;
}