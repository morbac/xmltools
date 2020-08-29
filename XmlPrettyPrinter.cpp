#include "StdAfx.h"
#include <sstream>

#include "XmlPrettyPrinter.h"

inline void XmlPrettyPrinter::TryCloseTag() {
    if (tagIsOpen) {
        outText.write(">", 1);
        indentlevel++;
        tagIsOpen = false;
    }
}

inline void XmlPrettyPrinter::TryIndent() {
    TryCloseTag();
    if (parms.insertNewLines) {
        if (!indented && parms.insertIndents) {
            for (int i = 0; i < indentlevel && i < parms.maxElementDepth; i++) {
                outText.write(parms.tab.c_str(), parms.tab.length());
            }
        }
        indented = true;
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
}

inline void XmlPrettyPrinter::WriteToken() {
    outText.write(lexer.TokenText(), lexer.TokenSize());
}

inline void XmlPrettyPrinter::WriteEatToken() {
    outText.write(lexer.TokenText(), lexer.TokenSize());
    lexer.EatToken();
}

void XmlPrettyPrinter::Parse() {
    while (!lexer.Done()) {
        auto token = lexer.FindNext();
        switch (token) {
          case Token::CData:
          case Token::Comment: {
              StartNewElement();
              TryIndent();
              WriteEatToken();
              break;
          }
          case Token::Text: {
              TryCloseTag();
              if (parms.removeWhitespace) {
                  auto start = lexer.TokenText();
                  auto len = lexer.TokenSize();
                  while (len > 0 && lexer.IsWhitespace(*start)) {
                      len--;
                      start++;
                  }
                  while (len > 0 && lexer.IsWhitespace(start[len - 1])) {
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
              if (lexer.TryGetName() != Token::Name) {
                  TryCloseTag();
                  outText.write("</", 2);
                  break; // back to default processing
              }

              bool matchesPrevTag = prevTag != NULL && lexer.TokenSize() == prevTagLen && 0 == strncmp(prevTag, lexer.TokenText(), prevTagLen);
              if (tagIsOpen) {
                  if (parms.autocloseEmptyElements) {
                      if (matchesPrevTag) {
                          outText.write("/>", 2);
                          lexer.EatToken();
                          AddNewline();
                          prevTag = NULL;
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

              if (lexer.TryGetName() == Token::Name) {
                  WriteEatToken();
              }
              else {
                  // ill-formed XML.. dump as is until next >
                  break;
              }

              for (auto token = lexer.TryGetAttribute(); token != Token::InputEnd; token = lexer.TryGetAttribute()) {
                  if (token == Token::Text) {
                      if (parms.removeWhitespace) {
                          outText.write(" ", 1);
                      }

                      WriteEatToken();
                  }
                  else if (token == Token::Whitespace) {
                      if (!parms.removeWhitespace) {
                          WriteToken();
                      }
                      lexer.EatToken();
                  }
                  else if (token == Token::SelfClosingTagEnd || token == Token::TagEnd) {
                      WriteEatToken();
                      AddNewline();
                      break;
                  }
                  else break;
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

              if (lexer.TryGetName() == Token::Name) {
                  prevTag = lexer.TokenText();
                  prevTagLen = lexer.TokenSize();
                  WriteEatToken();
              }
              else {
                  // ill-formed XML.. dump as is until next >
                  break;
              }

              for (auto token = lexer.TryGetAttribute(); token != Token::InputEnd; token = lexer.TryGetAttribute()) {
                  if (token == Token::Text) {
                      if (parms.removeWhitespace) {
                          outText.write(" ", 1);
                      }

                      WriteEatToken();
                  }
                  else if (token == Token::Whitespace) {
                      if (!parms.removeWhitespace) {
                          WriteToken();
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
                  else break;
              }
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
              throw 0;
          }
        }
    }
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
