#include "StdAfx.h"
#include "Scintilla.h"
#include "PluginInterface.h"
#include "nppHelpers.h"
#include "Report.h"
#include "XMLTools.h"
#include "XmlParser.h"

#include "SelectFileDlg.h"

#include "MSXMLWrapper.h"

#include <string>

using namespace QuickXml;

int performXMLCheck(int informIfNoError) {
    dbgln("performXMLCheck()");

    // 0. change current folder
    TCHAR currenPath[MAX_PATH] = { '\0' };
    ::SendMessage(nppData._nppHandle, NPPM_GETCURRENTDIRECTORY, MAX_PATH, (LPARAM)currenPath);
    if (_wchdir(currenPath)) {
        dbg(L"Unable to change directory to ");
        dbgln(currenPath);
    }

    int currentEdit, currentLength, res = 0;
    ::SendMessage(nppData._nppHandle, NPPM_GETCURRENTSCINTILLA, 0, (LPARAM)&currentEdit);
    HWND hCurrentEditView = getCurrentHScintilla(currentEdit);

    clearAnnotations(hCurrentEditView);

    currentLength = (int) ::SendMessage(hCurrentEditView, SCI_GETLENGTH, 0, 0);

    char* data = new char[currentLength + sizeof(char)];
    if (!data) return -1;  // allocation error, abort check
    memset(data, '\0', currentLength + sizeof(char));

    ::SendMessage(hCurrentEditView, SCI_GETTEXT, currentLength + sizeof(char), reinterpret_cast<LPARAM>(data));

    auto t_start = clock();

    MSXMLWrapper msxml;
    bool isok = msxml.checkSyntax(data, currentLength);

    auto t_end = clock();
    dbgln(L"crunch time: " + std::to_wstring(t_end - t_start) + L" ms", DBG_LEVEL::DBG_INFO);

    if (isok) {
        if (informIfNoError) {
            Report::_printf_inf(L"No error detected.");
        }
    }
    else {
        displayXMLErrors(msxml.getLastErrors(), hCurrentEditView, L"XML Parsing error");
    }

    return res;
}

void autoXMLCheck() {
    dbgln("autoXMLCheck()");

    performXMLCheck(0);
}

void manualXMLCheck() {
    dbgln("manualXMLCheck()");

    performXMLCheck(1);
}

///////////////////////////////////////////////////////////////////////////////

CSelectFileDlg* pSelectFileDlg = NULL;
void XMLValidation(int informIfNoError) {
    dbgln("XMLValidation()");

    HRESULT hr = S_OK;
    IXMLDOMDocument2* pXMLDom = NULL;
    IXMLDOMParseError* pXMLErr = NULL;
    IXMLDOMParseError2* pXMLErr2 = NULL;
    IXMLDOMNodeList* pNodes = NULL;
    IXMLDOMNode* pNode = NULL;
    IXMLDOMElement* pElement = NULL;
    IXMLDOMSchemaCollection2* pXS = NULL;
    IXMLDOMDocument2* pXD = NULL;
    VARIANT_BOOL varStatus;
    VARIANT varXML;
    BSTR bstrNodeName = NULL;
    long length;

    // 0. change current folder
    TCHAR currenPath[MAX_PATH] = { '\0' };
    ::SendMessage(nppData._nppHandle, NPPM_GETCURRENTDIRECTORY, MAX_PATH, (LPARAM)currenPath);
    if (_wchdir(currenPath)) {
        dbg(L"Unable to change directory to ");
        dbgln(currenPath);
    }

    // 1. check xml syntax
    //bool abortValidation = false;
    std::string xml_schema("");
    int currentEdit, currentLength;
    ::SendMessage(nppData._nppHandle, NPPM_GETCURRENTSCINTILLA, 0, (LPARAM)&currentEdit);
    HWND hCurrentEditView = getCurrentHScintilla(currentEdit);
    currentLength = (int) ::SendMessage(hCurrentEditView, SCI_GETLENGTH, 0, 0);

    char* data = new char[currentLength + sizeof(char)];
    if (!data) return;  // allocation error, abort check
    memset(data, '\0', currentLength + sizeof(char));

    ::SendMessage(hCurrentEditView, SCI_GETTEXT, currentLength + sizeof(char), reinterpret_cast<LPARAM>(data));

    Report::char2VARIANT(data, &varXML);

    CHK_HR(CreateAndInitDOM(&pXMLDom, (INIT_OPTION_VALIDATEONPARSE | INIT_OPTION_RESOLVEEXTERNALS)));

    /*
    // Configure DOM properties for namespace selection.
    CHK_HR(pXMLDoc->setProperty(L"SelectionLanguage", "XPath"));
    //_bstr_t ns = L"xmlns:x='urn:book'";
    CHK_HR(pXMLDoc->setProperty(L"SelectionNamespaces", "xmlns:x='urn:book'"));
    */

    CHK_HR(pXMLDom->load(varXML, &varStatus));
    if (varStatus == VARIANT_TRUE) {
        bool hasSchemaOrDTD = false;

        // search for xsi:noNamespaceSchemaLocation or xsi:schemaLocation
        CHK_HR(pXMLDom->setProperty(L"SelectionNamespaces", _variant_t("xmlns:xsi='http://www.w3.org/2001/XMLSchema-instance'")));
        hr = pXMLDom->selectNodes(L"/*/@xsi:noNamespaceSchemaLocation", &pNodes);
        CHK_HR(pNodes->get_length(&length));
        SAFE_RELEASE(pNodes);
        bool nnsl = (length > 0);
        hr = pXMLDom->selectNodes(L"/*/@xsi:schemaLocation", &pNodes);
        CHK_HR(pNodes->get_length(&length));
        bool sl = (length > 0);
        hasSchemaOrDTD = (nnsl || sl);

        // search for DTD - this will be done using QuickXml
        if (!hasSchemaOrDTD) {
            XmlParser parser(data, currentLength);
            XmlToken token = undefinedToken;
            do {
                token = parser.parseUntil(XmlTokenType::Declaration | XmlTokenType::TagOpening); 
                if (token.type == XmlTokenType::Declaration && !strncmp(token.chars, "<!DOCTYPE", 9)) {
                    break;
                }
            } while (token.type != XmlTokenType::EndOfFile && token.type != XmlTokenType::TagOpening);

            hasSchemaOrDTD = (token.type == XmlTokenType::Declaration);
        }

        if (hasSchemaOrDTD) {
            // if noNamespaceSchemaLocation or schemaLocation attribute is present,
            // validation is supposed OK since xml is loaded with INIT_OPTION_VALIDATEONPARSE
            // option
            if (pXMLDom->validate((IXMLDOMParseError**)&pXMLErr2) == S_FALSE) {
                displayXMLErrors(pXMLErr2, hCurrentEditView, L"XML Validation error");
            }
            else {
                Report::_printf_inf(L"No error detected.");
            }
        }
        else {
            // if noNamespaceSchemaLocation or schemaLocation attributes are not present,
            // we must prompt for XSD to user and create a schema cache
            if (pSelectFileDlg == NULL) {
                pSelectFileDlg = new CSelectFileDlg();
            }
            //pSelectFileDlg->m_sSelectedFilename = lastXMLSchema.c_str();

            CStringW rootSample = "<";
            CHK_HR(pXMLDom->get_documentElement(&pElement));
            CHK_HR(pElement->get_nodeName(&bstrNodeName));
            rootSample += bstrNodeName;
            rootSample += "\r\n    xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"";
            rootSample += "\r\n    xsi:noNamespaceSchemaLocation=\"XSD_FILE_PATH\">";

            pSelectFileDlg->m_sRootElementSample = rootSample;
            if (pSelectFileDlg->DoModal() == IDOK) {
                //lastXMLSchema = pSelectFileDlg->m_sSelectedFilename;

                // Create a schema cache and add schema to it (currently with no namespace)
                CHK_HR(CreateAndInitSchema(&pXS));
                hr = pXS->add(_bstr_t(pSelectFileDlg->m_sValidationNamespace), CComVariant(pSelectFileDlg->m_sSelectedFilename));
                if (SUCCEEDED(hr)) {
                    // Create a DOMDocument and set its properties.
                    CHK_HR(CreateAndInitDOM(&pXD, (INIT_OPTION_VALIDATEONPARSE | INIT_OPTION_RESOLVEEXTERNALS)));
                    CHK_HR(pXD->putref_schemas(CComVariant(pXS)));

                    /*
                    pXD->put_async(VARIANT_FALSE);
                    pXD->put_validateOnParse(VARIANT_TRUE);
                    pXD->put_resolveExternals(VARIANT_TRUE);
                    */

                    CHK_HR(pXD->load(varXML, &varStatus));
                    if (varStatus == VARIANT_TRUE) {
                        Report::_printf_inf(L"No error detected.");
                    }
                    else {
                        CHK_HR(pXD->get_parseError(&pXMLErr));
                        displayXMLErrors(pXMLErr, hCurrentEditView, L"XML Validation error");
                    }
                }
                else {
                    Report::_printf_err(L"Invalid schema or missing namespace.");
                }
            }
            else {
                goto CleanUp;
            }
        }
    }
    else {
        CHK_HR(pXMLDom->get_parseError(&pXMLErr));
        displayXMLErrors(pXMLErr, hCurrentEditView, L"XML Validation error");
    }

CleanUp:
    SAFE_RELEASE(pXMLDom);
    SAFE_RELEASE(pXMLErr);
    SAFE_RELEASE(pNodes);
    SAFE_RELEASE(pNode);
    SAFE_RELEASE(pElement);
    SAFE_RELEASE(pXS);
    SAFE_RELEASE(pXD);
    VariantClear(&varXML);
    SysFreeString(bstrNodeName);
}

void autoValidation() {
    dbgln("autoValidation()");

    XMLValidation(0);
}

void manualValidation() {
    dbgln("manualValidation()");

    XMLValidation(1);
}
