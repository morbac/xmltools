// OptionsDlg.cpp : fichier d'implémentation
//

#include "StdAfx.h"
#include "XMLTools.h"
#include "OptionsDlg.h"
#include "afxdialogex.h"

#include <string>

// Boîte de dialogue COptionsDlg

IMPLEMENT_DYNAMIC(COptionsDlg, CDialogEx)

COptionsDlg::COptionsDlg(CWnd* pParent /*=NULL*/)
  : CDialogEx(COptionsDlg::IDD, pParent)
{
}

COptionsDlg::~COptionsDlg()
{
}

void COptionsDlg::DoDataExchange(CDataExchange* pDX)
{
  CDialogEx::DoDataExchange(pDX);
  DDX_Control(pDX, IDC_EDITPROXYHOST, editProxyHost);
  DDX_Control(pDX, IDC_EDITPROXYPORT, editProxyPort);
  DDX_Control(pDX, IDC_EDITPROXYUSERNAME, editProxyUsername);
  DDX_Control(pDX, IDC_EDITPROXYPASSWORD, editProxyPassword);
}


BEGIN_MESSAGE_MAP(COptionsDlg, CDialogEx)
  ON_BN_CLICKED(IDC_CHKENABLEPROXY, &COptionsDlg::OnBnClickedChkenableproxy)
  ON_BN_CLICKED(IDOK, &COptionsDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// Gestionnaires de messages de COptionsDlg


BOOL COptionsDlg::OnInitDialog()
{
  CDialogEx::OnInitDialog();

  ((CButton*) GetDlgItem(IDC_CHKENABLEPROXY))->SetCheck(proxyoptions.status ? BST_CHECKED : BST_UNCHECKED);
  GetDlgItem(IDC_EDITPROXYHOST)->SetWindowTextW(proxyoptions.host);
  GetDlgItem(IDC_EDITPROXYPORT)->SetWindowTextW(std::to_wstring(static_cast<long long>(proxyoptions.port)).c_str());
  //GetDlgItem(IDC_EDITPROXYUSERNAME)->SetWindowTextW(this->proxyoptions->username);
  //GetDlgItem(IDC_EDITPROXYPASSWORD)->SetWindowTextW(this->proxyoptions->password);
  ((CButton*)GetDlgItem(IDC_CHKPROHIBITDTD))->SetCheck(xmlfeatures.prohibitDTD ? BST_CHECKED : BST_UNCHECKED);

  updateEditFieldsStatus();

  return TRUE;  // return TRUE unless you set the focus to a control
  // EXCEPTION : les pages de propriétés OCX devraient retourner FALSE
}


void COptionsDlg::updateEditFieldsStatus() {
  switch(((CButton*) GetDlgItem(IDC_CHKENABLEPROXY))->GetCheck()) {
    case BST_UNCHECKED: {
      GetDlgItem(IDC_EDITPROXYHOST)->EnableWindow(FALSE);
      GetDlgItem(IDC_EDITPROXYPORT)->EnableWindow(FALSE);
      //GetDlgItem(IDC_EDITPROXYUSERNAME)->EnableWindow(FALSE);
      //GetDlgItem(IDC_EDITPROXYPASSWORD)->EnableWindow(FALSE);
      break;
    }
    case BST_CHECKED: {
      GetDlgItem(IDC_EDITPROXYHOST)->EnableWindow(TRUE);
      GetDlgItem(IDC_EDITPROXYPORT)->EnableWindow(TRUE);
      //GetDlgItem(IDC_EDITPROXYUSERNAME)->EnableWindow(TRUE);
      //GetDlgItem(IDC_EDITPROXYPASSWORD)->EnableWindow(TRUE);

      GetDlgItem(IDC_EDITPROXYHOST)->SetFocus();
      break;
    }
  }
}

void COptionsDlg::OnBnClickedChkenableproxy() {
  updateEditFieldsStatus();
}


void COptionsDlg::OnBnClickedOk() {
  proxyoptions.status = (((CButton*) GetDlgItem(IDC_CHKENABLEPROXY))->GetCheck() == BST_CHECKED);

  CStringW buffer;
  this->editProxyHost.GetWindowText(buffer);
  wcscpy_s(proxyoptions.host, (const WCHAR *)buffer);

  this->editProxyPort.GetWindowText(buffer);
  proxyoptions.port = _wtoi((LPCTSTR)buffer);

  //this->editProxyUsername.GetWindowText(buffer);
  //wcscpy_s(this->proxyoptions->username, (const WCHAR *)buffer);

  //this->editProxyPassword.GetWindowText(buffer);
  //wcscpy_s(this->proxyoptions->password, (const WCHAR *)buffer);

  xmlfeatures.prohibitDTD = (((CButton*)GetDlgItem(IDC_CHKPROHIBITDTD))->GetCheck() == BST_CHECKED);

  CDialogEx::OnOK();
}
