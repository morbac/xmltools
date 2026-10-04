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

#include <direct.h>

using namespace QuickXml;

int performXMLCheck(int informIfNoError) {
    dbgln("performXMLCheck()");

    // Save process preexisting current working directory and drive.
    int prevDrive = _getdrive();
    wchar_t* prevPath = _wgetdcwd(prevDrive, NULL, MAX_PATH);

    // 0. change current folder
    TCHAR currenPath[MAX_PATH] = { '\0' };
    ::SendMessage(nppData._nppHandle, NPPM_GETCURRENTDIRECTORY, MAX_PATH, (LPARAM)currenPath);
    if (_wchdir(currenPath)) {
        dbg(L"Unable to change directory to ");
        dbgln(currenPath);
    }

    int currentEdit, res = 0;
    ::SendMessage(nppData._nppHandle, NPPM_GETCURRENTSCINTILLA, 0, (LPARAM)&currentEdit);
    HWND hCurrentEditView = getCurrentHScintilla(currentEdit);

    clearErrors(hCurrentEditView);

    size_t currentLength = (size_t) ::SendMessage(hCurrentEditView, SCI_GETLENGTH, 0, 0);

    char* data = new char[currentLength + sizeof(char)];
    if (!data) return -1;  // allocation error, abort check
    memset(data, '\0', currentLength + sizeof(char));

    ::SendMessage(hCurrentEditView, SCI_GETTEXT, currentLength + sizeof(char), reinterpret_cast<LPARAM>(data));

    auto t_start = clock();

    XmlWrapperInterface* wrapper = new MSXMLWrapper(data, currentLength);
    delete[] data; data = NULL;

    bool isok = wrapper->checkSyntax();

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

    // Restore process preexisting current working directory (drive restored implicitly).
    if (prevPath)
    {
        _wchdir(prevPath);
        free(prevPath);
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
    int currentEdit;
    ::SendMessage(nppData._nppHandle, NPPM_GETCURRENTSCINTILLA, 0, (LPARAM)&currentEdit);
    HWND hCurrentEditView = getCurrentHScintilla(currentEdit);

    clearErrors(hCurrentEditView);

    size_t currentLength = (size_t) ::SendMessage(hCurrentEditView, SCI_GETLENGTH, 0, 0);

    char* data = new char[currentLength + sizeof(char)];
    if (!data) return;  // allocation error, abort check
    memset(data, '\0', currentLength + sizeof(char));

    ::SendMessage(hCurrentEditView, SCI_GETTEXT, currentLength + sizeof(char), reinterpret_cast<LPARAM>(data));

    XmlWrapperInterface* wrapper = new MSXMLWrapper(data, currentLength);

    bool isok = wrapper->checkSyntax();

    if (isok) {
        // check if a schema prompt is requested
        std::vector<XPathResultEntryType> nodes = wrapper->xpathEvaluate(L"/*/@xsi:noNamespaceSchemaLocation", L"xmlns:xsi='http://www.w3.org/2001/XMLSchema-instance'");
        bool nnsl = (nodes.size() > 0);
        nodes.clear();

        nodes = wrapper->xpathEvaluate(L"/*/@xsi:schemaLocation", L"xmlns:xsi='http://www.w3.org/2001/XMLSchema-instance'");
        bool sl = (nodes.size() > 0);

        bool hasSchemaOrDTD = (nnsl || sl);

        if (!hasSchemaOrDTD) {
            // search for DTD - this will be done using QuickXml
            XmlParser parser(data, currentLength);
            XmlToken token = undefinedToken;
            do {
                token = parser.parseUntil(XmlTokenType::DeclarationBeg | XmlTokenType::TagOpening);
                if (token.type == XmlTokenType::DeclarationBeg && !strncmp(token.chars, "<!DOCTYPE", 9)) {
                    break;
                }
            } while (token.type != XmlTokenType::EndOfFile && token.type != XmlTokenType::TagOpening);

            hasSchemaOrDTD = (token.type == XmlTokenType::DeclarationBeg);
        }

        if (hasSchemaOrDTD) {
            if (!wrapper->checkValidity()) {
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
            nodes = wrapper->xpathEvaluate(L"/*", L"");
            if (nodes.size() == 1) {
                pSelectFileDlg->m_sRootElementName = nodes.at(0).name.c_str();
            }
            else {
                pSelectFileDlg->m_sRootElementName = L"";
            }

            if (pSelectFileDlg->DoModal() == IDOK) {
                //lastXMLSchema = pSelectFileDlg->m_sSelectedFilename;
                if (!wrapper->checkValidity(pSelectFileDlg->m_sSelectedFilename.GetString(), pSelectFileDlg->m_sValidationNamespace.GetString())) {
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

    delete[] data; data = NULL;

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
