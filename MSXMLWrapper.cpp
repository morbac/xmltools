#include "StdAfx.h"

#include "MSXMLWrapper.h"
#include "PluginInterface.h"
#include "Report.h"
#include <comutil.h>

MSXMLWrapper::MSXMLWrapper() {
    std::map<std::string, std::string> tristate{ { "-1", "default" }, { "0", "false" }, { "1", "true" } };

    this->options["allowDocumentFunction"] = {
        L"Allow document function",
        L"",
        L"-1",
        OptionDataType::OPTION_TYPE_INTEGER,
        OptionFormatType::OPTION_FORMAT_COMBO,
        tristate
    };
    this->options["allowXsltScript"] = {
        L"Allow Xslt Script",
        L"",
        L"-1",
        OptionDataType::OPTION_TYPE_INTEGER,
        OptionFormatType::OPTION_FORMAT_COMBO,
        tristate
    };
}

MSXMLWrapper::~MSXMLWrapper() {
    this->resetErrors();
}

int MSXMLWrapper::getCapabilities() {
    return XmlCapabilityType::ALL_OPTIONS;
}

void MSXMLWrapper::addErrorToVector(IXMLDOMParseError2* pXMLErr, const wchar_t* szDesc) {
    HRESULT hr = S_OK; 
    long line = 0;
    long linepos = 0;
    long filepos = 0;
    BSTR bstrReason = NULL;
    ErrorEntryType err;

    if (pXMLErr != NULL) {
        CHK_HR(pXMLErr->get_line(&line));
        CHK_HR(pXMLErr->get_linepos(&linepos));
        CHK_HR(pXMLErr->get_filepos(&filepos));
        CHK_HR(pXMLErr->get_reason(&bstrReason));

        if (line == 0 || linepos == 0) {
            line = 1;
            linepos = 1;
        }

        if (line >= 0 && linepos >= 0 && filepos >= 0) {
            err.positioned = TRUE;
            err.line = (size_t) line;
            err.linepos = (size_t)linepos;
            err.filepos = (size_t)filepos + 1;
        }
        else {
            err.positioned = FALSE;
            err.line = 0;
            err.linepos = 0;
            err.filepos = 0;
        }
        err.reason = std::wstring((wchar_t*)bstr_t(bstrReason));
        
        this->errors.push_back(err);
    }

CleanUp:
    SysFreeString(bstrReason);
}

void MSXMLWrapper::buildErrorsVector(IXMLDOMParseError2* pXMLErr, const wchar_t* szDesc) {
    HRESULT hr = S_OK;
    IXMLDOMParseErrorCollection* pAllErrors = NULL;
    IXMLDOMParseError2* pTmpErr = NULL;
    long length = 0;

    this->resetErrors();

    try {
        CHK_HR(pXMLErr->get_allErrors(&pAllErrors));
        if (pAllErrors != NULL) {
            CHK_HR(pAllErrors->get_length(&length));
            for (long i = 0; i < length; ++i) {
                CHK_HR(pAllErrors->get_next(&pTmpErr));
                CHK_HR(pAllErrors->get_item(i, &pTmpErr));
                this->addErrorToVector(pTmpErr);
                SAFE_RELEASE(pTmpErr);
            }

            SAFE_RELEASE(pAllErrors);
        }
        else {
            this->addErrorToVector(pXMLErr, szDesc);
        }
    }
    catch (...) {
        this->addErrorToVector(pXMLErr, szDesc);
    }

CleanUp:
    SAFE_RELEASE(pAllErrors);
    SAFE_RELEASE(pTmpErr);
}

bool MSXMLWrapper::checkSyntax(const char* xml, size_t size) {
    bool res = true;

    //updateProxyConfig();
    HRESULT hr = S_OK;
    IXMLDOMDocument3* pXMLDom = NULL;
    IXMLDOMParseError2* pXMLErr = NULL;
    VARIANT_BOOL varStatus;
    CComBSTR bstrXML;

    this->resetErrors();

    Report::char2BSTR(xml, &bstrXML);
    CHK_ALLOC(bstrXML);

    CHK_HR(CreateAndInitDOM(&pXMLDom));
    CHK_HR(pXMLDom->loadXML(bstrXML, &varStatus));

    res = (varStatus == VARIANT_TRUE);

    if (!res) {
        CHK_HR(pXMLDom->get_parseError((IXMLDOMParseError**)&pXMLErr));
        this->buildErrorsVector(pXMLErr);
    }

CleanUp:
    SAFE_RELEASE(pXMLDom);
    SAFE_RELEASE(pXMLErr);
    bstrXML.Empty();

	return res;
}

bool MSXMLWrapper::checkValidity(const char* xml, size_t size, std::wstring schemaFilename, std::wstring validationNamespace) {
    // source: https://docs.microsoft.com/en-us/previous-versions/windows/desktop/ms754649(v=vs.85)

    HRESULT hr = S_OK;
    IXMLDOMDocument3 *pXMLDom = NULL,  // The MultipleErrorMessages property is supported by IXMLDOMDocument3, together with
                     *pXSDDom = NULL;  // IXMLDOMParseError2 and IXMLDOMParseErrorCollection interfaces introduced in MSXML 5.0
    IXMLDOMParseError2* pXMLErr = NULL;
    IXMLDOMNodeList* pNodes = NULL;
    IXMLDOMNode* pNode = NULL;
    IXMLDOMElement* pElement = NULL;
    IXMLDOMSchemaCollection2* pXS = NULL;
    VARIANT_BOOL varStatus;
    CComBSTR bstrXML;
    BSTR bstrNodeName = NULL;
    bool res = true;

    this->resetErrors();

    Report::char2BSTR(xml, &bstrXML);
    CHK_ALLOC(bstrXML);

    CHK_HR(CreateAndInitDOM(&pXMLDom, (INIT_OPTION_VALIDATEONPARSE | INIT_OPTION_RESOLVEEXTERNALS)));
    CHK_HR(pXMLDom->loadXML(bstrXML, &varStatus));

    if (varStatus == VARIANT_TRUE) {
        if (!schemaFilename.empty()) {
            CHK_HR(CreateAndInitDOM(&pXSDDom, (INIT_OPTION_VALIDATEONPARSE | INIT_OPTION_RESOLVEEXTERNALS)));
            CHK_HR(pXSDDom->load(CComVariant(schemaFilename.c_str()), &varStatus));
            if (varStatus == VARIANT_TRUE) {
                CHK_HR(CreateAndInitSchema(&pXS));
                hr = pXS->add(_bstr_t(validationNamespace.c_str()), CComVariant(schemaFilename.c_str()));
                if (SUCCEEDED(hr)) {
                    // Create a DOMDocument and set its properties.
                    SAFE_RELEASE(pXMLDom);
                    CHK_HR(CreateAndInitDOM(&pXMLDom, (INIT_OPTION_VALIDATEONPARSE | INIT_OPTION_RESOLVEEXTERNALS)));
                    CHK_HR(pXMLDom->putref_schemas(CComVariant(pXS)));

                    /*
                    pXMLDom->put_async(VARIANT_FALSE);
                    pXMLDom->put_validateOnParse(VARIANT_TRUE);
                    pXMLDom->put_resolveExternals(VARIANT_TRUE);
                    */

                    CHK_HR(pXMLDom->loadXML(bstrXML, &varStatus));
                    if (varStatus != VARIANT_TRUE) {
                        CHK_HR(pXMLDom->get_parseError((IXMLDOMParseError**)&pXMLErr));
                        this->buildErrorsVector(pXMLErr);
                        res = false;
                    }
                }
                else {
                    this->errors.push_back({
                        FALSE,
                        0,
                        0,
                        0,
                        L"Invalid schema or missing namespace."
                    });
                    res = false;
                }
            }
            else {
                this->errors.push_back({
                        FALSE,
                        0,
                        0,
                        0,
                        L"The referenced schema is detected as being invalid. Please fix it before using it as validation schema."
                    });
                res = false;
            }
        }

        if (res) {
            // If we are here, this means that noNamespaceSchemaLocation or schemaLocation attribute is present.
            // So validation is supposed OK since xml is loaded with INIT_OPTION_VALIDATEONPARSE option. Then we
            // just have to test validity.
            if (pXMLDom->validate((IXMLDOMParseError**)&pXMLErr) == S_FALSE) {
                this->buildErrorsVector(pXMLErr);
                res = false;
            }
        }
    }
    else {
        CHK_HR(pXMLDom->get_parseError((IXMLDOMParseError**)&pXMLErr));
        this->buildErrorsVector(pXMLErr);
        res = false;
    }

CleanUp:
    SAFE_RELEASE(pXMLDom);
    SAFE_RELEASE(pXSDDom);
    SAFE_RELEASE(pXMLErr);
    SAFE_RELEASE(pNodes);
    SAFE_RELEASE(pNode);
    SAFE_RELEASE(pElement);
    SAFE_RELEASE(pXS);
    SysFreeString(bstrNodeName);
    bstrXML.Empty();

	return res;
}

std::vector<XPathResultEntryType> MSXMLWrapper::xpathEvaluate(const char* xml, size_t size, std::wstring xpath, std::wstring ns) {
    HRESULT hr = S_OK;
    IXMLDOMDocument3* pXMLDom = NULL;
    IXMLDOMNodeList* pNodes = NULL;
    IXMLDOMParseError2* pXMLErr = NULL;
    VARIANT_BOOL varStatus;
    IXMLDOMNode* pNode = NULL;
    DOMNodeType nodeType;
    BSTR bstrNodeName = NULL;
    BSTR bstrNodeType = NULL;
    CComBSTR bstrXPath;
    CComBSTR bstrXML;
    VARIANT varNodeValue;
    std::vector<XPathResultEntryType> res;
    std::wstring value;
    long length;

    this->resetErrors();

    UniMode encoding = Report::getEncoding(nppData._nppHandle);
    Report::char2BSTR(xpath.c_str(), &bstrXPath);
    CHK_ALLOC(bstrXPath);

    Report::char2BSTR(xml, &bstrXML);
    CHK_ALLOC(bstrXML);

    CHK_HR(CreateAndInitDOM(&pXMLDom));
    CHK_HR(pXMLDom->loadXML(bstrXML, &varStatus));
    if (varStatus == VARIANT_TRUE) {
        CHK_HR(pXMLDom->setProperty(L"SelectionNamespaces", _variant_t(ns.c_str())));
        CHK_HR(pXMLDom->setProperty(L"SelectionLanguage", _variant_t(L"XPath")));
        hr = pXMLDom->selectNodes(bstrXPath, &pNodes);
        if (FAILED(hr)) {
            CHK_HR(pXMLDom->get_parseError((IXMLDOMParseError**)&pXMLErr));
            this->buildErrorsVector(pXMLErr);

            this->errors.push_back({
                FALSE,
                0,
                0,
                0,
                L"Error: error on XPath expression, or missing namespace definition."
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
                            if (varNodeValue.vt != VT_NULL) {
                                value += (wchar_t*)_bstr_t(varNodeValue);
                            }
                        }
                        SAFE_RELEASE(pNode);
                    }
                    SAFE_RELEASE(pNodeList);
                    break;
                }
                default: {
                    if (varNodeValue.vt != VT_NULL) {
                        value = (wchar_t*)_bstr_t(varNodeValue);
                    }
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
        CHK_HR(pXMLDom->get_parseError((IXMLDOMParseError**)&pXMLErr));
        this->buildErrorsVector(pXMLErr);
    }
CleanUp:
    SAFE_RELEASE(pXMLDom);
    SAFE_RELEASE(pNodes);
    SAFE_RELEASE(pXMLErr);
    bstrXPath.Empty();
    bstrXML.Empty();

    SAFE_RELEASE(pNode);
    SysFreeString(bstrNodeName);
    SysFreeString(bstrNodeType);

	return res;
}

/*
  Get next key/val pair in string, starting from given position. The function
  returns the position where parser stoped, or -1 (std::string::npos) when no
  pair was found. The 'key' and 'val' variables are feeded with readen key and
  value.
  @todo: rewrite this method and use regex
*/
std::string::size_type getNextParam(std::wstring& str, std::string::size_type startpos, std::wstring* key, std::wstring* val) {
    std::string::size_type len = str.length();
    if (startpos == std::string::npos || startpos >= len || !key || !val) return std::string::npos;

    // skip spaces, tabs and carriage returns
    std::string::size_type keypos = str.find_first_not_of(L" \t\r\n", startpos);
    if (keypos == std::string::npos || keypos >= len) return std::string::npos;

    // next char shouldn't be a '='
    if (str.at(keypos) == '=') return std::string::npos;

    // keypos points on begin of the key; let's search for next '=' or ' '
    std::string::size_type valpos = str.find_first_of(L"=", keypos + 1);
    valpos = str.find_last_not_of(L" =", valpos);  // get last char of the key
    *key = str.substr(keypos, valpos - keypos + 1);

    // skip the '='
    valpos = 1 + str.find_first_of(L"=", valpos + 1);

    if (str.at(valpos) == ' ') valpos = str.find_first_not_of(L" ", valpos);  // skip eventual space chars
    if (valpos < 0 || valpos >= len) return std::string::npos;

    // here we must parse the string; if it starts with an apostroph, let's search
    // the next apostroph; otherwise let's read the next word
    std::string::size_type valendpos = valpos;
    if (str.at(valendpos) == '\'') {
        valendpos = str.find_first_of(L"\'", valendpos + 1);
        *val = str.substr(valpos + 1, valendpos - valpos - 1);  // get value without apostrophs
    }
    else {
        valendpos = str.find_first_of(L" \t\r\n", valendpos);
        // at the end of the string, valendpos = -1
        if (valendpos == std::string::npos) valendpos = len;
        *val = str.substr(valpos, valendpos - valpos);
    }

    return valendpos;
}

bool MSXMLWrapper::xslTransform(const char* xml, size_t xmllen, std::wstring xslfile, XSLTransformResultType* out, std::wstring options, UniMode srcEncoding) {
    // inspired from https://www.codeguru.com/cpp/data/data-misc/xml/article.php/c4565/Doing-XSLT-with-MSXML-in-C.htm
    // and msxsl tool source code (https://www.microsoft.com/en-us/download/details.aspx?id=21714)
    HRESULT hr = S_OK;
    HGLOBAL hg = NULL;
    IXMLDOMDocument3* pXml = NULL;
    IXMLDOMDocument3* pXslt = NULL;
    IXSLTemplate* pTemplate = NULL;
    IXSLProcessor* pProcessor = NULL;
    IXMLDOMParseError2* pXMLErr = NULL;
    IXMLDOMNodeList* pNodes = NULL;
    IXMLDOMNode* pNode = NULL;
    IStream* pOutStream = NULL;
    VARIANT varValue;
    VARIANT_BOOL varStatus;
    BSTR bstrEncoding = NULL;
    CComBSTR bstrXML;
    long length;
    bool currentDataIsXml = true;
    bool outputAsStream = false;
    bool res = true;

    this->resetErrors();

    V_VT(&varValue) = VT_UNKNOWN;

    Report::char2BSTR(xml, &bstrXML);
    CHK_ALLOC(bstrXML);

    // active document may either be XML or XSL; if XSL,
    // then m_sSelectedFile refers to an XML file
    CHK_HR(CreateAndInitDOM(&pXml));
    CHK_HR(pXml->loadXML(bstrXML, &varStatus));
    if (varStatus == VARIANT_TRUE) {
        CHK_HR(pXml->setProperty(L"SelectionNamespaces", variant_t(L"xmlns:xsl=\"http://www.w3.org/1999/XSL/Transform\"")));
        if (SUCCEEDED(pXml->selectNodes(L"/xsl:stylesheet", &pNodes))) {
            CHK_HR(pNodes->get_length(&length));
            if (length == 1) {
                // the active document is an XSL one; let's invert both files
                currentDataIsXml = false;
            }
        }
    }
    else {
        CHK_HR(pXml->get_parseError((IXMLDOMParseError**)&pXMLErr));
        this->buildErrorsVector(pXMLErr);

        if (this->errors.size() == 0) {
            this->errors.push_back({
                FALSE,
                0,
                0,
                0,
                L"An error occurred during current source loading. Please check source validity. Transformation aborted."
             });
        }
        res = false;
    }
    SAFE_RELEASE(pXml);
    SAFE_RELEASE(pNodes);

    if (!res) goto CleanUp;

    // load xml
    CHK_HR(CreateAndInitDOM(&pXml));
    if (currentDataIsXml) {
        CHK_HR(pXml->loadXML(bstrXML, &varStatus));
    }
    else {
        CHK_HR(pXml->load(_variant_t(xslfile.c_str()), &varStatus));
    }
    if (varStatus == VARIANT_TRUE) {
        // load xsl
        CHK_HR(CreateAndInitDOM(&pXslt, (INIT_OPTION_PRESERVEWHITESPACE | INIT_OPTION_FREETHREADED)));
        if (currentDataIsXml) {
            CHK_HR(pXslt->load(_variant_t(xslfile.c_str()), &varStatus));
        }
        else {
            CHK_HR(pXslt->loadXML(bstrXML, &varStatus));
        }
        if (varStatus == VARIANT_TRUE) {
            // detect output encoding
            CHK_HR(pXslt->setProperty(L"SelectionNamespaces", variant_t(L"xmlns:xsl=\"http://www.w3.org/1999/XSL/Transform\"")));
            if (SUCCEEDED(pXslt->selectNodes(L"/xsl:stylesheet/xsl:output/@encoding", &pNodes))) {
                CHK_HR(pNodes->get_length(&length));
                if (length == 1) {
                    // get encoding from output declaration
                    pNodes->get_item(0, &pNode);
                    pNode->get_text(&bstrEncoding);
                    out->encoding = Report::getEncoding(bstrEncoding);
                    SAFE_RELEASE(pNode);
                    SAFE_RELEASE(pNodes);

                    outputAsStream = TRUE;
                }
                else {
                    // get encoding of source file
                    out->encoding = srcEncoding;
                    outputAsStream = FALSE;
                }
            }
            CHK_HR(pXslt->setProperty(L"SelectionNamespaces", variant_t(L"")));

            // build template
            CHK_HR(CreateAndInitXSLTemplate(&pTemplate));
            if (SUCCEEDED(pTemplate->putref_stylesheet(pXslt))) {
                CHK_HR(pTemplate->createProcessor(&pProcessor));

                // set startMode
                // @todo

                // set parameters; let's decode params string; the string should have the following form:
                //   variable1=value1;variable2=value2;variable3="value 3"
                std::string::size_type i = std::string::npos;
                std::wstring key, val;
                while ((i = getNextParam(options, i + 1, &key, &val)) != std::string::npos) {
                    _bstr_t var0(key.c_str());
                    VARIANT var1;
                    VariantInit(&var1);
                    CHK_HR(VariantFromString(val.c_str(), var1));
                    CHK_HR(pProcessor->addParameter(var0, var1));
                    VariantClear(&var1);
                }

                // attach to processor XML file we want to transform,
                // add one parameter, maxprice, with a value of 35, and
                // do the transformation
                CHK_HR(pProcessor->put_input(_variant_t(pXml)));

                // method 1 -----------------------------------------------------------
                if (outputAsStream) {
                    // prepare Stream object to store results of transformation,
                    // and set processor output to it
                    CHK_HR(CreateStreamOnHGlobal(0, TRUE, &pOutStream));
                    V_VT(&varValue) = VT_UNKNOWN;
                    V_UNKNOWN(&varValue) = (IUnknown*)pOutStream;
                    CHK_HR(pProcessor->put_output(varValue));
                    VariantClear(&varValue);
                }

                // transform
                CHK_HR(pProcessor->transform(&varStatus));
                if (varStatus == VARIANT_TRUE) {
                    // get results of transformation and send them to a new NPP document
                    if (outputAsStream) {
                        CHK_HR(pOutStream->Write((void const*)"\0", 1, 0));
                        CHK_HR(GetHGlobalFromStream(pOutStream, &hg));
                        char* tmp = (char*)GlobalLock(hg);
                        if (tmp != NULL) {
                            out->data = tmp;
                        }
                        else {
                            this->errors.push_back({
                                FALSE,
                                0,
                                0,
                                0,
                                L"An unexpected error occurred during XSL transformation"
                            });
                            res = false;
                        }
                    }
                    else {
                        pProcessor->get_output(&varValue);
                        out->data = _com_util::ConvertBSTRToString(_bstr_t(varValue));
                        VariantClear(&varValue);
                    }

                    if (outputAsStream) {
                        GlobalUnlock(hg);
                    }
                }
                else {
                    this->errors.push_back({
                        FALSE,
                        0,
                        0,
                        0,
                        L"An error occurred during XSL transformation"
                    });
                    res = false;
                }
            }
            else {
                this->errors.push_back({
                    FALSE,
                    0,
                    0,
                    0,
                    L"The XSL stylesheet is not valid. Transformation aborted."
                });
                res = false;
            }
        }
        else {
            CHK_HR(pXslt->get_parseError((IXMLDOMParseError**)&pXMLErr));
            this->buildErrorsVector(pXMLErr);
            res = false;
        }
    }
    else {
        CHK_HR(pXml->get_parseError((IXMLDOMParseError**)&pXMLErr));
        this->buildErrorsVector(pXMLErr);

        if (this->errors.size() == 0) {
            if (currentDataIsXml) {
                this->errors.push_back({
                    FALSE,
                    0,
                    0,
                    0,
                    L"An error occurred during XSL loading. Please check XSL validity. Transformation aborted."
                });
            }
            else {
                this->errors.push_back({
                    FALSE,
                    0,
                    0,
                    0,
                    L"An error occurred during XML loading. Please check XML validity. Transformation aborted."
                });
            }
        }
        res = false;
    }

CleanUp:
    SAFE_RELEASE(pXml);
    SAFE_RELEASE(pXslt);
    SAFE_RELEASE(pTemplate);
    SAFE_RELEASE(pProcessor);
    SAFE_RELEASE(pXMLErr);
    SAFE_RELEASE(pOutStream);
    SAFE_RELEASE(pNodes);
    SAFE_RELEASE(pNode);
    SysFreeString(bstrEncoding);
    bstrXML.Empty();

    return res;
}