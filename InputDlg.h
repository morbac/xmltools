#if !defined(AFX_INPUTDLG_H__0E01277C_7E93_447A_B758_F48A3B7D14EF__INCLUDED_)
#define AFX_INPUTDLG_H__0E01277C_7E93_447A_B758_F48A3B7D14EF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// InputDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CInputDlg dialog

class CInputDlg : public CDialog
{
// Construction
public:
  CInputDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
  //{{AFX_DATA(CInputDlg)
  enum { IDD = IDD_INPUTDLG };
  CStringW m_sInputCaption;
  CStringW m_sInputBuffer;
  //}}AFX_DATA

  CStringW m_sInputTitle;

// Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CInputDlg)
  protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  //}}AFX_VIRTUAL

// Implementation
protected:

  // Generated message map functions
  //{{AFX_MSG(CInputDlg)
  virtual BOOL OnInitDialog();
  afx_msg void OnSetfocusEditInput();
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_INPUTDLG_H__0E01277C_7E93_447A_B758_F48A3B7D14EF__INCLUDED_)
