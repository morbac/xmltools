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
  /*
  DDX_Control(pDX, IDC_EDITPROXYHOST, editProxyHost);
  DDX_Control(pDX, IDC_EDITPROXYPORT, editProxyPort);
  DDX_Control(pDX, IDC_EDITPROXYUSERNAME, editProxyUsername);
  DDX_Control(pDX, IDC_EDITPROXYPASSWORD, editProxyPassword);
  */
  DDX_Control(pDX, IDC_EDITANNOTATIONSTYLE, editAnnotationStyle);
  DDX_Control(pDX, IDC_PROPERTIES, m_wndPropList);
}


BEGIN_MESSAGE_MAP(COptionsDlg, CDialogEx)
  /*
  ON_BN_CLICKED(IDC_CHKENABLEPROXY, &COptionsDlg::OnBnClickedChkenableproxy)
  */
  ON_BN_CLICKED(IDOK, &COptionsDlg::OnBnClickedOk)
  ON_BN_CLICKED(IDC_CHKANNOTATIONS, &COptionsDlg::OnBnClickedChkannotations)
  ON_BN_CLICKED(IDC_BTNVIEWANNOTATION, &COptionsDlg::OnBnClickedBtnviewannotation)
  ON_STN_CLICKED(IDC_PROPERTIES, &COptionsDlg::OnStnClickedProperties)
END_MESSAGE_MAP()



BOOL COptionsDlg::OnInitDialog() {
  CDialogEx::OnInitDialog();

  testAnnotation = false;

  RECT r;
  GetClientRect(&r);
  m_wndPropList.SetLeftColumnWidth((r.right - r.left) / 2);
  m_wndPropList.SetVSDotNetLook(TRUE);

  // properties
  //m_wndPropList.SetLeftColumnWidth(50);

  CMFCPropertyGridProperty* pTmpOption = NULL;
  CMFCPropertyGridProperty* pGrpOptions = new CMFCPropertyGridProperty(L"Options");
  m_wndPropList.AddProperty(pGrpOptions);

  pTmpOption = new CMFCPropertyGridProperty(L"Display errors as annotations", COleVariant((short) (xmltoolsoptions.useAnnotations ? VARIANT_TRUE : VARIANT_FALSE), VT_BOOL), L"When enabled, errors are displayed as annotation directly in the XML document. When disabled, errors are displayed in dialogs.", (DWORD_PTR) &xmltoolsoptions.useAnnotations);
  pGrpOptions->AddSubItem(pTmpOption); 
  vBoolProperties.push_back(pTmpOption);
  
  pTmpOption = new CMFCPropertyGridProperty(L"Annotations style", COleVariant((long) xmltoolsoptions.annotationStyle, VT_INT), L"Error messages style when displayed as annotations.", (DWORD_PTR) &xmltoolsoptions.annotationStyle);
  pGrpOptions->AddSubItem(pTmpOption);
  vIntProperties.push_back(pTmpOption);


  CMFCPropertyGridProperty* pGrpXml2Txt = new CMFCPropertyGridProperty(L"XML to Text conversion");
  m_wndPropList.AddProperty(pGrpXml2Txt);

  CMFCPropertyGridProperty* pGrpPrettyPrint = new CMFCPropertyGridProperty(L"Pretty print options");
  m_wndPropList.AddProperty(pGrpPrettyPrint);

  CMFCPropertyGridProperty* pGrpXmlFeatures = new CMFCPropertyGridProperty(L"XML Features");
  m_wndPropList.AddProperty(pGrpXmlFeatures);

  /*
  ((CButton*) GetDlgItem(IDC_CHKENABLEPROXY))->SetCheck(proxyoptions.status ? BST_CHECKED : BST_UNCHECKED);
  GetDlgItem(IDC_EDITPROXYHOST)->SetWindowTextW(proxyoptions.host);
  GetDlgItem(IDC_EDITPROXYPORT)->SetWindowTextW(std::to_wstring(static_cast<long>(proxyoptions.port)).c_str());
  //GetDlgItem(IDC_EDITPROXYUSERNAME)->SetWindowTextW(this->proxyoptions->username);
  //GetDlgItem(IDC_EDITPROXYPASSWORD)->SetWindowTextW(this->proxyoptions->password);
  */
  ((CButton*)GetDlgItem(IDC_CHKPROHIBITDTD))->SetCheck(xmltoolsoptions.prohibitDTD ? BST_CHECKED : BST_UNCHECKED);
  ((CButton*)GetDlgItem(IDC_CHKMULTIPLEERRORS))->SetCheck(xmltoolsoptions.multipleErrorMessages ? BST_CHECKED : BST_UNCHECKED);
  ((CButton*)GetDlgItem(IDC_CHKANNOTATIONS))->SetCheck(xmltoolsoptions.useAnnotations ? BST_CHECKED : BST_UNCHECKED);
  GetDlgItem(IDC_EDITANNOTATIONSTYLE)->SetWindowTextW(std::to_wstring(static_cast<int>(xmltoolsoptions.annotationStyle)).c_str());
  ((CButton*)GetDlgItem(IDC_CHKAMP))->SetCheck(xmltoolsoptions.convertAmp ? BST_CHECKED : BST_UNCHECKED);
  ((CButton*)GetDlgItem(IDC_CHKLT))->SetCheck(xmltoolsoptions.convertLt ? BST_CHECKED : BST_UNCHECKED);
  ((CButton*)GetDlgItem(IDC_CHKGT))->SetCheck(xmltoolsoptions.convertGt ? BST_CHECKED : BST_UNCHECKED);
  ((CButton*)GetDlgItem(IDC_CHKQUOTE))->SetCheck(xmltoolsoptions.convertQuote ? BST_CHECKED : BST_UNCHECKED);
  ((CButton*)GetDlgItem(IDC_CHKAPOS))->SetCheck(xmltoolsoptions.convertApos ? BST_CHECKED : BST_UNCHECKED);
  ((CButton*)GetDlgItem(IDC_CHKPPAUTOCLOSE))->SetCheck(xmltoolsoptions.ppAutoclose ? BST_CHECKED : BST_UNCHECKED);

  updateEditFieldsStatus();

  return TRUE;  // return TRUE unless you set the focus to a control
  // EXCEPTION : les pages de propriétés OCX devraient retourner FALSE
}


void COptionsDlg::updateEditFieldsStatus() {
  /*
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
  */

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

void UpdateBoolOption(CMFCPropertyGridProperty* src) {
  (*((bool*)(src->GetData()))) = (src->GetValue().boolVal == VARIANT_TRUE);
}
void UpdateIntOption(CMFCPropertyGridProperty* src) {
  (*((int*)(src->GetData()))) = (src->GetValue().iVal);
}
void UpdateStringOption(CMFCPropertyGridProperty* src) {
  (*((std::wstring*)(src->GetData()))) = src->GetValue().bstrVal;
}

void COptionsDlg::OnBnClickedOk() {
  CStringW buffer;

  /*
  proxyoptions.status = (((CButton*) GetDlgItem(IDC_CHKENABLEPROXY))->GetCheck() == BST_CHECKED);
  
  this->editProxyHost.GetWindowText(buffer);
  wcscpy_s(proxyoptions.host, (const WCHAR *)buffer);

  this->editProxyPort.GetWindowText(buffer);
  proxyoptions.port = _wtoi((LPCTSTR)buffer);

  //this->editProxyUsername.GetWindowText(buffer);
  //wcscpy_s(this->proxyoptions->username, (const WCHAR *)buffer);

  //this->editProxyPassword.GetWindowText(buffer);
  //wcscpy_s(this->proxyoptions->password, (const WCHAR *)buffer);
  */
  
  for (std::vector<CMFCPropertyGridProperty*>::iterator it = vBoolProperties.begin(); it != vBoolProperties.end(); ++it) {
    UpdateBoolOption(*it);
  }
  for (std::vector<CMFCPropertyGridProperty*>::iterator it = vIntProperties.begin(); it != vIntProperties.end(); ++it) {
    UpdateIntOption(*it);
  }
  for (std::vector<CMFCPropertyGridProperty*>::iterator it = vStringProperties.begin(); it != vStringProperties.end(); ++it) {
    UpdateStringOption(*it);
  }

  xmltoolsoptions.prohibitDTD = (((CButton*)GetDlgItem(IDC_CHKPROHIBITDTD))->GetCheck() == BST_CHECKED);
  xmltoolsoptions.multipleErrorMessages = (((CButton*)GetDlgItem(IDC_CHKMULTIPLEERRORS))->GetCheck() == BST_CHECKED);
  //xmltoolsoptions.useAnnotations = (((CButton*)GetDlgItem(IDC_CHKANNOTATIONS))->GetCheck() == BST_CHECKED);
  xmltoolsoptions.convertAmp = (((CButton*)GetDlgItem(IDC_CHKAMP))->GetCheck() == BST_CHECKED);
  xmltoolsoptions.convertLt = (((CButton*)GetDlgItem(IDC_CHKLT))->GetCheck() == BST_CHECKED);
  xmltoolsoptions.convertGt = (((CButton*)GetDlgItem(IDC_CHKGT))->GetCheck() == BST_CHECKED);
  xmltoolsoptions.convertQuote = (((CButton*)GetDlgItem(IDC_CHKQUOTE))->GetCheck() == BST_CHECKED);
  xmltoolsoptions.convertApos = (((CButton*)GetDlgItem(IDC_CHKAPOS))->GetCheck() == BST_CHECKED);
  xmltoolsoptions.ppAutoclose = (((CButton*)GetDlgItem(IDC_CHKPPAUTOCLOSE))->GetCheck() == BST_CHECKED);

  this->editAnnotationStyle.GetWindowText(buffer);
  //xmltoolsoptions.annotationStyle = _wtoi((LPCTSTR)buffer);

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



void COptionsDlg::OnStnClickedProperties() {
}
