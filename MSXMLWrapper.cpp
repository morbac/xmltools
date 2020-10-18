#include "MSXMLWrapper.h"
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
    HRESULT hr = S_OK;
    IXMLDOMDocument2* pXMLDom = NULL;
    IXMLDOMNodeList* pNodes = NULL;
    IXMLDOMParseError* pXMLErr = NULL;
    VARIANT_BOOL varStatus;
    BSTR bstrXPath = NULL;
    IXMLDOMNode* pNode = NULL;
    DOMNodeType nodeType;
    BSTR bstrNodeName = NULL;
    BSTR bstrNodeType = NULL;
    VARIANT varNodeValue;
    VARIANT varXML;
    std::vector<XPathResultEntryType> res;
    std::wstring value;
    long length;

    Report::char2BSTR(xpath.c_str(), &bstrXPath);
    Report::char2VARIANT(xml, &varXML);

    CHK_ALLOC(bstrXPath);

    CHK_HR(CreateAndInitDOM(&pXMLDom));
    CHK_HR(pXMLDom->load(varXML, &varStatus));
    if (varStatus == VARIANT_TRUE) {
        CHK_HR(pXMLDom->setProperty(L"SelectionNamespaces", _variant_t(ns.c_str())));
        CHK_HR(pXMLDom->setProperty(L"SelectionLanguage", _variant_t(L"XPath")));
        hr = pXMLDom->selectNodes(bstrXPath, &pNodes);
        if (FAILED(hr)) {
            CHK_HR(pXMLDom->get_parseError(&pXMLErr));
            this->buildErrorsVector(pXMLErr);

            this->errors.push_back({
                0,
                0,
                0,
                L"Error: error on XPath expression."
            });

            goto CleanUp;
        }

        // create result

        V_VT(&varNodeValue) = VT_UNKNOWN;
        CHK_HR(pNodes->get_length(&length));
        for (long i = 0; i < length; ++i) {
            CHK_HR(pNodes->get_item(i, &pNode));

            CHK_HR(pNode->get_nodeType(&nodeType));
            CHK_HR(pNode->get_nodeName(&bstrNodeName));
            CHK_HR(pNode->get_nodeValue(&varNodeValue));
            CHK_HR(pNode->get_nodeTypeString(&bstrNodeType));

            switch (nodeType) {
                case NODE_ELEMENT: {
                    // let's concatenate all direct text children
                    // rem: pNode->get_text(&bstrNodeValue) is not appropriate
                    //      because it concatenates the node content and the
                    //      content of its descendants; in our case we only
                    //      want direct child content
                    IXMLDOMNodeList* pNodeList;
                    // @fixme: would it be better to use CComPtr<IXMLDOMNodeList> pNodeList; ?
                    long numChildren;
                    value = L"";

                    CHK_HR(pNode->get_childNodes(&pNodeList));
                    CHK_HR(pNodeList->get_length(&numChildren));
                    for (long j = 0; j < numChildren; ++j) {
                        SAFE_RELEASE(pNode);
                        CHK_HR(pNodeList->get_item(j, &pNode));
                        CHK_HR(pNode->get_nodeType(&nodeType));
                        if (nodeType == NODE_TEXT) {
                            VariantClear(&varNodeValue);
                            CHK_HR(pNode->get_nodeValue(&varNodeValue));
                            value += varNodeValue.bstrVal;
                        }
                        SAFE_RELEASE(pNode);
                    }
                    SAFE_RELEASE(pNodeList);
                    break;
                }
                default: {
                    value = varNodeValue.bstrVal;
                }
            }

            res.push_back({
                bstrNodeType,
                bstrNodeName,
                value
            });

            SysFreeString(bstrNodeName);
            SysFreeString(bstrNodeType);
            VariantClear(&varNodeValue);
            SAFE_RELEASE(pNode);
        }
    }
    else {
        CHK_HR(pXMLDom->get_parseError(&pXMLErr));
        this->buildErrorsVector(pXMLErr);
    }
CleanUp:
    SAFE_RELEASE(pXMLDom);
    SAFE_RELEASE(pNodes);
    SAFE_RELEASE(pXMLErr);
    VariantClear(&varXML);
    SysFreeString(bstrXPath);

    SAFE_RELEASE(pNode);
    SysFreeString(bstrNodeName);
    SysFreeString(bstrNodeType);
    VariantClear(&varNodeValue);

	return res;
}

std::string MSXMLWrapper::xslTransform(const char* xml, size_t xmllen, const char* xsl, size_t xsllen) {
	std::stringstream out;
	return out.str();
}

std::vector<ErrorEntryType> MSXMLWrapper::getLastErrors() {
	return this->errors;
}