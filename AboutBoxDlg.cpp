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
  DDX_Control(pDX, IDC_ABOUTURL, m_wndExtURL);
}


BEGIN_MESSAGE_MAP(CAboutBoxDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutBoxDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


// Gestionnaires de messages de CAboutBoxDlg

BOOL CAboutBoxDlg::OnInitDialog()
{
  CDialog::OnInitDialog();

  SetDlgItemTextW(IDC_ABOUTTEXT, Report::str_format(L"%s \r\n \r\nlibXML %s \r\nlibXSTL %s",
      TEXT(XMLTOOLS_ABOUTINFO), TEXT(LIBXML_DOTTED_VERSION), TEXT(LIBXSLT_DOTTED_VERSION)).c_str());

  m_wndExtURL.SetURL(PAYPAL_URL);

  return TRUE;  // return TRUE unless you set the focus to a control
  // EXCEPTION : les pages de propriétés OCX devraient retourner FALSE
}