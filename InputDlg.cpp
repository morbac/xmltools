// InputDlg.cpp : implementation file
//

#include "StdAfx.h"
#include "XMLTools.h"
#include "InputDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CInputDlg dialog


CInputDlg::CInputDlg(CWnd* pParent /*=NULL*/)
  : CDialog(CInputDlg::IDD, pParent)
{
  //{{AFX_DATA_INIT(CInputDlg)
  m_sInputCaption = _T("");
  m_sInputBuffer = _T("");
  //}}AFX_DATA_INIT
}


void CInputDlg::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CInputDlg)
  DDX_Text(pDX, IDC_STATIC_CAPTION, m_sInputCaption);
  DDX_Text(pDX, IDC_EDIT_INPUT, m_sInputBuffer);
  //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CInputDlg, CDialog)
  //{{AFX_MSG_MAP(CInputDlg)
  ON_EN_SETFOCUS(IDC_EDIT_INPUT, OnSetfocusEditInput)
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInputDlg message handlers

BOOL CInputDlg::OnInitDialog() 
{
  CDialog::OnInitDialog();
  
  SetWindowText(m_sInputTitle);
  ((CEdit*)GetDlgItem(IDC_EDIT_INPUT))->SetFocus();
  
  return FALSE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

void CInputDlg::OnSetfocusEditInput() 
{
  CEdit *edit = (CEdit*)GetDlgItem(IDC_EDIT_INPUT);
  edit->SetSel(0, -1);
}
