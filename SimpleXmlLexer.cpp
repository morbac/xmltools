#include "StdAfx.h"
#include <cstddef>
#include <string.h>

#include "SimpleXmlLexer.h"

/*
#define isWhitespace(x) ((x) == 0x20 || (x) == 0x9)
#define isLinebreak(x) ((x) == 0xD || (x) == 0x0A)
*/
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

namespace SimpleXml {

    Lexer::Lexer(const char* start, size_t len) {
        xml = start;
        __curpos = xml;
        __endpos = xml + len;
        isInTag = false;
    }


    Token Lexer::FindNext() {
        if (__curpos == __endpos) {
            return Token::InputEnd;
        }

        lexeme.text = __curpos;
        lexeme.end = __curpos;
        auto curpos = __curpos;

        if (isTagStart(*curpos)) {
            // check for comment, CDATA, 
            if (curpos[1] == '!') {
                if (curpos[2] == '-' && curpos[3] == '-') { // <!--
                    lexeme.end = strstr(curpos + 4, "-->");
                    if (lexeme.end == NULL) {
                        lexeme.end = __endpos; // not found.. copy rest
                    }
                    else {
                        lexeme.end += 3;
                    }
                    return Token::Comment;
                }

                if (0 == strncmp(curpos, CDStart, 9)) {
                    lexeme.end = strstr(curpos + 9, CDEnd);
                    if (lexeme.end == NULL) {
                        lexeme.end = __endpos; // not found.. copy rest
                    }
                    else {
                        lexeme.end += strlen(CDEnd);
                    }

                    return Token::CDSect;
                }

                //<!SOMENAME is also valid
                lexeme.end = curpos + 2;
                return Token::TagStart;
            }

            if (curpos[1] == '/') { // closing 
                lexeme.end = curpos + 2;
                return Token::ClosingTag;
            }
            if (curpos[1] == '?') {
                lexeme.end = curpos + 2;
                return Token::ProcessingInstructionStart;
            }

            lexeme.end = curpos + 1;
            return Token::TagStart;
        }
        if (isInTag) {
            if (*curpos == '/' && isTagEnd(curpos[1])) {
                lexeme.end = curpos + 2;
                return Token::SelfClosingTagEnd;
            }

            if (isTagEnd(*curpos)) {
                lexeme.end = curpos + 1;
                return Token::TagEnd;
            }

            if (IsWhitespace(*curpos)) {
                for (curpos++; IsWhitespace(*curpos); curpos++);
                lexeme.end = curpos;
                return Token::Whitespace;
            }
            if (IsLinebreak(*curpos)) {
                for (curpos++; IsLinebreak(*curpos); curpos++);
                lexeme.end = curpos;
                return Token::Linebreak;
            }

            if (*curpos == '=') {
                lexeme.end = curpos + 1;
                return Token::Eq;
            }

            //if (isNameChar(*curpos)) {
                // should be isNameCharStart BUT we are not validating.... and we dont have unicode chars anyway
                for (curpos++; 
                    curpos < __endpos && (isNameChar(*curpos) || ((unsigned char)*curpos)>127); // utf-8 highbit
                    curpos++);
                lexeme.end = curpos;
                if (isNameStartChar(*lexeme.text))
                    return Token::Name;
                return Token::Nmtoken;
            //}

            //lexeme.end = curpos + 1;
            //return Token::Unknown;
        }

        // has to be text since we are outside tags

        auto pos = curpos;
        auto token = Token::None;
        auto lasttoken = Token::None;
        for (; pos < __endpos && !isTagStart(*pos); pos++) {
            if (IsWhitespace(*pos)) token = Token::Whitespace;
            else if (IsLinebreak(*pos)) token = Token::Linebreak;
            else token = Token::Text;

            if (lasttoken == Token::None) {
                lasttoken = token;
            }
            else if (token != lasttoken) {
                break;
            }
        }

        lexeme.end = pos;
        return lexeme.token = lasttoken;
    }
    /*
    Token Lexer::TryGetName() {
        auto curpos = __curpos;
        lexeme.end = curpos;
        if (!isNameStartChar(*curpos))
            return lexeme.token = Token::None;

        lexeme.text = curpos;

        auto pos = curpos + 1;
        for (; pos < __endpos && isNameChar(*pos); pos++);
        lexeme.end = pos;

        return lexeme.token = Token::Name;
    }
    */
    Token Lexer::TryGetAttribute() {
        auto pos = __curpos;
        char lastChar;

        lexeme.text = pos;
        lexeme.end = pos;
        bool inAttrVal = false;
        char attrBoundary;

        if (IsWhitespace(*pos)) {
            for (pos++; pos < __endpos && IsWhitespace(*pos); pos++);
            lexeme.end = pos;
            return lexeme.token = Token::Whitespace;
        }

        if (IsLinebreak(*pos)) {
            for (pos++; pos < __endpos && IsLinebreak(*pos); pos++);
            lexeme.end = pos;
            return lexeme.token = Token::Linebreak;
        }

        for (; pos < __endpos; pos++) {
            if (inAttrVal) {
                if (*pos == attrBoundary) {
                    inAttrVal = false;
                }
            }
            else if (*pos == '"' || *pos == '\'') {
                inAttrVal = true;
                attrBoundary = *pos;
            }
            else if (IsWhitespace(*pos) || IsLinebreak(*pos))
                break;
            else if (*pos == '/' || *pos == '>' || *pos == '<')
                break;
            lastChar = *pos;
        }
        lexeme.end = pos;
        if (TokenSize() == 0) {
            if (pos[0] == '/' && pos[1] == '>') {
                lexeme.end = pos + 2;
                return lexeme.token = Token::SelfClosingTagEnd;
            }
            if (pos[0] == '>') {
                lexeme.end = pos + 1;
                return lexeme.token = Token::TagEnd;
            }
            return lexeme.token = Token::None; // TODO get rid of it
        }

        return lexeme.token = Token::Text;
    }
}