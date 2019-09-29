#if !defined(AFX_SELECTFILEDLG_H__AE9AE655_AC47_48E0_B812_DAE9EC653435__INCLUDED_)
#define AFX_SELECTFILEDLG_H__AE9AE655_AC47_48E0_B812_DAE9EC653435__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SelectFileDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSelectFileDlg dialog

class CSelectFileDlg : public CDialog
{
// Construction
public:
  CSelectFileDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
  //{{AFX_DATA(CSelectFileDlg)
  enum { IDD = IDD_SELECTFILE };
  CStringW  m_sSelectedFilename;
  CStringW  m_sRootElementSample;
  //}}AFX_DATA

  CStringW ShowOpenFileDlg(CStringW filetypes);

// Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CSelectFileDlg)
  protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  //}}AFX_VIRTUAL

// Implementation
protected:

  // Generated message map functions
  //{{AFX_MSG(CSelectFileDlg)
  afx_msg void OnBtnExploreXSDFile();
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SELECTFILEDLG_H__AE9AE655_AC47_48E0_B812_DAE9EC653435__INCLUDED_)
