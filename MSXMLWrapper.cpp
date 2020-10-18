#include "MSXMLWrapper.h"
#include "MSXMLHelper.h"
#include "Report.h"
#include <comutil.h>

MSXMLWrapper::MSXMLWrapper() {
}

MSXMLWrapper::~MSXMLWrapper() {
    this->errors.clear();
}

int MSXMLWrapper::getCapabilities() {
	return ALL_OPTIONS;
}

void MSXMLWrapper::buildErrorsVector(IXMLDOMParseError* pXMLErr) {
    HRESULT hr = S_OK;
    IXMLDOMParseErrorCollection* pAllErrors = NULL;
    IXMLDOMParseError2* pTmpErr = NULL;
    long line = 0;
    long linepos = 0;
    long filepos = 0;
    BSTR bstrReason = NULL;
    long length = 0;

    this->resetErrors();

    CHK_HR(((IXMLDOMParseError2*)pXMLErr)->get_allErrors(&pAllErrors));
    if (pAllErrors != NULL) {

        CHK_HR(pAllErrors->get_length(&length));
        for (long i = 0; i < length; ++i) {
            CHK_HR(pAllErrors->get_next(&pTmpErr));
            CHK_HR(pAllErrors->get_item(i, &pTmpErr));

            CHK_HR(pXMLErr->get_line(&line));
            CHK_HR(pXMLErr->get_linepos(&linepos));
            CHK_HR(pXMLErr->get_filepos(&filepos));
            CHK_HR(pXMLErr->get_reason(&bstrReason));

            this->errors.push_back({
                line,
                linepos,
                filepos,
                std::wstring((wchar_t*)bstr_t(bstrReason))
            });

            SAFE_RELEASE(pTmpErr);
        }

        SAFE_RELEASE(pAllErrors);
    }

CleanUp:
    SAFE_RELEASE(pAllErrors);
    SAFE_RELEASE(pTmpErr);
}

bool MSXMLWrapper::checkSyntax(const char* xml, size_t size) {
    bool res = true;

    //updateProxyConfig();
    HRESULT hr = S_OK;
    IXMLDOMDocument2* pXMLDom = NULL;
    IXMLDOMParseError* pXMLErr = NULL;
    VARIANT_BOOL varStatus;
    VARIANT varCurrentData;

    this->errors.clear();

    Report::char2VARIANT(xml, &varCurrentData);

    CHK_HR(CreateAndInitDOM(&pXMLDom));
    CHK_HR(pXMLDom->load(varCurrentData, &varStatus));

    res = (varStatus == VARIANT_TRUE);

    if (!res) {
        CHK_HR(pXMLDom->get_parseError(&pXMLErr));
        this->buildErrorsVector(pXMLErr);
    }

CleanUp:
    SAFE_RELEASE(pXMLDom);
    SAFE_RELEASE(pXMLErr);
    VariantClear(&varCurrentData);

	return res;
}

bool MSXMLWrapper::checkValidity(const char* xml, size_t size) {
	return true;
}

std::vector<XPathResultEntryType> MSXMLWrapper::xpathEvaluate(const char* xml, size_t size, std::wstring xpath, std::wstring ns) {
	std::vector<XPathResultEntryType> res;
	return res;
}

std::string MSXMLWrapper::xslTransform(const char* xml, size_t xmllen, const char* xsl, size_t xsllen) {
	std::stringstream out;
	return out.str();
}

std::vector<ErrorEntryType> MSXMLWrapper::getLastErrors() {
	return this->errors;
}