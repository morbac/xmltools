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

enum XmlCapabilityType {
    GET_ERROR_DETAILS = 1 << 0, // the wrapped api can return errors details
    CHECK_SYNTAX      = 1 << 1, // the wrapped api can perform syntax check
    CHECK_VALIDITY    = 1 << 2, // the wrapped api can perform validity check
    EVALUATE_XPATH    = 1 << 3, // the wrapped api can perform xpath evaluation
    XSL_TRANSFORM     = 1 << 4, // the wrapped api can perform xsl transformation

    ALL_OPTIONS       = GET_ERROR_DETAILS | CHECK_SYNTAX | CHECK_VALIDITY | EVALUATE_XPATH | XSL_TRANSFORM
};

/*
* This abstract class is the interface for external XML-API wrappers.
* The purpose of this interface is to define the expected methods for
* an XML-API wrapper.
*/

class XmlWrapperInterface {
    int capabilities = 0;

public:
    XmlWrapperInterface() {}
    virtual ~XmlWrapperInterface() {}

    /*
    * Inform about the interface capabilities
    * @return An integer composed of XmlCapabilityType elements
    */
    virtual int getCapabilities() = 0;

    /*
    * Perform syntax check
    * @param xml A pointer on the begin of xml buffer
    * @param size The xml buffer length
    * @return Return true if xml is well formed, otherwise false
    */
    virtual bool checkSyntax(const char* xml, size_t size) = 0;

    /*
    * Perform validity check
    * @param xml A pointer on the begin of xml buffer
    * @param size The xml buffer length
    * @return Return true if xml is valid, otherwise false
    */
    virtual bool checkValidity(const char* xml, size_t size) = 0;

    /*
    * Perform xpath evaluation
    * @param xml A pointer on the begin of xml buffer
    * @param size The xml buffer length
    * @param xpath The expression to be evaluated
    * @param ns An optional string descibing the required namespaces
    * @param A vector containing all evaluation occurrences
    */
    virtual std::vector<XPathResultEntryType> xpathEvaluate(const char* xml, size_t size, std::wstring xpath, std::wstring ns = L"") = 0;

    /*
    * Perform xsl transformation
    * @param xml A pointer on the begin of xml buffer
    * @param xmllen The xml buffer length
    * @param xml A pointer on the begin of xsl buffer
    * @param xsllen The xsl buffer length
    * @param A pointer to a string stream containing xsl transformation result
    */
    virtual std::stringstream* xslTransform(const char* xml, size_t xmllen, const char* xsl, size_t xsllen) = 0;

    /*
    * Get errors of last executed action
    * @return A vector of errors descriptions
    */
    virtual std::vector<ErrorEntryType> getLastErrors() = 0;


};