// OptionsDlg.cpp : fichier d'implémentation
//

#include "stdafx.h"
#include "XMLTools.h"
#include "OptionsDlg.h"
#include "afxdialogex.h"

#include <string>

// Boîte de dialogue COptionsDlg

IMPLEMENT_DYNAMIC(COptionsDlg, CDialogEx)

COptionsDlg::COptionsDlg(CWnd* pParent /*=NULL*/, struct struct_proxyoptions* proxyoptions /*=NULL*/)
	: CDialogEx(COptionsDlg::IDD, pParent)
{
  this->proxyoptions = proxyoptions;
}

COptionsDlg::~COptionsDlg()
{
}

void COptionsDlg::DoDataExchange(CDataExchange* pDX)
{
  CDialogEx::DoDataExchange(pDX);
  DDX_Control(pDX, IDC_EDITPROXYHOST, editProxyHost);
  DDX_Control(pDX, IDC_EDITPROXYPORT, editProxyPort);
}


BEGIN_MESSAGE_MAP(COptionsDlg, CDialogEx)
  ON_BN_CLICKED(IDC_CHKENABLEPROXY, &COptionsDlg::OnBnClickedChkenableproxy)
  ON_BN_CLICKED(IDOK, &COptionsDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// Gestionnaires de messages de COptionsDlg


BOOL COptionsDlg::OnInitDialog()
{
  CDialogEx::OnInitDialog();

  if (this->proxyoptions != NULL) {
    if (this->proxyoptions->status) {
      ((CButton*) GetDlgItem(IDC_CHKENABLEPROXY))->SetCheck(BST_CHECKED);
    } else {
      ((CButton*) GetDlgItem(IDC_CHKENABLEPROXY))->SetCheck(BST_UNCHECKED);
    }
    GetDlgItem(IDC_EDITPROXYHOST)->SetWindowTextW(this->proxyoptions->host);
    GetDlgItem(IDC_EDITPROXYPORT)->SetWindowTextW(std::to_wstring(this->proxyoptions->port).c_str());
  }
  
  updateEditFieldsStatus();

  return TRUE;  // return TRUE unless you set the focus to a control
  // EXCEPTION : les pages de propriétés OCX devraient retourner FALSE
}


void COptionsDlg::updateEditFieldsStatus() {
  switch(((CButton*) GetDlgItem(IDC_CHKENABLEPROXY))->GetCheck()) {
    case BST_UNCHECKED: {
      GetDlgItem(IDC_EDITPROXYHOST)->EnableWindow(FALSE);
      GetDlgItem(IDC_EDITPROXYPORT)->EnableWindow(FALSE);
      break;
    }
    case BST_CHECKED: {
      GetDlgItem(IDC_EDITPROXYHOST)->EnableWindow(TRUE);
      GetDlgItem(IDC_EDITPROXYPORT)->EnableWindow(TRUE);

      GetDlgItem(IDC_EDITPROXYHOST)->SetFocus();
      break;
    }
  }
}

void COptionsDlg::OnBnClickedChkenableproxy() {
  updateEditFieldsStatus();
}


void COptionsDlg::OnBnClickedOk()
{
  if (this->proxyoptions != NULL) {
    this->proxyoptions->status = (((CButton*) GetDlgItem(IDC_CHKENABLEPROXY))->GetCheck() == BST_CHECKED);

    CString buffer;
    this->editProxyHost.GetWindowText(buffer);
    wcscpy_s(this->proxyoptions->host, (const WCHAR *)buffer);

    this->editProxyPort.GetWindowText(buffer);
    this->proxyoptions->port = _wtoi((LPCTSTR)buffer);
  }

  CDialogEx::OnOK();
}
