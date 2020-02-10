// HowtoUseDlg.cpp : implementation file
//

#include "StdAfx.h"
#include "XMLTools.h"
#include "HowtoUseDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CHowtoUseDlg dialog


CHowtoUseDlg::CHowtoUseDlg(CWnd* pParent /*=NULL*/)
  : CDialog(CHowtoUseDlg::IDD, pParent)
{
  //{{AFX_DATA_INIT(CHowtoUseDlg)
    // NOTE: the ClassWizard will add member initialization here
  //}}AFX_DATA_INIT
}


void CHowtoUseDlg::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CHowtoUseDlg)
    // NOTE: the ClassWizard will add DDX and DDV calls here
  //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CHowtoUseDlg, CDialog)
  //{{AFX_MSG_MAP(CHowtoUseDlg)
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHowtoUseDlg message handlers

BOOL CHowtoUseDlg::OnInitDialog()
{
  CDialog::OnInitDialog();

  return TRUE;  // return TRUE unless you set the focus to a control
                // EXCEPTION: OCX Property Pages should return FALSE
}