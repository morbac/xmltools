// XSLTransformDlg.cpp : implementation file
//

#include "StdAfx.h"
#include "Scintilla.h"
#include "XMLTools.h"
#include "XSLTransformDlg.h"
#include "Report.h"
#include "menuCmdID.h"
#include "XmlWrapperInterface.h"
#include "MSXMLWrapper.h"

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
  m_sSelectedFile = _T("");
  m_sXSLTOptions = _T("");
  //}}AFX_DATA_INIT

  this->m_iFlags = flags;
}


void CXSLTransformDlg::DoDataExchange(CDataExchange* pDX) {
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CXSLTransformDlg)
  DDX_Text(pDX, IDC_EDIT_XSLTFILE, m_sSelectedFile);
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

void CXSLTransformDlg::OnBtnTransform() {
    this->UpdateData();

    if (this->m_sSelectedFile.GetLength() <= 0) {
        Report::_printf_err(L"Cannot continue, missing parameters. Please select a file.");
        return;
    }

    int currentEdit;
    ::SendMessage(nppData._nppHandle, NPPM_GETCURRENTSCINTILLA, 0, (LPARAM)&currentEdit);
    HWND hCurrentEditView = getCurrentHScintilla(currentEdit);
    
    size_t currentLength = (size_t) ::SendMessage(hCurrentEditView, SCI_GETLENGTH, 0, 0);

    char* data = new char[currentLength + sizeof(char)];
    if (!data) return;  // allocation error, abort check
    memset(data, '\0', currentLength + sizeof(char));

    ::SendMessage(hCurrentEditView, SCI_GETTEXT, currentLength + sizeof(char), reinterpret_cast<LPARAM>(data));

    XmlWrapperInterface* wrapper = new MSXMLWrapper();
    XSLTransformResultType res;
    if (wrapper->xslTransform(data, currentLength, m_sSelectedFile.GetString(), &res, m_sXSLTOptions.GetString())) {
        ::SendMessage(nppData._nppHandle, NPPM_MENUCOMMAND, 0, IDM_FILE_NEW);
        if (res.encoding != UniMode::uniEnd) Report::setEncoding(res.encoding, hCurrentEditView);
        ::SendMessage(hCurrentEditView, SCI_SETTEXT, 0, reinterpret_cast<LPARAM>(res.data.c_str()));
    }
    else {
        std::vector<ErrorEntryType> errors = wrapper->getLastErrors();
        displayXMLErrors(errors, hCurrentEditView, L"Error while XSL transformation");
    }

    delete[] data;
    data = NULL;

    delete wrapper;
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
  CStringW ret = ShowOpenFileDlg("XML/XSL Files|*.xml;*.xsl|All files|*.*|");
  if (ret.GetLength()) GetDlgItem(IDC_EDIT_XSLTFILE)->SetWindowText(ret);
}
