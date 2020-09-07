#include "StdAfx.h"
#include <cstddef>
#include <string.h>

#include "SimpleXmlLexer.h"

#define ISTAGSTART(x) ((x=='<'))
#define ISTAGEND(x) ((x=='>'))


#define ISNAMESTARTCHAR(x) (ISNAMESTARTCHARSIMPLE(x) || ISNAMESTARTCHAREXTENDED(x))

#define ISNAMESTARTCHARSIMPLE(x) (\
    ((x) >= 'A' && (x) <= 'Z') || \
    ((x) >= 'a' && (x) <= 'z') || \
    (x) == '_' || \
    (x) == ':'\
)

#define ISNAMESTARTCHAREXTENDED(x) ( \
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

#define ISNAMECHAR(x) ( \
    ISNAMESTARTCHARSIMPLE(x) || \
    (x) == '-' || \
    (x) == '.' || \
    ((x)>='0' && (x)<='9') || \
    (x) == 0xB7 || \
    ((x) >= 0x0300 && (x) <= 0x036F) || \
    ((x) >= 0x203F && (x) <= 0x2040) || \
    ISNAMESTARTCHAREXTENDED(x) \
)

namespace SimpleXml {

    Lexer::Lexer(const char* start, size_t len) {
        xml = start;
        __curpos = xml;
        __endpos = xml + len;
        _isInTag = false;
    }

    void Lexer::reset() {
        __curpos = xml;
        _isInTag = false;
    }

    bool Lexer::readUntilTagEndOrStart() {
        if (Done()) {
            _lexeme = Lexeme(Token::InputEnd, __curpos, __curpos);
            return false;
        }

        auto pos = __curpos;
        while (pos < __endpos && !ISTAGSTART(*pos) && !ISTAGEND(*pos)) {
            pos++;
        }

        _lexeme = Lexeme(Token::Unknown, __curpos, pos);
        return pos < __endpos;
    }

    bool Lexer::readUntil(int startpos, const char* end, bool includeEnd) {
        auto pos = strstr(__curpos+startpos, end);
        if (pos != NULL) {
            if (includeEnd)
                _lexeme = Lexeme(Token::Unknown, __curpos, pos + strlen(end));
            else
                _lexeme = Lexeme(Token::Unknown, __curpos, pos);

            return true;
        }
        else {
            _lexeme = Lexeme(Token::Unknown,__curpos,__endpos);
            return false;
        }
    }

    Lexeme Lexer::tryReadName() {
        if (ISNAMESTARTCHAR(*__curpos)) {
            auto pos = __curpos;
            for (;
                pos < __endpos && (ISNAMECHAR(*pos) || ((unsigned char)*pos) > 127); // utf-8 highbit
                pos++);
            return _lexeme = Lexeme(Token::Name, __curpos, pos);
        }
        return Lexeme(Token::Unknown, NULL, NULL);
    }

    void Lexer::handleTagStart() {
        auto curpos = __curpos;

        // check for comment, CDATA, 
        if (curpos[1] == '!') {
            if (curpos[2] == '-' && curpos[3] == '-') { // <!--
                curpos += 4;
               
                readUntil(4,"-->",true);
                _lexeme.token = Token::Comment;
                return;
            }

            if (0 == strncmp(curpos, CDStart, 9)) {
                readUntil(9, CDEnd,true);
                _lexeme.token = Token::CDSect;
                return;
            }

            //<!SOMENAME is also valid
            _lexeme = Lexeme(Token::DeclStart, __curpos, curpos + 2);
            return;
        }

        if (curpos[1] == '/') { // closing 
            _lexeme = Lexeme(Token::ClosingTag, __curpos, curpos + 2);
            return;
        }
        if (curpos[1] == '?') {
            _lexeme = Lexeme(Token::ProcessingInstructionStart, __curpos, curpos + 2);
            return;
        }


        _lexeme = Lexeme(Token::TagStart, __curpos, curpos + 1);
    }

    void Lexer::handleOutsideTag() {
        bool hasText = false;
        bool hasWhitespace = false;
        bool hasLineBreak = false;
        auto curpos = __curpos;

        if (ISTAGSTART(*curpos)) {
            handleTagStart();
            return;
        }

        for (; curpos < __endpos; curpos++) {
            if (ISTAGSTART(*curpos))
                break;

            if (isWhitespace(*curpos))
                hasWhitespace = true;
            else if (isLinebreak(*curpos))
                hasLineBreak = true;
            else
                hasText = true;
        }
        if (hasText) {
            _lexeme = Lexeme(Token::Text, __curpos, curpos);
            return;
        }

        if (!registerLinebreaks || (hasWhitespace && !hasLineBreak)) {
            _lexeme = Lexeme(Token::Whitespace, __curpos, curpos);
            return;
        }
        if (hasLineBreak && !hasWhitespace) {
            _lexeme = Lexeme(Token::Linebreak, __curpos, curpos);
            return;
        }
        // mixed stuff

        curpos = __curpos;
        bool ws = isWhitespace(*curpos);
        for (curpos++; curpos < __endpos; curpos++) {
            if (ws && !isWhitespace(*curpos))
                break;
            if (!ws && !isLinebreak(*curpos))
                break;
        }
        /*
        auto pos = curpos;
        auto token = Token::None;
        auto lasttoken = Token::None;
        for (; pos < __endpos && !isTagStart(*pos); pos++) {
            if (isWhitespace(*pos)) token = Token::Whitespace;
            else if (isLinebreak(*pos)) token = Token::Linebreak;
            else token = Token::Text;

            if (lasttoken == Token::None) {
                lasttoken = token;
            }
            else if (token != lasttoken) {
                break;
            }
        }
        */
        _lexeme = Lexeme(ws ? Token::Whitespace : Token::Linebreak, __curpos, curpos);
    }

    void Lexer::handleInTag() {
        auto curpos = __curpos;

        if (ISTAGSTART(*curpos)) {
            handleTagStart();
            return;
        }

        if (ISTAGEND(*curpos)) {
            _lexeme = Lexeme(Token::TagEnd,__curpos, curpos+1);
            return;
        }

        if (*curpos == '/' && ISTAGEND(curpos[1])) {
            _lexeme = Lexeme(Token::SelfClosingTagEnd, __curpos, curpos + 2);
            return;
        }

        if (*curpos == '?' && ISTAGEND(curpos[1])) {
            _lexeme = Lexeme(Token::ProcessingInstructionEnd, __curpos, curpos + 2);
            return;
        }

        if (registerLinebreaks) {
            if (isWhitespace(*curpos)) {
                for (curpos++; isWhitespace(*curpos); curpos++);
                _lexeme = Lexeme(Token::Whitespace, __curpos, curpos);
                return;
            }
            if (isLinebreak(*curpos)) {
                for (curpos++; isLinebreak(*curpos); curpos++);
                _lexeme = Lexeme(Token::Linebreak, __curpos, curpos);
                return;
            }
        }
        else {
            if (isWhitespace(*curpos) || isLinebreak(*curpos)) {
                for (curpos++; isWhitespace(*curpos) || isLinebreak(*curpos); curpos++);
                _lexeme = Lexeme(Token::Whitespace, __curpos, curpos);
                return;
            }
        }

        if (*curpos == '=') {
            _lexeme = Lexeme(Token::Eq, __curpos, curpos+1);
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
                _lexeme = Lexeme(Token::SystemLiteral, __curpos, curpos);
                return;
            }
            curpos = __curpos; // reset and fall through
        }

        //if (ISNAMECHAR(*curpos)) {
            // should be isNameCharStart BUT we are not validating.... and we dont have unicode chars anyway
        for (curpos++;
            curpos < __endpos && (ISNAMECHAR(*curpos) || ((unsigned char)*curpos) > 127); // utf-8 highbit
            curpos++);

        if (ISNAMESTARTCHAR(*__curpos)) {
            _lexeme = Lexeme(Token::Name, __curpos, curpos);
        }
        else {
            _lexeme = Lexeme(Token::Nmtoken, __curpos, curpos);
        }
    }


    void Lexer::findNext() {
        _lexeme.clear();

        if (__curpos == __endpos) {
            _lexeme.token = Token::InputEnd;
        }
        else 
        if (_isInTag) {
            handleInTag();
        }
        else {
            handleOutsideTag();
        }
    }

    Token Lexer::TryGetAttribute() {
        auto pos = __curpos;
        char lastChar;

        bool inAttrVal = false;
        char attrBoundary=0;

        if (isWhitespace(*pos)) {
            for (pos++; pos < __endpos && isWhitespace(*pos); pos++);
            _lexeme = Lexeme(Token::Whitespace, __curpos, pos);
            return _lexeme.token;
        }

        if (isLinebreak(*pos)) {
            for (pos++; pos < __endpos && isLinebreak(*pos); pos++);
            _lexeme = Lexeme(Token::Linebreak, __curpos, pos);
            return _lexeme.token;
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
            else if (isWhitespace(*pos) || isLinebreak(*pos))
                break;
            else if (*pos == '/' || *pos == '>' || *pos == '<')
                break;
            lastChar = *pos;
        }
        if (pos == __curpos) {
            if (pos[0] == '/' && pos[1] == '>') {
                _lexeme = Lexeme(Token::SelfClosingTagEnd, __curpos, pos+2);
                return _lexeme.token;
            }
            if (pos[0] == '>') {
                _lexeme = Lexeme(Token::TagEnd, __curpos, pos + 1);
                return _lexeme.token = Token::TagEnd;
            }
            _lexeme = Lexeme(Token::Unknown, __curpos, __curpos);
            return _lexeme.token; // TODO get rid of it
        }

        _lexeme = Lexeme(Token::Text, __curpos, pos);
        return _lexeme.token;
    }

    void Lexer::EatToken() { eatToken(); }

    void Lexer::eatToken() {
        if (!_lexeme.value) return;

        if (trackPosition) {
            if (_lexeme == Token::Text ||
                _lexeme == Token::Whitespace ||
                _lexeme == Token::Linebreak ||
                _lexeme == Token::CDSect ||
                _lexeme == Token::Comment ||
                _lexeme == Token::Unknown) {
                for (auto pc = _lexeme.value.text();
                    pc < _lexeme.value.end();
                    pc++) {
                    if (*pc == '\n') {
                        pos_row++;
                        pos_col = 0;
                    }
                    else if (*pc == '\r'); // just eat em up
                    else {
                        pos_col++;
                    }
                }
            }
        }

        __curpos = _lexeme.value.end();

        if (isTagStart(_lexeme.token))
            _isInTag = true;

        else if (isTagEnd(_lexeme.token))
            _isInTag = false;
    }
}