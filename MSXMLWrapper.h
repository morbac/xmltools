#pragma once

#include "XmlWrapperInterface.h"

class MSXMLWrapper : public XmlWrapperInterface {
	std::vector<ErrorEntryType> errors;

public:
	MSXMLWrapper();
	~MSXMLWrapper();

	int getCapabilities();
	bool checkSyntax(const char* xml, size_t size);
	bool checkValidity(const char* xml, size_t size);
	std::vector<XPathResultEntryType> xpathEvaluate(const char* xml, size_t size, std::wstring xpath, std::wstring ns = L"");
	std::string xslTransform(const char* xml, size_t xmllen, const char* xsl, size_t xsllen);
	std::vector<ErrorEntryType> getLastErrors();
};

