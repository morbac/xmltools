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
    Token token;
    for (token = lexer.TryGetAttribute(); token != Token::InputEnd; token = lexer.TryGetAttribute()) {
        if (token == Token::Text) {
            if (insertWhitespaceBeforeAttribute) {
                if (!indented && indentlevel > 0 && parms.insertIndents)
                    Indent();
                else
                    outText.write(" ", 1);

                insertWhitespaceBeforeAttribute = false;
            }

            WriteEatToken();
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
            WriteEatToken();
            inTag = false;
            tagIsOpen = false;
            AddNewline();
            break;
        }
        else if (token == Token::TagEnd) {
            inTag = false;
            lexer.EatToken();
            break;
        }
        else {
            inTag = false;
            tagIsOpen = false;
            return false; // something unexpected happened but we definately were not able to eat/close the end >
        }
    }
    return token != Token::InputEnd;
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
            //indentlevel++; // correct
            break;
        }
        case Token::Text: {
            TryCloseTag();
            if (parms.removeWhitespace) {
                auto start = lexer.TokenText();
                auto len = lexer.TokenSize();
                while (len > 0 && lexer.IsWhitespace(*start) || (!parms.keepExistingBreaks && lexer.IsLinebreak(*start))) {
                    len--;
                    start++;
                }
                while (len > 0 && lexer.IsWhitespace(start[len - 1] || (!parms.keepExistingBreaks && lexer.IsLinebreak(start[len - 1])))) {
                    len--;
                }
                if (len > 0) {
                    TryIndent();
                    outText.write(start, len);
                }
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

                        AddNewline();
                        prevTag = NULL;
                        tagIsOpen = false;
                        inTag = false;
                        break;
                    }
                }
            }
            else if (!matchesPrevTag) { // needs a new line
                StartNewElement();
                if (indented) {
                    AddNewline();
                }
                TryIndent();
            }

            outText.write("</", 2);
            WriteEatToken();

            bool ateWhitespace = false;
            for (auto token = lexer.TryGetAttribute(); token != Token::InputEnd; token = lexer.TryGetAttribute()) {
                if (token == Token::Whitespace) {
                    lexer.EatToken();
                    ateWhitespace = true;
                }
                else if (token == Token::TagEnd) {
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
                WriteEatToken();
            }
            else {
                // ill-formed XML.. dump as is until next >
                break;
            }

            indentlevel++; // indent attributes
            bool parseOk = ParseAttributes();
            indentlevel--; // indent attributes
            if (parseOk && tagIsOpen) { // the ? would be eaten as 'text'
                WriteCloseTag();
            }

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
