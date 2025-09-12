#pragma once

#include "XmlWrapperInterface.h"
//#include <libsaxon-hec-12.5.0.h>

class SaxonWrapper : public XmlWrapperInterface {
	CComBSTR m_sXml;

public:
	SaxonWrapper(const char* xml, size_t size);
	~SaxonWrapper();

	int getCapabilities();
	bool checkSyntax();
	bool checkValidity(std::wstring schemaFilename = L"", std::wstring validationNamespace = L"");
	std::vector<XPathResultEntryType> xpathEvaluate(std::wstring xpath, std::wstring ns = L"");
	bool xslTransform(std::wstring xslfile, XSLTransformResultType* out, std::wstring options = L"", UniMode srcEncoding = UniMode::uniEnd);
};

