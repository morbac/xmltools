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
        auto pos = strstr(__curpos+startpos, end);
        if (pos != NULL) {
            lexeme = Lexeme(Token::Unknown, __curpos, pos + strlen(end));
            return true;
        }
        else {
            lexeme = Lexeme(Token::Unknown,__curpos,__endpos);
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
            lexeme = Lexeme(Token::TagStart, __curpos, curpos + 2);
            return;
        }

        if (curpos[1] == '/') { // closing 
            lexeme = Lexeme(Token::ClosingTag, __curpos, curpos + 2);
            return;
        }
        if (curpos[1] == '?') {
            lexeme = Lexeme(Token::ProcessingInstructionStart, __curpos, curpos + 2);
            return;
        }


        lexeme = Lexeme(Token::TagStart, __curpos, curpos + 1);
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
            lexeme = Lexeme(Token::Text, __curpos, curpos);
            return;
        }

        if (!RegisterLinebreaks || (hasWhitespace && !hasLineBreak)) {
            lexeme = Lexeme(Token::Whitespace, __curpos, curpos);
            return;
        }
        if (hasLineBreak && !hasWhitespace) {
            lexeme = Lexeme(Token::Linebreak, __curpos, curpos);
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
        lexeme = Lexeme(ws ? Token::Whitespace : Token::Linebreak, __curpos, curpos);
    }

    void Lexer::handleInTag() {
        auto curpos = __curpos;

        if (isTagStart(*curpos)) {
            handleTagStart();
            return;
        }

        if (isTagEnd(*curpos)) {
            lexeme = Lexeme(Token::TagEnd,__curpos, curpos+1);
            return;
        }

        if (*curpos == '/' && isTagEnd(curpos[1])) {
            lexeme = Lexeme(Token::SelfClosingTagEnd, __curpos, curpos + 2);
            return;
        }

        if (*curpos == '?' && isTagEnd(curpos[1])) {
            lexeme = Lexeme(Token::ProcessingInstructionEnd, __curpos, curpos + 2);
            return;
        }

        if (RegisterLinebreaks) {
            if (IsWhitespace(*curpos)) {
                for (curpos++; IsWhitespace(*curpos); curpos++);
                lexeme = Lexeme(Token::Whitespace, __curpos, curpos);
                return;
            }
            if (IsLinebreak(*curpos)) {
                for (curpos++; IsLinebreak(*curpos); curpos++);
                lexeme = Lexeme(Token::Linebreak, __curpos, curpos);
                return;
            }
        }
        else {
            if (IsWhitespace(*curpos) || IsLinebreak(*curpos)) {
                for (curpos++; IsWhitespace(*curpos) || IsLinebreak(*curpos); curpos++);
                lexeme = Lexeme(Token::Whitespace, __curpos, curpos);
                return;
            }
        }

        if (*curpos == '=') {
            lexeme = Lexeme(Token::Eq, __curpos, curpos+1);
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
                lexeme = Lexeme(Token::SystemLiteral, __curpos, curpos);
                return;
            }
            curpos = __curpos; // reset and fall through
        }

        //if (isNameChar(*curpos)) {
            // should be isNameCharStart BUT we are not validating.... and we dont have unicode chars anyway
        for (curpos++;
            curpos < __endpos && (isNameChar(*curpos) || ((unsigned char)*curpos) > 127); // utf-8 highbit
            curpos++);

        if (isNameStartChar(*__curpos)) {
            lexeme = Lexeme(Token::Name, __curpos, curpos);
        }
        else {
            lexeme = Lexeme(Token::Nmtoken, __curpos, curpos);
        }
    }


    void Lexer::findNext() {
        lexeme.clear();
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
        lexeme.setEnd(curpos;
        if (!isNameStartChar(*curpos))
            return lexeme.token = Token::None;

        lexeme.text = curpos;

        auto pos = curpos + 1;
        for (; pos < __endpos && isNameChar(*pos); pos++);
        lexeme.setEnd(pos;

        return lexeme.token = Token::Name;
    }
    */
    Token Lexer::TryGetAttribute() {
        auto pos = __curpos;
        char lastChar;

        bool inAttrVal = false;
        char attrBoundary=0;

        if (IsWhitespace(*pos)) {
            for (pos++; pos < __endpos && IsWhitespace(*pos); pos++);
            lexeme = Lexeme(Token::Whitespace, __curpos, pos);
            return lexeme.token;
        }

        if (IsLinebreak(*pos)) {
            for (pos++; pos < __endpos && IsLinebreak(*pos); pos++);
            lexeme = Lexeme(Token::Linebreak, __curpos, pos);
            return lexeme.token;
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
        if (pos == __curpos) {
            if (pos[0] == '/' && pos[1] == '>') {
                lexeme = Lexeme(Token::SelfClosingTagEnd, __curpos, pos+2);
                return lexeme.token;
            }
            if (pos[0] == '>') {
                lexeme = Lexeme(Token::TagEnd, __curpos, pos + 1);
                return lexeme.token = Token::TagEnd;
            }
            lexeme = Lexeme(Token::Unknown, __curpos, __curpos);
            return lexeme.token; // TODO get rid of it
        }

        lexeme = Lexeme(Token::Text, __curpos, pos);
        return lexeme.token;
    }
}