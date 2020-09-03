#include "StdAfx.h"
#include <cstddef>
#include <string.h>

#include "SimpleXmlLexer.h"

#define isTagStart(x) ((x=='<'))
#define isTagEnd(x) ((x=='>'))


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

    bool Lexer::readUntil(int startpos, const char* end) {
        lexeme.token = Token::Unknown;
        lexeme.text = __curpos;

        auto pos = strstr(__curpos+startpos, end);
        if (pos != NULL) {
            lexeme.end = pos + strlen(end);
            return true;
        }
        else {
            lexeme.end = __endpos;
            return false;
        }
    }

    void Lexer::handleTagStart() {
        auto curpos = __curpos;

        // check for comment, CDATA, 
        if (curpos[1] == '!') {
            if (curpos[2] == '-' && curpos[3] == '-') { // <!--
                curpos += 4;
               
                readUntil(4,"-->");
                lexeme.token = Token::Comment;
                return;
            }

            if (0 == strncmp(curpos, CDStart, 9)) {
                readUntil(9, CDEnd);
                lexeme.token = Token::CDSect;
                return;
            }

            //<!SOMENAME is also valid
            lexeme.end = curpos + 2;
            lexeme.token = Token::TagStart;
            return;
        }

        if (curpos[1] == '/') { // closing 
            lexeme.end = curpos + 2;
            lexeme.token = Token::ClosingTag;
            return;
        }
        if (curpos[1] == '?') {
            lexeme.end = curpos + 2;
            lexeme.token = Token::ProcessingInstructionStart;
            return;
        }

        lexeme.end = curpos + 1;
        lexeme.token = Token::TagStart;
    }

    void Lexer::handleOutsideTag() {
        bool hasText = false;
        bool hasWhitespace = false;
        bool hasLineBreak = false;
        auto curpos = __curpos;

        if (isTagStart(*curpos)) {
            handleTagStart();
            return;
        }

        for (; curpos < __endpos; curpos++) {
            if (isTagStart(*curpos))
                break;

            if (IsWhitespace(*curpos))
                hasWhitespace = true;
            else if (IsLinebreak(*curpos))
                hasLineBreak = true;
            else
                hasText = true;
        }
        if (hasText) {
            lexeme.end = curpos;
            lexeme.token = Token::Text;
            return;
        }

        if (!RegisterLinebreaks || (hasWhitespace && !hasLineBreak)) {
            lexeme.end = curpos;
            lexeme.token = Token::Whitespace;
            return;
        }
        if (hasLineBreak && !hasWhitespace) {
            lexeme.end = curpos;
            lexeme.token = Token::Linebreak;
            return;
        }
        // mixed stuff

        curpos = __curpos;
        bool ws = IsWhitespace(*curpos);
        for (curpos++; curpos < __endpos; curpos++) {
            if (ws && !IsWhitespace(*curpos))
                break;
            if (!ws && !IsLinebreak(*curpos))
                break;
        }
        /*
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
        */
        lexeme.end = curpos;
        lexeme.token = ws ? Token::Whitespace : Token::Linebreak;
    }

    void Lexer::handleInTag() {
        auto curpos = __curpos;

        if (isTagStart(*curpos)) {
            handleTagStart();
            return;
        }

        if (isTagEnd(*curpos)) {
            lexeme.end = curpos + 1;
            lexeme.token = Token::TagEnd;
            return;
        }

        if (*curpos == '/' && isTagEnd(curpos[1])) {
            lexeme.end = curpos + 2;
            lexeme.token = Token::SelfClosingTagEnd;
            return;
        }

        if (*curpos == '?' && isTagEnd(curpos[1])) {
            lexeme.end = curpos + 2;
            lexeme.token = Token::ProcessingInstructionEnd;
            return;
        }

        if (RegisterLinebreaks) {
            if (IsWhitespace(*curpos)) {
                for (curpos++; IsWhitespace(*curpos); curpos++);
                lexeme.end = curpos;
                lexeme.token = Token::Whitespace;
                return;
            }
            if (IsLinebreak(*curpos)) {
                for (curpos++; IsLinebreak(*curpos); curpos++);
                lexeme.end = curpos;
                lexeme.token = Token::Linebreak;
                return;
            }
        }
        else {
            if (IsWhitespace(*curpos) || IsLinebreak(*curpos)) {
                for (curpos++; IsWhitespace(*curpos) || IsLinebreak(*curpos); curpos++);
                lexeme.end = curpos;
                lexeme.token = Token::Whitespace;
                return;
            }
        }

        if (*curpos == '=') {
            lexeme.end = curpos + 1;
            lexeme.token = Token::Eq;
            return;
        }

        if (*curpos == '\'' || *curpos == '"') {
            auto attrBoundary = *curpos;
            bool ok = false;
            for (curpos++; curpos < __endpos; curpos++) {
                if (*curpos == attrBoundary) {
                    curpos++;
                    ok = true;
                    break;
                }
            }
            if (ok == true) {
                lexeme.end = curpos;
                lexeme.token = Token::SystemLiteral;
                return;
            }
            curpos = __curpos; // reset and fall through
        }

        //if (isNameChar(*curpos)) {
            // should be isNameCharStart BUT we are not validating.... and we dont have unicode chars anyway
        for (curpos++;
            curpos < __endpos && (isNameChar(*curpos) || ((unsigned char)*curpos) > 127); // utf-8 highbit
            curpos++);
        lexeme.end = curpos;
        if (isNameStartChar(*lexeme.text)) {
            lexeme.token = Token::Name;
        }
        else {
            lexeme.token = Token::Nmtoken;
        }
    }


    void Lexer::findNext() {
        lexeme.text = __curpos;
        lexeme.end = __curpos;
        lexeme.token = Token::None;

        if (__curpos == __endpos) {
            lexeme.token = Token::InputEnd;
        }
        else 
        if (isInTag) {
            handleInTag();
        }
        else {
            handleOutsideTag();
        }
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