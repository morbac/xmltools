// AboutBoxDlg.cpp : fichier d'implémentation
//

#include "stdafx.h"
#include "XMLTools.h"
#include "AboutBoxDlg.h"
#include "afxdialogex.h"

#include "Report.h"
#include <libxml/xmlversion.h>
#include <libxslt/xsltconfig.h>

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
END_MESSAGE_MAP()


// Gestionnaires de messages de CAboutBoxDlg

BOOL CAboutBoxDlg::OnInitDialog()
{
  CDialog::OnInitDialog();

  #ifdef _DEBUG
    SetDlgItemTextW(IDC_ABOUTTEXT, Report::str_format(L"XML Tools Plugin\r\nversion %s %s (debug)\r\n \r\nlibXML %s \r\nlibXSTL %s",
          XMLTOOLS_VERSION_NUMBER, XMLTOOLS_VERSION_STATUS, TEXT(LIBXML_DOTTED_VERSION), TEXT(LIBXSLT_DOTTED_VERSION)).c_str());
  #else
    SetDlgItemTextW(IDC_ABOUTTEXT, Report::str_format(L"XML Tools Plugin\r\nversion %s %s\r\n \r\nlibXML %s \r\nlibXSTL %s",
        XMLTOOLS_VERSION_NUMBER, XMLTOOLS_VERSION_STATUS, TEXT(LIBXML_DOTTED_VERSION), TEXT(LIBXSLT_DOTTED_VERSION)).c_str());
  #endif

  return TRUE;  // return TRUE unless you set the focus to a control
  // EXCEPTION : les pages de propriétés OCX devraient retourner FALSE
}

void CAboutBoxDlg::OnBnClickedButton1() {
  ShellExecute(NULL, TEXT("open"), PAYPAL_URL, NULL, NULL, SW_SHOW);
}
