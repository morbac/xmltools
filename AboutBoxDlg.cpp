// AboutBoxDlg.cpp : fichier d'implémentation
//

#include "StdAfx.h"
#include "XMLTools.h"
#include "AboutBoxDlg.h"
#include "afxdialogex.h"

#include "Report.h"

// Boîte de dialogue CAboutBoxDlg

IMPLEMENT_DYNAMIC(CAboutBoxDlg, CDialogEx)

CAboutBoxDlg::CAboutBoxDlg(CWnd* pParent /*=NULL*/)
  : CDialog(CAboutBoxDlg::IDD, pParent)
{

}

CAboutBoxDlg::~CAboutBoxDlg()
{
}

void CAboutBoxDlg::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CAboutBoxDlg, CDialog)
  //{{AFX_MSG_MAP(CAboutBoxDlg)
  //}}AFX_MSG_MAP
  ON_BN_CLICKED(IDC_BUTTON1, &CAboutBoxDlg::OnBnClickedButton1)
  ON_NOTIFY(NM_CLICK, IDC_LNKHOMEPAGE, &CAboutBoxDlg::OnNMClickLnkhomepage)
END_MESSAGE_MAP()


// Gestionnaires de messages de CAboutBoxDlg

BOOL CAboutBoxDlg::OnInitDialog()
{
  CDialog::OnInitDialog();

  #ifdef _DEBUG
    SetDlgItemTextW(IDC_ABOUTTEXT, (Lang_LoadStrFmt(IDS_MSG_ABOUT_LINE1, XMLTOOLS_VERSION_NUMBER, XMLTOOLS_VERSION_STATUS) + Lang_Str(IDS_MSG_ABOUT_DEBUG) + Lang_LoadStr(IDS_MSG_ABOUT_ENGINE)).c_str());
  #else
    SetDlgItemTextW(IDC_ABOUTTEXT, (Lang_LoadStrFmt(IDS_MSG_ABOUT_LINE1, XMLTOOLS_VERSION_NUMBER, XMLTOOLS_VERSION_STATUS) + Lang_LoadStr(IDS_MSG_ABOUT_ENGINE)).c_str());
  #endif

  SetWindowText(Lang_Str(IDS_DLG_ABOUT_CAPTION));

  GetDlgItem(IDC_LNKHOMEPAGE)->SetWindowText(Report::str_format(L"<a href=\"%s\">%s</a>", XMLTOOLS_HOMEPAGE_URL, XMLTOOLS_HOMEPAGE_URL).c_str());

  return TRUE;  // return TRUE unless you set the focus to a control
  // EXCEPTION : les pages de propriétés OCX devraient retourner FALSE
}

void CAboutBoxDlg::OnBnClickedButton1() {
  ShellExecute(NULL, TEXT("open"), PAYPAL_URL, NULL, NULL, SW_SHOW);
}


void CAboutBoxDlg::OnNMClickLnkhomepage(NMHDR* pNMHDR, LRESULT* pResult) {
    PNMLINK pNMLink = (PNMLINK)pNMHDR;

    ShellExecuteW(NULL, L"open", pNMLink->item.szUrl, NULL, NULL, SW_SHOWNORMAL);

    *pResult = 0;
}
