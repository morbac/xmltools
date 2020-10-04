#pragma once

#include <sstream>
#include <vector>

// WORK IN PROGRESS

struct XPathResultEntryType {
    std::string type;
    std::string name;
    std::string value;
};

struct ErrorEntryType {
    size_t line;        // 0-based line number having the error
    size_t linepos;     // 0-based position in line
    size_t filepos;     // 0-based position in whole stream
    std::string reason; // error description
};

/*
* This abstract class is the interface for external XML-API wrappers.
* The purpose of this interface is to define the expected methods for
* an XML-API wrapper.
*/

class XmlWrapperInterface {
public:
    XmlWrapperInterface() {}
    virtual ~XmlWrapperInterface() {}

    virtual bool checkSyntax(const char* xml, size_t size) = 0;
    virtual bool checkValidity(const char* xml, size_t size) = 0;
    virtual std::vector<XPathResultEntryType> xpathEvaluate(const char* xml, size_t size, std::wstring xpath, std::wstring ns) = 0;
    virtual std::stringstream* xslTransform(const char* xml, size_t xmllen, const char* xsl, size_t xsllen) = 0;
    virtual std::vector<ErrorEntryType> getLastErrors() = 0;
};