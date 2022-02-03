#include <sstream>

#include "PrettyPrinter.h"

namespace SimpleXml {

    inline void PrettyPrinter::WriteCloseTag() {
        outText.write(">", 1);
        tagIsOpen = false;
        inTag = false;
    }

    inline void PrettyPrinter::TryCloseTag() {
        if (tagIsOpen) {
            WriteCloseTag();
            indentlevel++;
        }
    }

    inline void PrettyPrinter::Indent() {
        if (!indented && parms.insertIndents) {
            for (int i = 0; i < indentlevel && i < parms.maxElementDepth; i++) {
                outText.write(parms.tab.c_str(), parms.tab.length());
            }
        }
        indented = true;
    }

    inline void PrettyPrinter::TryIndent() {
        TryCloseTag();
        if (parms.insertNewLines) {
            Indent();
        }
    }

    inline void PrettyPrinter::AddNewline() {
        if (parms.insertNewLines && !parms.keepExistingBreaks) {
            outText.write(parms.eol.c_str(), parms.eol.length());
            indented = false;
        }
    }

    inline void PrettyPrinter::StartNewElement() {
        if (tagIsOpen) {
            TryCloseTag();
            AddNewline();
        }
        else if (indented) {
            AddNewline();
        }
    }

    inline void PrettyPrinter::WriteToken() {
        lexer.writeTokenData(outText);
        //outText.write(lexer.TokenText(), lexer.TokenSize());
    }

    inline void PrettyPrinter::WriteEatToken() {
        lexer.writeTokenData(outText);
        //    outText.write(lexer.TokenText(), lexer.TokenSize());
        //    lexer.eatToken();
    }

    bool PrettyPrinter::ParseAttributes() {
        bool insertWhitespaceBeforeAttribute = false;
        int numAttribute = 0;   // count the attributes (this is used for attributes indentation)
        Token token;
        for (token = lexer.peekToken(); token != Token::InputEnd; token = lexer.peekToken()) {
            if (token == Token::Name || token == Token::Nmtoken) {
                if (insertWhitespaceBeforeAttribute) {
                    if (parms.indentAttributes && prevTag.size() > 0 && numAttribute > 0) {
                        AddNewline();
                        Indent();

                        for (size_t i = 0; i < prevTag.size() + 2; i++) {
                            outText.write(" ", 1);
                        }
                    }
                    else if (!indented && indentlevel > 0 && parms.insertIndents) {
                        Indent();
                    }
                    else {
                        outText.write(" ", 1);
                    }

                    insertWhitespaceBeforeAttribute = false;
                    ++numAttribute;
                }

                WriteEatToken();
            }
            else if (token == Token::Eq) {
                WriteEatToken();
                insertWhitespaceBeforeAttribute = false;
            }
            else if (token == Token::SystemLiteral) {
                WriteEatToken();
                insertWhitespaceBeforeAttribute = true;
            }
            else if (token == Token::Whitespace) {
                if (!parms.removeWhitespace) {
                    WriteToken();
                    insertWhitespaceBeforeAttribute = false;
                }
                else {
                    insertWhitespaceBeforeAttribute = true;
                }
                lexer.eatToken();
            }
            else if (token == Token::Linebreak) {
                insertWhitespaceBeforeAttribute = true;
                if (parms.keepExistingBreaks) {
                    WriteToken();
                    indented = false;
                }

                lexer.eatToken();
            }
            else if (token == Token::SelfClosingTagEnd) {
                break;
            }
            else if (token == Token::TagEnd) {
                break;
            }
            else {
                return false; // something unexpected happened but we definately were not able to eat/close the end >
            }
        }
        return false; //  token != Token::InputEnd;
    }

    void PrettyPrinter::trimTextAndOutput(std::string_view is) {
        auto len = is.size();
        auto start = is.data();
#define trimText__isWhiteSpace(c) (lexer.isWhitespace(c) || (!parms.keepExistingBreaks && lexer.isLinebreak(c)))

        bool prefixTrimmed = false;
        if (trimText__isWhiteSpace(*start)) {
            start++;
            len--;
            prefixTrimmed = true;
            while (len > 0 && trimText__isWhiteSpace(*start)) {
                start++;
                len--;
            }
        }

        bool postfixTrimmed = false;
        if (len > 0 && trimText__isWhiteSpace(start[len - 1])) {
            postfixTrimmed = true;
            len--;
            while (len > 0 && trimText__isWhiteSpace(start[len - 1])) {
                len--;
            }
        }

        if (len > 0) {
            TryIndent();

            if (parms.keepStartEndWhitespace) {
                if (prefixTrimmed)
                    outText.write(" ", 1);
            }
            if (parms.keepExistingBreaks)
                outText.write(start, len);
            else {
                auto end = start + len;
                while (start < end) {
                    auto pos = start;
                    while (pos < end && !trimText__isWhiteSpace(*pos)) {
                        pos++;
                    }

                    outText.write(start, pos - start);
                    if (pos < end) {
                        while (pos < end && trimText__isWhiteSpace(*pos)) pos++; // eat whitespace
                        outText.write(" ", 1); // only put 1 back
                    }

                    start = pos;
                }

                if (parms.keepStartEndWhitespace) {
                    if (postfixTrimmed) {
                        outText.write(" ", 1);
                    }
                }
            }
        }
    }

    void PrettyPrinter::Parse() {
        while (!lexer.Done()) {
            auto token = lexer.peekToken();
            switch (token) {
            case Token::CDSect:
            case Token::Comment: {
                StartNewElement();
                TryIndent();
                WriteEatToken();
                break;
            }
            case Token::Text: {
                TryCloseTag();
                if (parms.removeWhitespace) {
                    std::string tmp;
                    tmp.resize(lexer.tokenSize());
                    lexer.readTokenData(&tmp[0]);
                    trimTextAndOutput(tmp);
                }
                else {
                    TryIndent();
                    WriteToken();
                }
                lexer.eatToken();
                break;
            }
            case Token::Whitespace: {
                if (parms.removeWhitespace) {
                    if (inTag) { // attributes need to be separated
                        outText.write(" ", 1);
                    }
                }
                else {
                    WriteToken();
                    indented = true;
                }

                lexer.eatToken();
                break;
            }
            case Token::Linebreak: {
                TryCloseTag();
                if (!parms.insertNewLines) {
                    AddNewline();
                }
                else if (parms.keepExistingBreaks) {
                    WriteToken();
                    indented = false;
                }
                lexer.eatToken();
                break;
            }
            case Token::ClosingTag: {
                if (tagIsOpen && !parms.autocloseEmptyElements) {
                    TryCloseTag();
                }
                indentlevel--;
                lexer.eatToken();
                if (!lexer.isIdentifier(lexer.peekToken())) {
                    TryCloseTag();
                    outText.write("</", 2);
                    break; // back to default processing
                }
                std::string tag;
                lexer.readTokenData(tag);

                bool matchesPrevTag = prevTag == tag;
                if (tagIsOpen) {
                    if (parms.autocloseEmptyElements) {
                        if (matchesPrevTag) {
                            outText.write("/>", 2);
                            Token nextToken;
                            do {// not really charming but at least we will end up with valid XML.. in valid XML cases the first token would be a tag end
                                nextToken = lexer.peekToken();
                                lexer.eatToken();
                            } while (nextToken != Token::TagEnd && !lexer.Done());

                            prevTag = "";
                            tagIsOpen = false;
                            inTag = false;
                            indentlevel++; // correction for previous decrease
                            break;
                        }
                    }
                }
                else {
                    if (!matchesPrevTag) { // needs a new line
                        StartNewElement();
                        if (indented) {
                            AddNewline();
                        }
                        TryIndent();
                    }
                }

                outText.write("</", 2);
                outText.write(tag.c_str(), tag.size());

                bool ateWhitespace = false;
                for (token = lexer.peekToken(); token != Token::InputEnd; token = lexer.peekToken()) {
                    if (token == Token::Whitespace) {
                        lexer.eatToken();
                        ateWhitespace = true;
                    }
                    else if (token == Token::TagEnd) {
                        WriteEatToken();
                        break;
                    }
                    else if (token == Token::SelfClosingTagEnd ||
                        token == Token::ProcessingInstructionEnd) { // wtf?
                        WriteEatToken();
                        break;
                    }
                    else {
                        if (ateWhitespace) {
                            outText.write(" ", 1); // put it back
                        }
                        // unexpected ... whatever
                        break;
                    }
                }
                inTag = false;
                tagIsOpen = false;
                AddNewline();
                break;
            }
            case Token::ProcessingInstructionStart: {
                StartNewElement();
                if (indented) {
                    AddNewline();
                }
                TryIndent();
                WriteEatToken();
                tagIsOpen = true;
                inTag = true;

                if (lexer.isIdentifier(lexer.peekToken())) {
                    std::string tag;
                    lexer.readTokenData(tag);
                    outText.write(tag.c_str(), tag.size());

                    if (tag == "xml") {
                        ParseAttributes();

                        if (lexer.peekToken() != Token::ProcessingInstructionEnd) {
                            // well dont know... didnt close?
                        }
                        else {
                            WriteEatToken();
                        }
                    }
                    else {
                        lexer.readUntil("?>", true);
                        WriteEatToken();
                    }
                }
                else {
                    lexer.readUntil("?>", true);
                    WriteEatToken();
                }
                lexer.cancelInTag();
                inTag = false;
                tagIsOpen = false;
                AddNewline();

                break;
            }
            case Token::TagStart: {
                StartNewElement();
                if (indented) {
                    AddNewline();
                }
                TryIndent();
                WriteEatToken();
                tagIsOpen = true;
                inTag = true;

                if (lexer.isIdentifier(lexer.peekToken())) {
                    lexer.readTokenData(prevTag);
                    outText.write(prevTag.c_str(), prevTag.size());
                }
                else {
                    // ill-formed XML.. dump as is until next >
                    break;
                }
                ParseAttributes();

                inTag = false;
                break;
            }
            case Token::TagEnd: {
                lexer.eatToken();
                tagIsOpen = true;
                break;
            }
            case Token::SelfClosingTagEnd: {
                WriteEatToken();
                prevTag = "";
                AddNewline();
                inTag = false;
                tagIsOpen = false;
                break;
            }
            case Token::DeclStart: {
                StartNewElement();
                if (indented) {
                    AddNewline();
                }
                WriteEatToken();
                auto res = lexer.readUntilTagEndOrStart();
                if (parms.removeWhitespace) {
                    std::string tmp;
                    lexer.readTokenData(tmp);
                    trimTextAndOutput(tmp);
                }
                else {
                    TryIndent();
                    WriteToken();
                }
                lexer.eatToken();
                if (res) {
                    if (lexer.peekChar() == '<') {
                    }
                    else {
                        lexer.readChar();
                        outText.write(">", 1);
                    }
                    lexer.cancelInTag(); // always self-closing
                    inTag = false;
                }

                break;
            }
            case Token::InputEnd:
                break;

            default: {
                //WriteEatToken();
                throw std::exception("The pretty print parser encountered an unexpected error. This might be caused by invalid XML structure. Please try using another formating engine, for instance QuickXml, in pretty print options (go in XMLTools options dialog in order to change formating engine). If issue still happens, please get in touch with the developers @ https://github.com/morbac/XMLTools");
            }
            }
        }

        TryCloseTag();
    }

    bool PrettyPrinter::Convert() {
        inTag = false;
        indentlevel = 0;
        indented = false;
        tagIsOpen = false;
        prevTag = "";

        if (outText.tellp()) {
            return false; // single use class
        }

        Parse();

        return true;
    }
}