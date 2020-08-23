#include "StdAfx.h"
#include "Scintilla.h"
#include "PluginInterface.h"
#include "nppHelpers.h"
#include "Report.h"
#include "XMLTools.h"

#include "SelectFileDlg.h"

#include <string>
#include <map>

std::map<LRESULT, bool> hasAnnotations;

void deleteAnnotations() {
    hasAnnotations.clear();
}

// tricky way of centering text on given vertical and horizontal position
// for vertical position, we get the contol size and estimate the number
// of lines based on text height then we force centering by going to wanted
// line +- nlines/2
// for horizontal position, we calculate the text width and check if it is
// out of the bounding box; then scroll horizontally if required
void centerOnPosition(HWND view, size_t line, size_t hofs, size_t wofs, const char* text) {
    try {
        RECT rect;
        GetClientRect(view, &rect);
        int height = (int) ::SendMessage(view, SCI_TEXTHEIGHT, line, NULL);
        int last = (int) ::SendMessage(view, SCI_GETMAXLINESTATE, NULL, NULL);
        size_t nlines_2 = (rect.bottom - rect.top) / (2 * height);

        // force uncollapse target line and ensure line is visible
        ::SendMessage(view, SCI_ENSUREVISIBLE, line - 1, NULL);
        ::SendMessage(view, SCI_SETFIRSTVISIBLELINE, line - 1, NULL);

        // center on line
        if (line - 1 + nlines_2 > (size_t)last) {
            // line is on the end of document
            ::SendMessage(view, SCI_SHOWLINES, line - 1, line + hofs); // force surround lines visibles if folded
        }
        else {
            ::SendMessage(view, SCI_GOTOLINE, line - 1 - nlines_2, 0);
            ::SendMessage(view, SCI_GOTOLINE, line - 1 + nlines_2, 0);
        }

        ::SendMessage(view, SCI_GOTOLINE, line - 1, 0);

        // center on column
        if (text != NULL) {
            size_t width = (size_t) ::SendMessage(view, SCI_TEXTWIDTH, 32, reinterpret_cast<LPARAM>(text));
            size_t margins = (size_t) ::SendMessage(view, SCI_GETMARGINS, 0, 0);
            size_t wmargin = 0;
            for (size_t i = 0; i < margins; ++i) {
                wmargin += (size_t) ::SendMessage(view, SCI_GETMARGINWIDTHN, i, 0);
            }

            if (width + wofs > ((size_t)rect.right - (size_t)rect.left - wmargin)) {
                // error message goes out of the bounding box
                size_t scroll = width + wofs - ((size_t)rect.right - (size_t)rect.left - wmargin);
                ::SendMessage(view, SCI_SETXOFFSET, (int)scroll, 0);
            }
        }

    }
    catch (...) {}
}

void displayXMLError(std::wstring wmsg, HWND view, size_t line, size_t linepos, size_t filepos) {
    // clear final \r\n
    std::string::size_type p = wmsg.find_last_not_of(L"\r\n");
    if (p != std::string::npos && p < wmsg.length()) {
        wmsg.erase(p + 1, std::string::npos);
    }

    if (view == NULL) {
        int currentEdit;
        ::SendMessage(nppData._nppHandle, NPPM_GETCURRENTSCINTILLA, 0, (LPARAM)&currentEdit);
        view = getCurrentHScintilla(currentEdit);
    }

    LRESULT bufferid = ::SendMessage(nppData._nppHandle, NPPM_GETCURRENTBUFFERID, 0, 0);
    UniMode encoding = UniMode(::SendMessage(nppData._nppHandle, NPPM_GETBUFFERENCODING, bufferid, 0));

    if (xmltoolsoptions.useAnnotations) {
        if (line == NULL) {
            line = (size_t) ::SendMessage(nppData._nppHandle, NPPM_GETCURRENTLINE, 0, 0) + 1;
        }

        int maxannotwidth = 0;
        char* buffer = NULL;
        if (linepos != NULL) {
            if (SendMessage(view, SCI_GETWRAPMODE, 0, 0) == 0) {
                size_t linelen = (size_t) ::SendMessage(view, SCI_LINELENGTH, line - 1, 0);
                buffer = new char[linelen + sizeof(char)];
                memset(buffer, '\0', linelen + sizeof(char));
                ::SendMessage(view, SCI_GETLINE, line - 1, reinterpret_cast<LPARAM>(buffer));

                // add spaces before each line to align the annotation on linepos
                // @todo: scroll horizontally if necessary to make annotation visible in current view
                if (linepos <= linelen) {
                    size_t i;
                    std::wstring tabs, spaces;

                    // calculate tabs width
                    int tabwidth = (int) ::SendMessage(view, SCI_GETTABWIDTH, 0, 0);
                    if (tabwidth <= 0) tabwidth = 4;
                    for (int i = 0; i < tabwidth; ++i) tabs += ' ';

                    // replace all char except tabs with space
                    for (i = 0; i < linepos - 1; ++i) {
                        if (buffer[i] == '\t') spaces += tabs;
                        else spaces += ' ';
                    }
                    buffer[linepos - 1] = '\0'; // force buffer end (required for horizontal scrolling)

                    tabs.clear();

                    wmsg.insert(0, spaces);
                    std::string::size_type oldpos = spaces.length(), pos = wmsg.find(L"\r\n");
                    int tmpannotwidth;
                    while (pos != std::string::npos) {
                        tmpannotwidth = (int) ::SendMessage(view, SCI_TEXTWIDTH, 32, reinterpret_cast<LPARAM>(Report::castChar(wmsg.substr(oldpos, pos - oldpos), encoding).c_str()));
                        if (tmpannotwidth > maxannotwidth) maxannotwidth = tmpannotwidth;

                        pos += lstrlen(L"\r\n");
                        wmsg.insert(pos, spaces);
                        oldpos = pos + spaces.length();
                        pos = wmsg.find(L"\r\n", pos);
                    }

                    // calculate last line
                    tmpannotwidth = (int) ::SendMessage(view, SCI_TEXTWIDTH, 32, reinterpret_cast<LPARAM>(Report::castChar(wmsg.substr(oldpos, wmsg.length() - oldpos), encoding).c_str()));
                    if (tmpannotwidth > maxannotwidth) maxannotwidth = tmpannotwidth;

                    spaces.clear();
                }
            }
        }

        // display error as an annotation

        //char* styles = new char[wmsg.length()];
        //memset(styles, xmltoolsoptions.annotationStyle, wmsg.length());
        //memset(styles, 0, wmsg.find(L"\r\n")+1);

        ::SendMessage(view, SCI_ANNOTATIONSETTEXT, line - 1, reinterpret_cast<LPARAM>(Report::castChar(wmsg, encoding).c_str()));

        //::SendMessage(view, SCI_ANNOTATIONSETSTYLES, line - 1, reinterpret_cast<LPARAM>(styles));
        ::SendMessage(view, SCI_ANNOTATIONSETSTYLE, line - 1, xmltoolsoptions.annotationStyle);
        ::SendMessage(view, SCI_ANNOTATIONSETVISIBLE, 1, NULL);

        hasAnnotations[::SendMessage(nppData._nppHandle, NPPM_GETCURRENTBUFFERID, 0, 0)] = true;

        centerOnPosition(view, line, std::count(wmsg.begin(), wmsg.end(), '\n'), maxannotwidth, buffer);

        if (buffer != NULL) delete[] buffer;

        if (filepos != NULL) {
            ::SendMessage(view, SCI_GOTOPOS, filepos - 1, 0);
        }
    }
    else {
        Report::_printf_err(wmsg);
    }
}

void displayXMLError(IXMLDOMParseError* pXMLErr, HWND view, const wchar_t* szDesc) {
    HRESULT hr = S_OK;
    long line = 0;
    long linepos = 0;
    long filepos = 0;
    BSTR bstrReason = NULL;
    std::wstring wmsg = L"";

    if (pXMLErr != NULL) {
        CHK_HR(pXMLErr->get_line(&line));
        CHK_HR(pXMLErr->get_linepos(&linepos));
        CHK_HR(pXMLErr->get_filepos(&filepos));
        CHK_HR(pXMLErr->get_reason(&bstrReason));

        if (szDesc != NULL) {
            if (line > 0 && linepos > 0) {
                wmsg = Report::str_format(L"%s - line %d, pos %d: \r\n%s", szDesc, line, linepos, bstrReason);
            }
            else {
                wmsg = Report::str_format(L"%s: \r\n%s", szDesc, bstrReason);
                line = 1;
                linepos = 1;
            }
        }
        else {
            if (line > 0 && linepos > 0) {
                wmsg = Report::str_format(L"XML Parsing error - line %d, pos %d: \r\n%s", line, linepos, bstrReason);
            }
            else {
                wmsg = Report::str_format(L"XML Parsing error: \r\n%s", bstrReason);
                line = 1;
                linepos = 1;
            }
        }

        displayXMLError(wmsg, view, line, linepos, (size_t)filepos + 1);
    }

CleanUp:
    SysFreeString(bstrReason);
}

void displayXMLErrors(IXMLDOMParseError* pXMLErr, HWND view, const wchar_t* szDesc) {
    HRESULT hr = S_OK;
    IXMLDOMParseError2* pTmpErr = NULL;
    IXMLDOMParseErrorCollection* pAllErrors = NULL;
    long length = 0;

    try {
        CHK_HR(((IXMLDOMParseError2*)pXMLErr)->get_allErrors(&pAllErrors));
        if (pAllErrors != NULL) {
            CHK_HR(pAllErrors->get_length(&length));
            for (long i = 0; i < length; ++i) {
                CHK_HR(pAllErrors->get_next(&pTmpErr));
                CHK_HR(pAllErrors->get_item(i, &pTmpErr));
                displayXMLError(pTmpErr, view, szDesc);
                SAFE_RELEASE(pTmpErr);
            }
        }
        else {
            displayXMLError(pXMLErr, view, szDesc);
        }
    }
    catch (...) {
        displayXMLError(pXMLErr, view, szDesc);
    }

CleanUp:
    SAFE_RELEASE(pTmpErr);
    SAFE_RELEASE(pAllErrors);
}

bool hasCurrentDocAnnotations() {
    if (!xmltoolsoptions.useAnnotations) return false;
    try {
        return hasAnnotations.at(::SendMessage(nppData._nppHandle, NPPM_GETCURRENTBUFFERID, 0, 0));
    }
    catch (const std::out_of_range) {}

    return false;
}

void clearAnnotations(HWND view) {
    if (hasCurrentDocAnnotations()) {
        if (view == NULL) {
            int currentEdit;
            ::SendMessage(nppData._nppHandle, NPPM_GETCURRENTSCINTILLA, 0, (LPARAM)&currentEdit);
            view = getCurrentHScintilla(currentEdit);
        }
        ::SendMessage(view, SCI_ANNOTATIONCLEARALL, NULL, NULL);
        hasAnnotations[::SendMessage(nppData._nppHandle, NPPM_GETCURRENTBUFFERID, 0, 0)] = false;
    }
}

int performXMLCheck(int informIfNoError) {
    dbgln("performXMLCheck()");

    int currentEdit, currentLength, res = 0;
    ::SendMessage(nppData._nppHandle, NPPM_GETCURRENTSCINTILLA, 0, (LPARAM)&currentEdit);
    HWND hCurrentEditView = getCurrentHScintilla(currentEdit);

    clearAnnotations(hCurrentEditView);

    currentLength = (int) ::SendMessage(hCurrentEditView, SCI_GETLENGTH, 0, 0);

    char* data = new char[currentLength + sizeof(char)];
    if (!data) return -1;  // allocation error, abort check
    memset(data, '\0', currentLength + sizeof(char));

    ::SendMessage(hCurrentEditView, SCI_GETTEXT, currentLength + sizeof(char), reinterpret_cast<LPARAM>(data));

    //updateProxyConfig();
    HRESULT hr = S_OK;
    IXMLDOMDocument2* pXMLDom = NULL;
    IXMLDOMParseError* pXMLErr = NULL;
    VARIANT_BOOL varStatus;
    VARIANT varCurrentData;

    Report::char2VARIANT(data, &varCurrentData);

    delete[] data;
    data = NULL;

    CHK_HR(CreateAndInitDOM(&pXMLDom));
    CHK_HR(pXMLDom->load(varCurrentData, &varStatus));
    if (varStatus == VARIANT_TRUE) {
        if (informIfNoError) {
            Report::_printf_inf(L"No error detected.");
        }
    }
    else {
        CHK_HR(pXMLDom->get_parseError(&pXMLErr));
        displayXMLErrors(pXMLErr, hCurrentEditView, L"XML Parsing error");
    }

CleanUp:
    SAFE_RELEASE(pXMLDom);
    SAFE_RELEASE(pXMLErr);
    VariantClear(&varCurrentData);

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
    bool abortValidation = false;
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
        // search for xsi:noNamespaceSchemaLocation or xsi:schemaLocation
        CHK_HR(pXMLDom->setProperty(L"SelectionNamespaces", _variant_t("xmlns:xsi='http://www.w3.org/2001/XMLSchema-instance'")));
        hr = pXMLDom->selectNodes(L"/*/@xsi:noNamespaceSchemaLocation", &pNodes);
        CHK_HR(pNodes->get_length(&length));
        SAFE_RELEASE(pNodes);
        bool nnsl = (length > 0);
        hr = pXMLDom->selectNodes(L"/*/@xsi:schemaLocation", &pNodes);
        CHK_HR(pNodes->get_length(&length));
        bool sl = (length > 0);

        if (nnsl || sl) {
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
