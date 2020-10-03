#pragma once

#include <sstream>
#include <list>

namespace QuickXml {
    enum class XmlTokenType {
        TagOpening,        // <nx:sample
        TagClosing,        // </nx:sample
        TagOpeningEnd,     // > of opening tag
        TagClosingEnd,     // > of closing tag
        TagSelfClosingEnd, // /> of self closing tag
        AttrName,
        AttrValue,
        Text,
        Whitespace,
        Instruction,
        Comment,
        CDATA,
        LineBreak,
        Equal,

        EndOfFile,
        Undefined
    };

    struct XmlToken {
        XmlTokenType type;      // the token type
        const char* chars;      // a pointer to token chars
        size_t size;            // the token chars length
        size_t pos;             // the token position in stream
    };

    struct XmlContext {
        bool inOpeningTag;
        bool inClosingTag;
    };

    class XmlParser {
        // constant elements (they no vary after having been set)
        const char* srcText;        // pointer to original source text
        size_t srcLength;           // the original source text length

        // variying elements
        size_t currpos;             // the current position of the parser
        XmlContext currcontext;     // the actual parsing context
        bool hasAttrName;           // indicates that we got en attribute name

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
        * Search for given characters and return position of first occurrence
        * @param characters The substring to find in main stream
        * @return The position of substring or -1 if not found
        */
        size_t isIncoming(const char* characters);

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
        size_t readNextWord();

        /*
        * Reads stream (and update cursor position) until given delimiter
        * @param delimiter The string to search
        * @param offset The number of chars to skip before checking delimiter
        * @param goAfter Indicates to place cursor after the delimiter
        * @return Number of readen chars
        */
        size_t readUntil(const char* delimiter, size_t offset = 0, bool goAfter = false);

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