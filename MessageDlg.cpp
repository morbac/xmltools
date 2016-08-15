// MessageDlg.cpp : implementation file
//

#include "stdafx.h"
#include "XMLTools.h"
#include "MessageDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMessageDlg dialog


CMessageDlg::CMessageDlg(CWnd* pParent /*=NULL*/)
  : CDialog(CMessageDlg::IDD, pParent)
{
  //{{AFX_DATA_INIT(CMessageDlg)
  m_sMessage = _T("");
  //}}AFX_DATA_INIT
}


void CMessageDlg::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CMessageDlg)
  DDX_Text(pDX, IDC_EDIT_MULTILINEMSG, m_sMessage);
  //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMessageDlg, CDialog)
  //{{AFX_MSG_MAP(CMessageDlg)
  ON_WM_SIZE()
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMessageDlg message handlers

BOOL CMessageDlg::OnInitDialog() 
{
  CDialog::OnInitDialog();
  
  CRect myRect;
  GetClientRect(&myRect);
  ClientToScreen(myRect);
  MoveWindow(myRect.left+100, myRect.top+100, myRect.Width(), myRect.Height());
  
  return TRUE;  // return TRUE unless you set the focus to a control
  // EXCEPTION: OCX Property Pages should return FALSE
}

void CMessageDlg::OnSize(UINT nType, int cx, int cy) 
{
  CDialog::OnSize(nType, cx, cy);
  
  CWnd *edit_wnd = GetDlgItem(IDC_EDIT_MULTILINEMSG);
  CWnd *btn_wnd = GetDlgItem(IDOK);
  
  if (btn_wnd && btn_wnd) {
    const int border = 8;
    const int wndspace = 6;
    const int btnwidth = 75;
    const int btnheight = 24;
    
    edit_wnd->MoveWindow(border,border,cx-2*border,cy-2*border-btnheight-wndspace);
    
    btn_wnd->MoveWindow(cx-border-btnwidth, cy-border-btnheight, btnwidth, btnheight);
  }
}
