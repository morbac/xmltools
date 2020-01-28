// XSLTransformDlg.cpp : implementation file
//

#include "StdAfx.h"
#include "Scintilla.h"
#include "XMLTools.h"
#include "XSLTransformDlg.h"
#include "Report.h"
#include "menuCmdID.h"
#include "MSXMLHelper.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//extern void updateProxyConfig();

/////////////////////////////////////////////////////////////////////////////
// CXSLTransformDlg dialog

CXSLTransformDlg::CXSLTransformDlg(CWnd* pParent /*=NULL*/, unsigned long flags /*= 0*/)
  : CDialog(CXSLTransformDlg::IDD, pParent) {
  //{{AFX_DATA_INIT(CXSLTransformDlg)
  m_sXSLTFile = _T("");
  m_sXSLTOptions = _T("");
  //}}AFX_DATA_INIT
  
  this->m_iFlags = flags;
}


void CXSLTransformDlg::DoDataExchange(CDataExchange* pDX) {
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CXSLTransformDlg)
  DDX_Text(pDX, IDC_EDIT_XSLTFILE, m_sXSLTFile);
  DDX_Text(pDX, IDC_EDIT_XSLTOPTIONS, m_sXSLTOptions);
  //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CXSLTransformDlg, CDialog)
  //{{AFX_MSG_MAP(CXSLTransformDlg)
  ON_BN_CLICKED(IDC_BTN_TRANSFORM, OnBtnTransform)
  ON_BN_CLICKED(IDC_BTN_BROWSEXSLTFILE, OnBtnBrowseXSLTFile)
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CXSLTransformDlg message handlers

HWND CXSLTransformDlg::getCurrentHScintilla(int which) {
  return (which == 0)?nppData._scintillaMainHandle:nppData._scintillaSecondHandle;
}

BOOL CXSLTransformDlg::OnInitDialog() {
  CDialog::OnInitDialog();
  
  CRect myRect;
  GetWindowRect(&myRect);

  MoveWindow(myRect.left+100, myRect.top+100, myRect.Width(), myRect.Height());

  return TRUE;  // return TRUE unless you set the focus to a control
                // EXCEPTION: OCX Property Pages should return FALSE
}

/*
  Get next key/val pair in string, starting from given position. The function
  returns the position where parser stoped, or -1 (std::string::npos) when no
  pair was found. The 'key' and 'val' variables are feeded with readen key and
  value.
  @todo: rewrite this method and use regex
*/
std::string::size_type getNextParam(std::wstring& str, std::string::size_type startpos, std::wstring *key, std::wstring *val) {
  std::string::size_type len = str.length();
  if (startpos < 0 || startpos >= len || !key || !val) return std::string::npos;
  
  // skip spaces, tabs and carriage returns
  std::string::size_type keypos = str.find_first_not_of(L" \t\r\n", startpos);
  if (keypos == std::string::npos || keypos >= len) return std::string::npos;
  
  // next char shouldn't be a '='
  if (str.at(keypos) == '=') return std::string::npos;
  
  // keypos points on begin of the key; let's search for next '=' or ' '
  std::string::size_type valpos = str.find_first_of(L"=", keypos+1);
  valpos = str.find_last_not_of(L" =", valpos);  // get last char of the key
  *key = str.substr(keypos, valpos-keypos+1);

  // skip the '='
  valpos = 1+str.find_first_of(L"=", valpos+1);

  if (str.at(valpos) == ' ') valpos = str.find_first_not_of(L" ", valpos);  // skip eventual space chars
  if (valpos < 0 || valpos >= len) return std::string::npos;
  
  // here we must parse the string; if it starts with an apostroph, let's search
  // the next apostroph; otherwise let's read the next word
  std::string::size_type valendpos = valpos;
  if (str.at(valendpos) == '\'') {
    valendpos = str.find_first_of(L"\'", valendpos+1);
    *val = str.substr(valpos, valendpos-valpos+1);
  } else {
    valendpos = str.find_first_of(L" \t\r\n", valendpos);
    // at the end of the string, valendpos = -1
    if (valendpos < 0) valendpos = len;
    *val = str.substr(valpos, valendpos-valpos);
  }
  
  return valendpos;
}

void CXSLTransformDlg::OnBtnTransform() {
  // inspired from https://www.codeguru.com/cpp/data/data-misc/xml/article.php/c4565/Doing-XSLT-with-MSXML-in-C.htm
  // and msxsl tool source code (https://www.microsoft.com/en-us/download/details.aspx?id=21714)
  HRESULT hr = S_OK;
  HGLOBAL hg = NULL;
  char* output = NULL;
  IXMLDOMDocument2* pXml = NULL;
  IXMLDOMDocument2* pXslt = NULL;
  IXSLTemplate* pTemplate = NULL;
  IXSLProcessor* pProcessor = NULL;
  IXMLDOMParseError* pXMLErr = NULL;
  IXMLDOMNodeList* pNodes = NULL;
  IXMLDOMNode* pNode = NULL;
  //IXMLDOMDocument2* pDOMObject = NULL;
  IStream* pOutStream = NULL;
  VARIANT varXML;
  VARIANT varValue;
  VARIANT_BOOL varStatus;
  BSTR bstrEncoding = NULL;

  bool outputAsStream = false;

  this->UpdateData();

  if (this->m_sXSLTFile.GetLength() <= 0) {
    Report::_printf_err(L"XSLT File missing. Cannot continue.");
    return;
  }

  int currentEdit, currentLength;
  ::SendMessage(nppData._nppHandle, NPPM_GETCURRENTSCINTILLA, 0, (LPARAM)&currentEdit);
  HWND hCurrentEditView = getCurrentHScintilla(currentEdit);
  UniMode encoding = UniMode::uniEnd;

  currentLength = (int) ::SendMessage(hCurrentEditView, SCI_GETLENGTH, 0, 0);

  char* data = new char[currentLength + sizeof(char)];
  if (!data) return;  // allocation error, abort check
  memset(data, '\0', currentLength + sizeof(char));

  ::SendMessage(hCurrentEditView, SCI_GETTEXT, currentLength + sizeof(char), reinterpret_cast<LPARAM>(data));

  Report::char2VARIANT(data, &varXML);

  delete[] data;
  data = NULL;

  // load xml
  CHK_HR(CreateAndInitDOM(&pXml));
  CHK_HR(pXml->load(varXML, &varStatus));
  if (varStatus == VARIANT_TRUE) {
    // load xsl
    CHK_HR(CreateAndInitDOM(&pXslt, (INIT_OPTION_PRESERVEWHITESPACE | INIT_OPTION_FREETHREADED)));
    CHK_HR(pXslt->load(_variant_t(m_sXSLTFile), &varStatus));
    if (varStatus == VARIANT_TRUE) {
      // detect output encoding
      CHK_HR(pXslt->setProperty(L"SelectionNamespaces", variant_t(L"xmlns:xsl=\"http://www.w3.org/1999/XSL/Transform\"")));
      hr = pXslt->selectNodes(L"/xsl:stylesheet/xsl:output/@encoding", &pNodes);
      if (SUCCEEDED(hr)) {
        long length;
        CHK_HR(pNodes->get_length(&length));
        if (length == 1) {
          // get encoding from output declaration
          pNodes->get_item(0, &pNode);
          pNode->get_text(&bstrEncoding);
          encoding = Report::getEncoding(bstrEncoding);
          SAFE_RELEASE(pNode);
          SAFE_RELEASE(pNodes);

          outputAsStream = TRUE;
        } else {
          // get encoding of source file
          encoding = Report::getEncoding(nppData._nppHandle);
          outputAsStream = FALSE;
        }
      }
      CHK_HR(pXslt->setProperty(L"SelectionNamespaces", variant_t(L"")));

      // build template
      CHK_HR(CreateAndInitXSLTemplate(&pTemplate));
      hr = pTemplate->putref_stylesheet(pXslt);
      if (SUCCEEDED(hr)) {
        CHK_HR(pTemplate->createProcessor(&pProcessor));

        // set startMode
        // @todo

        // set parameters; let's decode params string; the string should have the following form:
        //   variable1=value1;variable2=value2;variable3="value 3"
        std::string::size_type i = std::string::npos;
        std::wstring options = this->m_sXSLTOptions, key, val;
        while ((i = getNextParam(options, i + 1, &key, &val)) != std::string::npos) {
          _bstr_t var0(key.c_str());
          VARIANT var1;
          CHK_HR(VariantFromString(val.c_str(), var1));
          CHK_HR(pProcessor->addParameter(var0, var1));
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
        }

        // transform
        CHK_HR(pProcessor->transform(&varStatus));
        if (varStatus == VARIANT_TRUE) {
          // get results of transformation and send them to a new NPP document
          if (outputAsStream) {
            CHK_HR(pOutStream->Write((void const*)"\0", 1, 0));
            CHK_HR(GetHGlobalFromStream(pOutStream, &hg));
            output = (char*)GlobalLock(hg);
          } else {
            pProcessor->get_output(&varValue);
            output = _com_util::ConvertBSTRToString(_bstr_t(varValue));
          }

          ::SendMessage(nppData._nppHandle, NPPM_MENUCOMMAND, 0, IDM_FILE_NEW);
          if (encoding != uniEnd) Report::setEncoding(encoding, hCurrentEditView);
          ::SendMessage(hCurrentEditView, SCI_SETTEXT, 0, reinterpret_cast<LPARAM>(output));

            if (outputAsStream) {
              GlobalUnlock(hg);
            }
        } else {
          Report::_printf_err(L"An error occurred during XSL transformation");
        }
      } else {
        Report::_printf_err(L"The XSL stylesheet is not valid. Transformation aborted.");
      }
    } else {
      CHK_HR(pXslt->get_parseError(&pXMLErr));
      displayXMLError(pXMLErr, hCurrentEditView, L"Error: unable to parse XML");
    }
  } else {
    CHK_HR(pXml->get_parseError(&pXMLErr));
    displayXMLError(pXMLErr, hCurrentEditView, L"Error while loading XSL");
  }

CleanUp:
  SAFE_RELEASE(pXml);
  SAFE_RELEASE(pXslt);
  SAFE_RELEASE(pTemplate);
  SAFE_RELEASE(pProcessor);
  SAFE_RELEASE(pXMLErr);
  //SAFE_RELEASE(pDOMObject);
  SAFE_RELEASE(pOutStream);
  SAFE_RELEASE(pNodes);
  SAFE_RELEASE(pNode);
  SysFreeString(bstrEncoding);
  VariantClear(&varXML);
  VariantClear(&varValue);
}

CStringW CXSLTransformDlg::ShowOpenFileDlg(CStringW filetypes) {
  CFileDialog dlg(TRUE, NULL, NULL, NULL, filetypes);
  INT_PTR ret = dlg.DoModal();
  if (ret == IDOK) {
    return dlg.GetPathName();
  }
  return "";
}

void CXSLTransformDlg::OnBtnBrowseXSLTFile() {
  CStringW ret = ShowOpenFileDlg("XSL Files (*.xsl)|*.xsl|XML Files (*.xml)|*.xml|All files (*.*)|*.*|");
  if (ret.GetLength()) GetDlgItem(IDC_EDIT_XSLTFILE)->SetWindowText(ret);
}
