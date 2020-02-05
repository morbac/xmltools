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

COptionsDlg::~COptionsDlg() {
}

void COptionsDlg::DoDataExchange(CDataExchange* pDX)
{
  CDialogEx::DoDataExchange(pDX);
  DDX_Control(pDX, IDC_EDITPROXYHOST, editProxyHost);
  DDX_Control(pDX, IDC_EDITPROXYPORT, editProxyPort);
  DDX_Control(pDX, IDC_EDITPROXYUSERNAME, editProxyUsername);
  DDX_Control(pDX, IDC_EDITPROXYPASSWORD, editProxyPassword);
  DDX_Control(pDX, IDC_EDITANNOTATIONSTYLE, editAnnotationStyle);
}


BEGIN_MESSAGE_MAP(COptionsDlg, CDialogEx)
  ON_BN_CLICKED(IDC_CHKENABLEPROXY, &COptionsDlg::OnBnClickedChkenableproxy)
  ON_BN_CLICKED(IDOK, &COptionsDlg::OnBnClickedOk)
  ON_BN_CLICKED(IDC_CHKANNOTATIONS, &COptionsDlg::OnBnClickedChkannotations)
  ON_BN_CLICKED(IDC_BTNVIEWANNOTATION, &COptionsDlg::OnBnClickedBtnviewannotation)
END_MESSAGE_MAP()


// Gestionnaires de messages de COptionsDlg


BOOL COptionsDlg::OnInitDialog()
{
  CDialogEx::OnInitDialog();

  testAnnotation = false;

  ((CButton*) GetDlgItem(IDC_CHKENABLEPROXY))->SetCheck(proxyoptions.status ? BST_CHECKED : BST_UNCHECKED);
  GetDlgItem(IDC_EDITPROXYHOST)->SetWindowTextW(proxyoptions.host);
  GetDlgItem(IDC_EDITPROXYPORT)->SetWindowTextW(std::to_wstring(static_cast<long>(proxyoptions.port)).c_str());
  //GetDlgItem(IDC_EDITPROXYUSERNAME)->SetWindowTextW(this->proxyoptions->username);
  //GetDlgItem(IDC_EDITPROXYPASSWORD)->SetWindowTextW(this->proxyoptions->password);
  ((CButton*)GetDlgItem(IDC_CHKPROHIBITDTD))->SetCheck(xmltoolsoptions.prohibitDTD ? BST_CHECKED : BST_UNCHECKED);
  ((CButton*)GetDlgItem(IDC_CHKANNOTATIONS))->SetCheck(xmltoolsoptions.useAnnotations ? BST_CHECKED : BST_UNCHECKED);
  GetDlgItem(IDC_EDITANNOTATIONSTYLE)->SetWindowTextW(std::to_wstring(static_cast<int>(xmltoolsoptions.annotationStyle)).c_str());

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

  switch (((CButton*)GetDlgItem(IDC_CHKANNOTATIONS))->GetCheck()) {
    case BST_UNCHECKED: {
      GetDlgItem(IDC_EDITANNOTATIONSTYLE)->EnableWindow(FALSE);
      break;
    }
    case BST_CHECKED: {
      GetDlgItem(IDC_EDITANNOTATIONSTYLE)->EnableWindow(TRUE);
      break;
    }
  }
}

void COptionsDlg::OnBnClickedChkenableproxy() {
  updateEditFieldsStatus();
}


void COptionsDlg::OnBnClickedChkannotations() {
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

  xmltoolsoptions.prohibitDTD = (((CButton*)GetDlgItem(IDC_CHKPROHIBITDTD))->GetCheck() == BST_CHECKED);
  xmltoolsoptions.useAnnotations = (((CButton*)GetDlgItem(IDC_CHKANNOTATIONS))->GetCheck() == BST_CHECKED);
  
  this->editAnnotationStyle.GetWindowText(buffer);
  xmltoolsoptions.annotationStyle = _wtoi((LPCTSTR)buffer);

  CDialogEx::OnOK();
}


void COptionsDlg::OnBnClickedBtnviewannotation() {
  CStringW buffer; 
  
  bool prevStatus = xmltoolsoptions.useAnnotations;
  int prevStyle = xmltoolsoptions.annotationStyle;

  xmltoolsoptions.useAnnotations = true;
  this->editAnnotationStyle.GetWindowText(buffer);
  xmltoolsoptions.annotationStyle = _wtoi((LPCTSTR)buffer);

  testAnnotation = true;
  clearAnnotations();
  displayXMLError(L"This is an annotation example.");

  xmltoolsoptions.useAnnotations = prevStatus;
  xmltoolsoptions.annotationStyle = prevStyle;
}


BOOL COptionsDlg::OnCommand(WPARAM wParam, LPARAM lParam) {
  if (testAnnotation) {
    clearAnnotations();
  }

  return CDialogEx::OnCommand(wParam, lParam);
}
