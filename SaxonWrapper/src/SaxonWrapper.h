#pragma once

#define SAXONC_EXPORT

#include "../../XmlWrapperInterface.h"
#include "saxonc/SaxonProcessor.h"

class SaxonWrapper : public XmlWrapperInterface {
	std::string data;
	void buildErrorsVector(SaxonApiException& exception, const wchar_t* szDesc = L"An unexpected error occurred");

public:
	SaxonWrapper(const char* xml, size_t size);
	~SaxonWrapper();

	void loadOptions();
	void saveOptions();

	int getCapabilities();
	bool checkSyntax();
	bool checkValidity(std::wstring schemaFilename = L"", std::wstring validationNamespace = L"");
	std::vector<XPathResultEntryType> xpathEvaluate(std::wstring xpath, std::wstring ns = L"");
	bool xslTransform(std::wstring xslfile, XSLTransformResultType* out, std::wstring options = L"", UniMode srcEncoding = UniMode::uniEnd);
};

