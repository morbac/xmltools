#pragma once
#include "afxwin.h"

// Boîte de dialogue COptionsDlg

class COptionsDlg : public CDialogEx
{
  DECLARE_DYNAMIC(COptionsDlg)

public:
  COptionsDlg(CWnd* pParent = NULL);   // constructeur standard
  virtual ~COptionsDlg();

// Données de boîte de dialogue
  enum { IDD = IDD_OPTIONS };

protected:
  bool testAnnotation = false;

  virtual void DoDataExchange(CDataExchange* pDX);    // Prise en charge de DDX/DDV

  void updateEditFieldsStatus();

  DECLARE_MESSAGE_MAP()
public:
  virtual BOOL OnInitDialog();
  afx_msg void OnBnClickedChkenableproxy();
  afx_msg void OnBnClickedOk();

  /*
  CEdit editProxyHost;
  CEdit editProxyPort;
  CEdit editProxyUsername;
  CEdit editProxyPassword;
  */
  CEdit editAnnotationStyle;
  afx_msg void OnBnClickedChkannotations();
  afx_msg void OnBnClickedBtnviewannotation();
  virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
};
