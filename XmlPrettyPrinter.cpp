#include "StdAfx.h"
#include <sstream>

#include "XmlPrettyPrinter.h"

using namespace SimpleXml;

inline void XmlPrettyPrinter::WriteCloseTag() {
    outText.write(">", 1);
    tagIsOpen = false;
    inTag = false;
}

inline void XmlPrettyPrinter::TryCloseTag() {
    if (tagIsOpen) {
        WriteCloseTag();
        indentlevel++;
    }
}

inline void XmlPrettyPrinter::Indent() {
    if (!indented && parms.insertIndents) {
        for (int i = 0; i < indentlevel && i < parms.maxElementDepth; i++) {
            outText.write(parms.tab.c_str(), parms.tab.length());
        }
    }
    indented = true;
}

inline void XmlPrettyPrinter::TryIndent() {
    TryCloseTag();
    if (parms.insertNewLines) {
        Indent();
    }
}

inline void XmlPrettyPrinter::AddNewline() {
    if (parms.insertNewLines && !parms.keepExistingBreaks) {
        outText.write(parms.eol.c_str(), parms.eol.length());
        indented = false;
    }
}

inline void XmlPrettyPrinter::StartNewElement() {
    if (tagIsOpen) {
        TryCloseTag();
        AddNewline();
    }
    else if (indented) {
        AddNewline();
    }
}

inline void XmlPrettyPrinter::WriteToken() {
    outText.write(lexer.TokenText(), lexer.TokenSize());
}

inline void XmlPrettyPrinter::WriteEatToken() {
    outText.write(lexer.TokenText(), lexer.TokenSize());
    lexer.EatToken();
}

bool XmlPrettyPrinter::ParseAttributes() {
    bool insertWhitespaceBeforeAttribute = false;
    int numAttribute = 0;   // count the attributes (this is used for attributes indentation)
    Token token;
    for (token = lexer.peekToken(); token != Token::InputEnd; token = lexer.peekToken()) {
        if (token == Token::Name || token == Token::Nmtoken) {
            if (insertWhitespaceBeforeAttribute) {
                if (parms.indentAttributes && prevTagLen > 0 && numAttribute > 0) {
                    AddNewline();
                    if (indentlevel > 0) {
                        indentlevel--;  // the indent level is prepared for children, but we still are in the 
                                        // parent tag therefore we just have to decrease it for the indent
                        Indent();
                        indentlevel++;  // restore correct indent level
                    }
                    for (int i = 0; i < prevTagLen + 2; i++) {
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
            lexer.EatToken();
        }
        else if (token == Token::Linebreak) {
            insertWhitespaceBeforeAttribute = true;
            if (parms.keepExistingBreaks) {
                WriteToken();
                indented = false;
            }

            lexer.EatToken();
        }
        else if (token == Token::SelfClosingTagEnd) {
            break;
        }
        else if (token == Token::TagEnd) {
            break;
        }
        else {
            //inTag = false;
            //tagIsOpen = false;
            return false; // something unexpected happened but we definately were not able to eat/close the end >
        }
    }
    return false; //  token != Token::InputEnd;
}

void XmlPrettyPrinter::trimTextAndOutput(const InlineString& is) {
    auto len = is.size();
    auto start = is.text();
#define trimText__isWhiteSpace(c) (lexer.IsWhitespace(c) || (!parms.keepExistingBreaks && lexer.IsLinebreak(c)))

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

void XmlPrettyPrinter::Parse() {
    while (!lexer.Done()) {
        auto token = lexer.peek_token();
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
                auto s = lexer.currentLexeme().value;
                trimTextAndOutput(s);
            }
            else {
                TryIndent();
                WriteToken();
            }
            lexer.EatToken();
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

            lexer.EatToken();
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
            lexer.EatToken();
            break;
        }
        case Token::ClosingTag: {
            if (tagIsOpen && !parms.autocloseEmptyElements) {
                TryCloseTag();
            }
            indentlevel--;
            lexer.EatToken();
            if (!lexer.IsIdentifier(lexer.peek_token())) {
                TryCloseTag();
                outText.write("</", 2);
                break; // back to default processing
            }

            bool matchesPrevTag = prevTag != NULL && lexer.TokenSize() == prevTagLen && 0 == strncmp(prevTag, lexer.TokenText(), prevTagLen);
            if (tagIsOpen) {
                if (parms.autocloseEmptyElements) {
                    if (matchesPrevTag) {
                        outText.write("/>", 2);
                        lexer.EatToken(); // TAG NAME
                        Token nextToken;
                        do {// not really charming but at least we will end up with valid XML.. in valid XML cases the first token would be a tag end
                            nextToken = lexer.peek_token();
                            lexer.EatToken(); 
                        } while (nextToken != Token::TagEnd && !lexer.Done());

                        //AddNewline();
                        prevTag = NULL;
                        tagIsOpen = false;
                        inTag = false;
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
            WriteEatToken();

            bool ateWhitespace = false;
            for (token = lexer.peekToken(); token != Token::InputEnd; token = lexer.peekToken()) {
                if (token == Token::Whitespace) {
                    lexer.EatToken();
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

            if (lexer.IsIdentifier(lexer.peek_token())) {
                auto tag = lexer.currentLexeme().value;
                WriteEatToken();
                if (tag == "xml") {
                    indentlevel++; // indent attributes
                    ParseAttributes();
                    indentlevel--; // indent attributes
                }
            }
            lexer.readUntil("?>", true);
            WriteEatToken();
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

            if (lexer.IsIdentifier(lexer.peek_token())) {
                prevTag = lexer.TokenText();
                prevTagLen = lexer.TokenSize();
                WriteEatToken();
            }
            else {
                // ill-formed XML.. dump as is until next >
                break;
            }
            indentlevel++; // indent attributes
            ParseAttributes();
            indentlevel--; // indent attributes
            break;
        }
        case Token::TagEnd: {
            lexer.EatToken();
            tagIsOpen = true;
            break;
        }
        case Token::SelfClosingTagEnd: {
            WriteEatToken();
            prevTag = NULL;
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
                auto s = lexer.currentLexeme().value;
                trimTextAndOutput(s);
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
        default: {
            //WriteEatToken();
            throw std::exception("This should not happen.. please get in touch with the developers @ https://github.com/morbac/XMLTools");
        }
        }
    }

    TryCloseTag();
}

bool XmlPrettyPrinter::Convert() {
    inTag = false;
    indentlevel = 0;
    indented = false;
    tagIsOpen = false;
    prevTag = NULL;
    prevTagLen = 0;

    if (outText.tellp()) {
        return false; // single use class
    }

    Parse();

    return true;
}
