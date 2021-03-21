#pragma once
#include "afxwin.h"
#include "CMyPropertyGridCtrl.h"

// Boîte de dialogue COptionsDlg

enum class enumOptionType {
  TYPE_BOOL,
  TYPE_TRISTATE,
  TYPE_INT,
  TYPE_LONG,
  TYPE_STRING,
  TYPE_WSTRING,
  TYPE_WCHAR255
};

class COptionsDlg : public CDialogEx {
  DECLARE_DYNAMIC(COptionsDlg)

public:
  COptionsDlg(CWnd* pParent = NULL);   // constructeur standard
  virtual ~COptionsDlg();

// Données de boîte de dialogue
  enum { IDD = IDD_OPTIONS };

protected:
  bool testAnnotation = false;

  std::vector<CMFCPropertyGridProperty*> vIntProperties;
  std::vector<CMFCPropertyGridProperty*> vLongProperties;
  std::vector<CMFCPropertyGridProperty*> vBoolProperties;
  std::vector<CMFCPropertyGridProperty*> vTristateProperties;
  std::vector<CMFCPropertyGridProperty*> vWStringProperties;
  std::vector<CMFCPropertyGridProperty*> vWChar255Properties;
  CMFCPropertyGridProperty* pAnnotationStyleProperty = NULL;
  CMFCPropertyGridProperty* pAnnotationHighlightStyleProperty = NULL;

  virtual void DoDataExchange(CDataExchange* pDX);    // Prise en charge de DDX/DDV

  void UpdateProperty(CMFCPropertyGridProperty* src, enumOptionType type);

  DECLARE_MESSAGE_MAP()
public:
  virtual BOOL OnInitDialog();
  afx_msg void OnBnClickedOk();

  afx_msg void OnBnClickedBtnviewannotation();
  virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
  CMyPropertyGridCtrl m_wndPropList;
};
