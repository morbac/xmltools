#include "StdAfx.h"
#include "Scintilla.h"
#include "PluginInterface.h"
#include "nppHelpers.h"
#include "Report.h"
#include "XMLTools.h"
#include "XmlParser.h"

#include "SelectFileDlg.h"

#include "XmlWrapperInterface.h"
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

    XmlWrapperInterface* wrapper = new MSXMLWrapper();

    bool isok = wrapper->checkSyntax(data, currentLength);

    auto t_end = clock();
    dbgln(L"crunch time: " + std::to_wstring(t_end - t_start) + L" ms", DBG_LEVEL::DBG_INFO);

    if (isok) {
        if (informIfNoError) {
            Report::_printf_inf(L"No error detected.");
        }
    }
    else {
        displayXMLErrors(wrapper->getLastErrors(), hCurrentEditView, L"XML Parsing error");
    }

    delete wrapper;

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

    XmlWrapperInterface* wrapper = new MSXMLWrapper();
    bool isok = wrapper->checkSyntax(data, currentLength);

    if (isok) {
        // check if a schema prompt is requested
        std::vector<XPathResultEntryType> nodes = wrapper->xpathEvaluate(data, currentLength, L"/*/@xsi:noNamespaceSchemaLocation", L"xmlns:xsi='http://www.w3.org/2001/XMLSchema-instance'");
        bool nnsl = (nodes.size() > 0);
        nodes.clear();

        nodes = wrapper->xpathEvaluate(data, currentLength, L"/*/@xsi:schemaLocation", L"xmlns:xsi='http://www.w3.org/2001/XMLSchema-instance'");
        bool sl = (nodes.size() > 0);

        bool hasSchemaOrDTD = (nnsl || sl);

        if (!hasSchemaOrDTD) {
            // search for DTD - this will be done using QuickXml
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
            if (!wrapper->checkValidity(data, currentLength)) {
                std::vector<ErrorEntryType> errors = wrapper->getLastErrors();
                displayXMLErrors(errors, hCurrentEditView, L"XML Validation error");
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
            nodes.clear();
            nodes = wrapper->xpathEvaluate(data, currentLength, L"/*", L"");
            if (nodes.size() == 1) {
                pSelectFileDlg->m_sRootElementName = nodes.at(0).name.c_str();
            }
            else {
                pSelectFileDlg->m_sRootElementName = L"";
            }

            if (pSelectFileDlg->DoModal() == IDOK) {
                //lastXMLSchema = pSelectFileDlg->m_sSelectedFilename;
                if (!wrapper->checkValidity(data, currentLength, pSelectFileDlg->m_sSelectedFilename.GetString(), pSelectFileDlg->m_sValidationNamespace.GetString())) {
                    std::vector<ErrorEntryType> errors = wrapper->getLastErrors();
                    displayXMLErrors(errors, hCurrentEditView, L"XML Validation error");
                }
                else {
                    Report::_printf_inf(L"No error detected.");
                }
            }
        }
    }
    else {
        Report::_printf_inf(L"Please fix xml syntax first.");
    }

    delete[] data;
    delete wrapper;
}

void autoValidation() {
    dbgln("autoValidation()");

    XMLValidation(0);
}

void manualValidation() {
    dbgln("manualValidation()");

    XMLValidation(1);
}
