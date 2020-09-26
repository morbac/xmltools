#ifndef __XMLPARSER_H__
#define __XMLPARSER_H__

#include <sstream>

enum class XmlToken {
    EndOfFile,


    InputEnd,
    Whitespace,
    Linebreak,
    ProcessingInstructionStart,
    ProcessingInstructionEnd,
    DeclStart,
    TagStart,
    TagEnd,
    SelfClosingTagEnd,
    ClosingTag,
    Name,
    Nmtoken,
    Text,
    Comment,
    CDSect,
    Eq,
    SystemLiteral,
    Unknown,
    None
};


class XmlParser {
    char* cursor;
    size_t srcLength;

    std::stringstream outText;
    XmlToken token;

public:
    XmlParser(char* data);

    XmlToken nextToken();
    const char* getTokenName();
};

#endif /* __XMLPARSER_H__ */