#include <cstddef>
#include <string.h>
#include <ostream>

#include "Lexer.h"

#define ISTAGSTART(x) ((x=='<'))
#define ISTAGEND(x) ((x=='>'))


#define ISNAMESTARTCHAR(x) (ISNAMESTARTCHARSIMPLE(x))// || ISNAMESTARTCHAREXTENDED(x))

#define ISNAMESTARTCHARSIMPLE(x) (\
    ((x) >= 'A' && (x) <= 'Z') || \
    ((x) >= 'a' && (x) <= 'z') || \
    (x) == '_' || \
    (x) == ':' || \
    ((unsigned)x>=0x80)\
)
/*
#define ISNAMESTARTCHAREXTENDED(x) ( \
    ((x) >= 0xC0   && (x) <= 0xD6) || \
    ((x) >= 0xD8   && (x) <= 0xF6) || 
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
*/
#define ISNAMECHAR(x) ( \
    ISNAMESTARTCHARSIMPLE(x) || \
    (x) == '-' || \
    (x) == '.' || \
    ((x)>='0' && (x)<='9') \
)
/*
    (x) == 0xB7 || \
    ((x) >= 0x0300 && (x) <= 0x036F) || \
    ((x) >= 0x203F && (x) <= 0x2040) || \
    ISNAMESTARTCHAREXTENDED(x) \*/

namespace SimpleXml {

    Lexer::Lexer(ChunkedStream &input) {
        inputStream = &input;
        _isInTag = false;
        currentToken = Token::None;
        currentTokenSize = 0;
    }

    void Lexer::reset() {
        inputStream->reset();
        _isInTag = false;
        currentToken = Token::None;
        currentTokenSize = 0;
        pos_col = 1;
        pos_row = 1;
    }

    template <typename Function>
    static inline bool readUntil(ChunkedStream *inputStream, size_t startoffset, size_t &endsize, Function end_pred)
    {
        const char* start = inputStream->begin();
        const char* end = inputStream->end();
        int bufidx = -1;
        endsize = 0;
        while (startoffset > 0) {
            size_t avail = end - start;
            if (startoffset < avail) {
                start += startoffset;
                endsize += startoffset;
                break;;
            }

            startoffset -= avail;
            endsize += avail;
            start += avail;

            bufidx++;
            auto b = inputStream->peekBuf(bufidx);
            if (b.data == NULL)
                return false;
            else {
                start = b.data;
                end = b.data + b.size;
            }
        }
        while (1) {
            const char* pos;
            for (pos = start; pos < end; pos++)
            {
                if (end_pred(*pos)) {
                    endsize += pos - start;
                    return true;
                }
            }
            endsize += pos - start;
            
            bufidx++;
            auto b = inputStream->peekBuf(bufidx);
            if (b.data == NULL)
                break;
            else {
                start = b.data;
                end = b.data + b.size;
            }
        }
        return false;
    }

    bool Lexer::readUntilTagEndOrStart() {
        if (Done()) {
            return false;
        }

        currentToken = Token::Unknown;
        bool found = SimpleXml::readUntil(inputStream, 0, currentTokenSize, [](char c) { return ISTAGSTART(c) || ISTAGEND(c); });
        return found;
/*
        bool tagFound = false;
        int bufidx = -1;

        currentToken = Token::Unknown;
        currentTokenSize = 0;

        const char* start = inputStream->begin();
        const char* end = inputStream->end();
        while (!tagFound) {
            const char* pos;
            for (pos = start; pos < end; pos++)
            {
                if (ISTAGSTART(*pos) || ISTAGEND(*pos)) {
                    tagFound = true;
                    break;
                }
            }
            currentTokenSize += pos - inputStream->begin();
            if (!tagFound)
            {
                bufidx++;
                auto b = inputStream->peekBuf(bufidx);
                if (b.data == NULL)
                    break;
                else {
                    start = b.data;
                    end = b.data + b.size;
                }
            }
        }

        return false;
*/
    }

    bool Lexer::readUntil(int startpos, const char*match, bool includeEnd) {
        currentToken = Token::Unknown;
        currentTokenSize = startpos;
        do {
            bool found = SimpleXml::readUntil(inputStream, currentTokenSize, currentTokenSize, [match](char c) {return c == *match; });
            if (!found)
                return false;

            auto matchlen = strlen(match);
            size_t i;
            for (i = 1; i < matchlen; i++) {
                if (match[i] != inputStream->peekChar(currentTokenSize + i))
                    break;
            }
            if (i == matchlen) {
                // found
                if (includeEnd) {
                    currentTokenSize += matchlen;
                }
                return true;
            }
            currentTokenSize += 1;
        } while (true);
    }

    bool Lexer::tryReadWhitespace() {
        char c = inputStream->peekChar();
        if (isWhitespace(c) || (!parms.registerLinebreaks && isLinebreak(c))) {
            if (parms.registerLinebreaks)
                SimpleXml::readUntil(inputStream, 0, currentTokenSize, [](char c) {return !isWhitespace(c); });
            else
                SimpleXml::readUntil(inputStream, 0, currentTokenSize, [](char c) {return !((isWhitespace(c) || isLinebreak(c))); });

            currentToken = Token::Whitespace;
            return true;
        }
        return false;
    }

    bool Lexer::tryReadName() {
        char c = inputStream->peekChar(0);
        if (ISNAMESTARTCHAR(c)) {
            int idx = 0;
            do {
                idx++;
                c = inputStream->peekChar(idx); // slow'ish but better than nothing
            } while (ISNAMECHAR(c));

            currentToken = Token::Name;
            currentTokenSize = idx;
            return true;
        }
        return false;
    }

    bool Lexer::tryReadNmtoken() {
        char c = inputStream->peekChar(0);
        if (ISNAMECHAR(c)) {
            int idx = 0;
            do {
                idx++;
                c = inputStream->peekChar(idx); // slow'ish but better than nothing
            } while (ISNAMECHAR(c));

            currentToken = Token::Nmtoken;
            currentTokenSize = idx;
            return true;
        }
        return false;
    }

    void Lexer::handleTagStart() {
        char c2 = inputStream->peekChar(1);
        
        if (c2 == '/') { // closing 
            currentToken = Token::ClosingTag;
            currentTokenSize = 2;
            return;
        }

        if (c2 == '!') {
            // check for comment, CDATA, 
            if (inputStream->peekMatch("<!--")) {
                if (readUntil(4, "-->", true))
                {
                }
                currentToken = Token::Comment;
                return;
            }

            if (inputStream->peekMatch(CDStart)) {
                readUntil(9, CDEnd, true);
                currentToken = Token::CDSect;
                return;
            }

            //<!SOMENAME is also valid
            currentToken = Token::DeclStart;
            currentTokenSize = 2;
            return;
        }

        if (c2 == '?') {
            currentToken = Token::ProcessingInstructionStart;
            currentTokenSize = 2;
            return;
        }

        currentToken = Token::TagStart;
        currentTokenSize = 1;
    }

    void Lexer::handleOutsideTag() {
        bool hasText = false;
        bool hasWhitespace = false;
        bool hasLineBreak = false;

        if (ISTAGSTART(inputStream->peekChar(0))) {
            handleTagStart();
            return;
        }

        bool tagFound = false;
        int bufidx = -1;

        currentToken = Token::Unknown;
        currentTokenSize = 0;

        auto start = inputStream->begin();
        auto end = inputStream->end();
        while (!tagFound) {
            const char* pos;
            for (pos = start; pos < end; pos++)
            {
                if (ISTAGSTART(*pos)) {
                    tagFound = true;
                    break;
                }

                if (isWhitespace(*pos))
                    hasWhitespace = true;
                else if (isLinebreak(*pos))
                    hasLineBreak = true;
                else
                    hasText = true;
            }
            currentTokenSize += pos - start;
            if (!tagFound)
            {
                bufidx++;
                auto b = inputStream->peekBuf(bufidx);
                if (b.data == NULL)
                    break;
                else {
                    start = b.data;
                    end = b.data + b.size;
                }
            }
        }

        if (hasText) {
            currentToken = Token::Text;
            return;
        }

        if (!parms.registerLinebreaks || (hasWhitespace && !hasLineBreak)) {
            currentToken = Token::Whitespace;
            return;
        }
        if (hasLineBreak && !hasWhitespace) {
            currentToken = Token::Linebreak;
            return;
        }
        // mixed stuff

        if (inputStream->eod()) {
            currentToken = Token::InputEnd;
            return;
        }

        if (currentTokenSize < inputStream->available())
        {
            auto pos = inputStream->begin();
            auto textend = pos + currentTokenSize;

            bool ws = isWhitespace(*pos);
            for (pos++; pos < textend; pos++) {
                if (ws && !isWhitespace(*pos))
                    break;
                if (!ws && !isLinebreak(*pos))
                    break;
            }
            if (ws) currentToken = Token::Whitespace;
            else currentToken = Token::Linebreak;
            currentTokenSize = pos - inputStream->begin();
            return;
        }
        // fall back to the slow version
        {
            bool ws = isWhitespace(inputStream->peekChar());
            size_t i;
            for (i = 1; i < currentTokenSize; i++) {
                char c = inputStream->peekChar(i);
                if (ws && !isWhitespace(c))
                    break;
                if (!ws && !isLinebreak(c))
                    break;
            }
            if (ws) currentToken = Token::Whitespace;
            else currentToken = Token::Linebreak;
            currentTokenSize = i;
            return;
        }
    }

    void Lexer::handleInTag() {
        auto c = inputStream->peekChar();

        if (ISTAGSTART(c)) {
            handleTagStart();
            return;
        }

        if (ISTAGEND(c)) {
            currentToken = Token::TagEnd;
            currentTokenSize = 1;
            return;
        }

        if (c == '/' && ISTAGEND(inputStream->peekChar(1))) {
            currentToken = Token::SelfClosingTagEnd;
            currentTokenSize = 2;
            return;
        }

        if (c == '?' && ISTAGEND(inputStream->peekChar(1))) {
            currentToken = Token::ProcessingInstructionEnd;
            currentTokenSize = 2;
            return;
        }

        if (parms.registerLinebreaks) {
            if (isWhitespace(c)) {
                SimpleXml::readUntil(inputStream, 1, currentTokenSize, [](char c) { return !(isWhitespace(c)); });
                currentToken = Token::Whitespace;
                return;
            }
            if (isLinebreak(c)) {
                SimpleXml::readUntil(inputStream, 1, currentTokenSize, [](char c) { return !(isLinebreak(c)); });
                currentToken = Token::Linebreak;
                return;
            }
        }
        else {
            if (isWhitespace(c) || isLinebreak(c)) {
                SimpleXml::readUntil(inputStream, 1, currentTokenSize, [](char c) { return !(isWhitespace(c) || isLinebreak(c)); });
                currentToken = Token::Whitespace;
                return;
            }
        }

        if (c == '='){
            currentToken = Token::Eq;
            currentTokenSize = 1;
            return;
        }

        if (c == '\'' || c == '"') {
            auto attrBoundary = c;
            bool ok = SimpleXml::readUntil(inputStream, 1, currentTokenSize, [attrBoundary](char c) { return c == attrBoundary; });
            if (ok == true) {
                currentTokenSize += 1;
                currentToken = Token::SystemLiteral;
                return;
            }
        }

        if (!ISNAMECHAR(c)) {
            // should be isNameCharStart BUT we are not validating.... and we dont have unicode chars anyway
            currentTokenSize = 1;
            currentToken = Token::Unknown;
            return;
        }
        if (ISNAMESTARTCHAR(c)) {
            currentToken = Token::Name;
        }
        else {
            currentToken = Token::Nmtoken;
        }
        SimpleXml::readUntil(inputStream, 1, currentTokenSize, [](char c) { return !ISNAMECHAR(c); });
    }


    void Lexer::findNext() {
        currentToken = Token::None;
        currentTokenSize = 0;
/*
        if (inputStream->eod()) {
            currentToken = Token::InputEnd;
        }
        else */
        if (_isInTag) {
            handleInTag();
        }
        else {
            handleOutsideTag();
        }
    }

    void Lexer::trackPosition(size_t len) {
        if (len < inputStream->available()) {
            trackPosition(inputStream->begin(), len);
            return;
        }

        trackPosition(inputStream->begin(), inputStream->available());
        ChunkedStream::buf b;
        int bidx = -1;
        do {
            b = inputStream->peekBuf(++bidx);
            size_t copysize;
            if (b.size > len)
                copysize = len;
            else
                copysize = b.size;

            trackPosition(b.data, copysize);
            len -= copysize;
        } while (len > 0 && b.size > 0);
    }

    void Lexer::trackPosition(const char* data, size_t len) {
        for (size_t i = 0; i < len; i++){
            char c = data[i];

            if (c == '\n') {
                pos_row++;
                pos_col = 1;
            }
            else if (c == '\r'); // just eat em up
            else if (((unsigned char)c)<0x80) {// utf-8 marker means 2nd byte... not a char
                pos_col++;
            }
        }
    }

    void Lexer::readTokenData(char* target) {
        if (parms.trackPosition) {
            trackPosition(currentTokenSize);
        }
        inputStream->read(target, currentTokenSize);

        if (isTagStart(currentToken))
            _isInTag = true;

        else if (isTagEnd(currentToken))
            _isInTag = false;

        currentToken = Token::None;
        currentTokenSize = 0;
    }

    void Lexer::eatToken() {
        if (currentToken == Token::None) return;

        if (parms.trackPosition) {
            trackPosition(currentTokenSize);
        }
        inputStream->skip(currentTokenSize);

        if (isTagStart(currentToken))
            _isInTag = true;

        else if (isTagEnd(currentToken))
            _isInTag = false;

        currentToken = Token::None;
        currentTokenSize = 0;
    }

    void Lexer::writeTokenData(std::ostream& out) {
        char tmp[128];

        auto size = inputStream->available();
        if (size >= currentTokenSize) { // happy flow
            auto begin = inputStream->begin();
            out.write(begin, currentTokenSize);
            if (parms.trackPosition) {
                trackPosition(begin, currentTokenSize);
            }
            inputStream->skip(currentTokenSize);
        }
        else { // not so happy flow
            while (currentTokenSize > 0) {

                auto copysize = currentTokenSize;
                if (copysize > sizeof(tmp))
                    copysize = sizeof(tmp);

                inputStream->read(tmp, copysize);
                if (parms.trackPosition) {
                    trackPosition(tmp, copysize);
                }

                out.write(tmp, copysize);
                currentTokenSize -= copysize;
            }

        }

        if (isTagStart(currentToken))
            _isInTag = true;

        else if (isTagEnd(currentToken))
            _isInTag = false;

        currentToken = Token::None;
        currentTokenSize = 0;
    }

    void Lexer::readSystemLiteral(string& s) {
        if (!currentTokenSize)
            return;

        if (parms.trackPosition) {
            trackPosition(currentTokenSize);
        }

        inputStream->skip(1);
        s.resize(--currentTokenSize);
        inputStream->read(&s[0],s.size());
        if (s[s.size() - 1] == '"' || s[s.size() - 1] == '\'')
            s.resize(s.size() - 1);

        currentToken = Token::None;
        currentTokenSize = 0;
    }
}