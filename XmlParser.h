#pragma once

#include <sstream>

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

    CharEQ,

    EndOfFile,
    Unknown,
    None
};

#define WHITESPACE_CHARS " \t"
#define LINEBREAK_CHARS "\r\n"
#define DELIMITER_CHARS " \t\r\n=\"'<>"

struct XmlToken {
    XmlTokenType type;
    char* chars;
    size_t size;
};

class XmlParser {
    // constant elements (they no vary after having been set)
    char* srcText;          // pointer to original source text
    size_t srcLength;       // the original source text length

    // variying elements
    size_t curpos;          // the current position of the parser
    bool inTag;             // indicates we are inside an opening tag
    bool hasAttrName;       // indicates that we got en attribute name

    XmlToken token;

    XmlToken nextToken();

public:
    /*
    * Constructor
    * @param data The data to parse
    */
    XmlParser(char* data);

    /*
    * Reset the parser settings
    */
    void reset();

    bool isInTag() { return this->inTag; }

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
    * @param goAfter Indicates to place cursor after the delimiter
    * @return Number of readen chars
    */
    size_t readUntil(const char* delimiter, bool goAfter = false);

    /*
    * Reads stream (and update cursor position) until it finds one of given characters
    * @param characters Set of characters to find
    * @param goAfter Indicates to place cursor after the delimiter
    * @return Number of readen chars
    */
    size_t readUntilFirstOf(const char* characters, bool goAfter = false);

    /*
    * Reads stream (and update cursor position) until it finds any characters which differs from given characters
    * @param characters Set of characters to skip
    * @return Number of readen chars
    */
    size_t readUntilFirstNotOf(const char* characters);

    /*
    * Gets the current token name (for debug)
    * @return A string representation of current token
    */
    std::string getTokenName();
};