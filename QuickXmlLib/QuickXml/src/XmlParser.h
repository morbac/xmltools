#pragma once

#include <sstream>
#include <list>

namespace QuickXml {
    enum XmlTokenType {
        Undefined         = 1 <<  0,

        TagOpening        = 1 <<  1, // <nx:sample
        TagClosing        = 1 <<  2, // </nx:sample
        TagOpeningEnd     = 1 <<  3, // > of opening tag
        TagClosingEnd     = 1 <<  4, // > of closing tag
        TagSelfClosingEnd = 1 <<  5, // /> of self closing tag
        AttrName          = 1 <<  6,
        AttrValue         = 1 <<  7,
        Text              = 1 <<  8,
        Whitespace        = 1 <<  9,
        Instruction       = 1 << 10, // <?..?> / <%..%>
        Declaration       = 1 << 11, // <!
        DeclarationEnd    = 1 << 12, // >
        Comment           = 1 << 13,
        CDATA             = 1 << 14,
        LineBreak         = 1 << 15,
        Equal             = 1 << 16,

        EndOfFile         = 1 << 30
    };

    typedef int XmlTokensType;  // combined tokens (ex: XmlTokenType::TagOpening | XmlTokenType::Declaration)

    struct XmlToken {
        XmlTokenType type;      // the token type
        const char* chars;      // a pointer to token chars
        size_t size;            // the token chars length
        size_t pos;             // the token position in stream
    };

    const XmlToken undefinedToken = {
        XmlTokenType::Undefined,
        "",
        0,
        0
    };

    struct XmlContext {
        bool inOpeningTag;
        bool inClosingTag;
        size_t declarationObjects;
    };

    class XmlParser {
        // constant elements (they no vary after having been set)
        const char* srcText;        // pointer to original source text
        size_t srcLength;           // the original source text length

        // variying elements
        size_t currpos;             // the current position of the parser
        XmlContext currcontext;     // the actual parsing context
        bool hasAttrName;           // indicates that we got en attribute name
        bool expectAttrValue;       // the paread read an = in tag, then it expects an attribute value

        XmlToken prevtoken;         // the previous token
        XmlToken currtoken;         // the current parsed token
        XmlToken nexttoken;         // the following token

        XmlToken fetchToken();

        // a queue of read tokens
        std::list<XmlToken> buffer;

    public:
        /*
        * Constructor
        * @param data The data to parse
        * @param length The data length
        */
        XmlParser(const char* data, size_t length);

        /*
        * Destructor
        */
        ~XmlParser();

        /*
        * Reset the parser settings
        */
        void reset();

        /*
        * Getters
        */
        XmlContext getXmlContext() { return this->currcontext; }
        XmlToken getPrevToken() { return this->prevtoken; }
        XmlToken getCurrToken() { return this->currtoken; }
        XmlToken getNextToken() { return this->nexttoken; }

        /*
        * Get the next non-text token
        * This function feed the tokens queue until it finds a structural token.
        * The queue is poped on next "parseNext()" calls
        * @return The next non-text token
        */
        XmlToken getNextStructureToken();

        /*
        * Fetch next token
        * @return The next recognized token
        */
        XmlToken parseNext();

        /*
        * Parse input until first token of given type
        * @type The type of tokens to fetch; multiple tokens can be passed using OR operator
        *       (ex. XmlTokenType::Declaration | XmlTokenType::TagOpening)
        * @return The found token. The EndOfFile token is returned if no occurrence
        *         could be found
        */
        XmlToken parseUntil(XmlTokensType type);

        /*
        * Reads some chars in main stream
        * @nchars The number of chars to read
        * @return The number of chars read (might be lower then parameter,
        *         especially when reaching the end of stream)
        */
        size_t readChars(size_t nchars = 1);

        /*
        * Reads the next word in main stream and update cursor position
        * @return The size of word
        */
        size_t readNextWord(bool skipQuotedStrings = false);

        /*
        * Reads stream (and update cursor position) until given delimiter
        * @param delimiter The string to search
        * @param offset The number of chars to skip before checking delimiter
        * @param goAfter Indicates to place cursor after the delimiter
        * @param skipDelimiter A delimiter which introduce a segment to ignore. Let's consider following example:
        *          <!DOCTYPE greeting [
                     <!ELEMENT greeting (#PCDATA)>
                   ]>
                 Reading until delimiter ">" with skipDelimiter "<" will skip the internal <!ELEMENT..>
        * @return Number of readen chars
        */
        size_t readUntil(const char* delimiter, size_t offset = 0, bool goAfter = false, std::string skipDelimiter = "");

        /*
        * Reads stream (and update cursor position) until it finds one of given characters
        * @param characters Set of characters to find
        * @param offset The number of chars to skip before checking characters
        * @param goAfter Indicates to place cursor after the delimiter
        * @return Number of readen chars
        */
        size_t readUntilFirstOf(const char* characters, size_t offset = 0, bool goAfter = false);

        /*
        * Reads stream (and update cursor position) until it finds any characters which differs from given characters
        * @param characters Set of characters to skip
        * @param offset The number of chars to skip before checking characters
        * @return Number of readen chars
        */
        size_t readUntilFirstNotOf(const char* characters, size_t offset = 0);

        /*
        * Gets the current token name (for debug)
        * @return A string representation of current token
        */
        std::string getTokenName();
    };
}