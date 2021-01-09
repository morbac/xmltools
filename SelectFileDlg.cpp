// SelectFileDlg.cpp : implementation file
//

#include "StdAfx.h"
#include "XMLTools.h"
#include "SelectFileDlg.h"

#include <string>
#include <fstream>
#include <streambuf>

#include "XmlWrapperInterface.h"
#include "MSXMLWrapper.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSelectFileDlg dialog


CSelectFileDlg::CSelectFileDlg(CWnd* pParent /*=NULL*/)
  : CDialog(CSelectFileDlg::IDD, pParent)
{
  //{{AFX_DATA_INIT(CSelectFileDlg)
  m_sSelectedFilename = _T("");
  m_sValidationNamespace = _T("");
  m_sRootElementName = _T("");
  //}}AFX_DATA_INIT
}


void CSelectFileDlg::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CSelectFileDlg)
  DDX_Text(pDX, IDC_EDIT_FILENAME, m_sSelectedFilename);
  DDX_Text(pDX, IDC_EDIT_NAMESPACE, m_sValidationNamespace);
  //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSelectFileDlg, CDialog)
  //{{AFX_MSG_MAP(CSelectFileDlg)
  ON_BN_CLICKED(IDC_BTN_EXPLORE_XSDFILE, OnBtnExploreXSDFile)
  //}}AFX_MSG_MAP
  ON_BN_CLICKED(IDOK, &CSelectFileDlg::OnBnClickedOk)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSelectFileDlg message handlers

CStringW CSelectFileDlg::ShowOpenFileDlg(CStringW filetypes)
{
  CFileDialog dlg(TRUE, NULL, NULL, NULL, filetypes);
  INT_PTR ret = dlg.DoModal();
  if (ret == IDOK) {
    return dlg.GetPathName();
  }
  return "";
}

BOOL CSelectFileDlg::OnInitDialog() {
    GetDlgItem(IDC_EDIT_ROOTELEMSAMPLE)->SetWindowText(L"Please select a XML schema file");
    return TRUE;
}

void CSelectFileDlg::OnBtnExploreXSDFile() {
    CStringW ret = ShowOpenFileDlg("XML Schema (*.xsd)|*.xsd|All files (*.*)|*.*|");
    if (ret.GetLength()) {
        if (this->m_sRootElementName.GetLength() > 0) {
            // let's parse the selected file and search for a @targetNamespace attribute in root element
            std::string str;
            std::ifstream t((LPCTSTR)ret);

            t.seekg(0, std::ios::end);
            str.reserve(t.tellg());
            t.seekg(0, std::ios::beg);

            str.assign((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());

            XmlWrapperInterface* wrapper = new MSXMLWrapper();
            std::vector<XPathResultEntryType> nodes = wrapper->xpathEvaluate(str.c_str(), str.size(), L"/xs:schema/@targetNamespace", L"xmlns:xs=\"http://www.w3.org/2001/XMLSchema\"");

            if (nodes.size() == 1) {
                std::wstring targetNamespace = nodes.at(0).value;
                GetDlgItem(IDC_EDIT_NAMESPACE)->SetWindowText(targetNamespace.c_str());

                // write info text
                CStringW rootSample = "<";
                rootSample += this->m_sRootElementName;
                rootSample += "\r\n    xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"";
                rootSample += "\r\n    xsi:schemaLocation=\"";
                rootSample += targetNamespace.c_str();
                rootSample += " ";
                rootSample += ret;
                rootSample += "\">";

                GetDlgItem(IDC_EDIT_ROOTELEMSAMPLE)->SetWindowText(rootSample);
            }
            else {
                GetDlgItem(IDC_EDIT_NAMESPACE)->SetWindowText(L"");

                // write info text
                CStringW rootSample = "<";
                rootSample += this->m_sRootElementName;
                rootSample += "\r\n    xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"";
                rootSample += "\r\n    xsi:noNamespaceSchemaLocation=\"";
                rootSample += ret;
                rootSample += "\">";

                GetDlgItem(IDC_EDIT_ROOTELEMSAMPLE)->SetWindowText(rootSample);
            }

            delete wrapper;
        }

        GetDlgItem(IDC_EDIT_FILENAME)->SetWindowText(ret);
    }
}


void CSelectFileDlg::OnBnClickedOk()
{
  // TODO: Add your control notification handler code here
  CDialog::OnOK();
}