#pragma once
#include "afxwin.h"
#include "CMyPropertyGridCtrl.h"

// Boîte de dialogue COptionsDlg

typedef enum {
  TYPE_BOOL,
  TYPE_INT,
  TYPE_LONG,
  TYPE_STRING,
  TYPE_WSTRING
} enumOptionType;

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
  std::vector<CMFCPropertyGridProperty*> vBoolProperties;
  std::vector<CMFCPropertyGridProperty*> vStringProperties;
  CMFCPropertyGridProperty* pAnnotationStyleProperty;

  virtual void DoDataExchange(CDataExchange* pDX);    // Prise en charge de DDX/DDV

  void UpdateProperty(CMFCPropertyGridProperty* src, enumOptionType type);

  DECLARE_MESSAGE_MAP()
public:
  virtual BOOL OnInitDialog();
  afx_msg void OnBnClickedOk();

  /*
  CEdit editProxyHost;
  CEdit editProxyPort;
  CEdit editProxyUsername;
  CEdit editProxyPassword;
  */
  afx_msg void OnBnClickedBtnviewannotation();
  virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
  CMyPropertyGridCtrl m_wndPropList;
};
