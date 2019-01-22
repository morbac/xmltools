// SelectFileDlg.cpp : implementation file
//

#include "StdAfx.h"
#include "XMLTools.h"
#include "SelectFileDlg.h"

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
  m_sRootElementSample = _T("");
  //}}AFX_DATA_INIT
}


void CSelectFileDlg::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CSelectFileDlg)
  DDX_Text(pDX, IDC_EDIT_FILENAME, m_sSelectedFilename);
  DDX_Text(pDX, IDC_EDIT_ROOTELEMSAMPLE, m_sRootElementSample);
  //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSelectFileDlg, CDialog)
  //{{AFX_MSG_MAP(CSelectFileDlg)
  ON_BN_CLICKED(IDC_BTN_EXPLORE_XSDFILE, OnBtnExploreXSDFile)
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSelectFileDlg message handlers

CString CSelectFileDlg::ShowOpenFileDlg(CString filetypes)
{
  CFileDialog dlg(TRUE, NULL, NULL, NULL, filetypes);
  INT_PTR ret = dlg.DoModal();
  if (ret == IDOK) {
    return dlg.GetPathName();
  }
  return "";
}

void CSelectFileDlg::OnBtnExploreXSDFile() 
{
  CString ret = ShowOpenFileDlg("XML Schema (*.xsd)|*.xsd|All files (*.*)|*.*|");
  if (ret.GetLength()) GetDlgItem(IDC_EDIT_FILENAME)->SetWindowText(ret);
}
