#include <sstream>

#include "SimpleXmlLexer.h"

struct PrettyPrintParms
{
    std::string eol;
    std::string tab;

    int maxElementDepth = 255;
    bool insertIndents = false;
    bool insertNewLines = false;
    bool removeWhitespace = false;
    bool autocloseEmptyElements = true;
};

class XmlPrettyPrinter {
    std::stringstream outText;
    SimpleXmlLexer lexer;
    PrettyPrintParms parms;

    bool inTag = false;
    int indentlevel = 0;
    bool indented = false;
    bool tagIsOpen = false;

    const char* prevTag = NULL;
    size_t prevTagLen = 0;

    inline void TryCloseTag();
    inline void TryIndent();
    inline void AddNewline();
    inline void StartNewElement();
    inline void WriteToken();
    inline void WriteEatToken();
    void Parse();

public:
    XmlPrettyPrinter(const char* txt, size_t textlen, PrettyPrintParms parms) :lexer(txt, textlen), parms(parms) {
    }

    std::stringstream* Stream() { return &outText; }

    bool Convert(); 
};
