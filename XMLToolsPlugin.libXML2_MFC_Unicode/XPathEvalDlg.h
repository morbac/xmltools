#if !defined(AFX_XPATHEVALDLG_H__0800E840_15B1_4508_B0B7_B032F1835764__INCLUDED_)
#define AFX_XPATHEVALDLG_H__0800E840_15B1_4508_B0B7_B032F1835764__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// XpathEvalDlg.h : header file
//

#include "PluginInterface.h"

#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>

/////////////////////////////////////////////////////////////////////////////
// CXPathEvalDlg dialog

class CXPathEvalDlg : public CDialog
{
// Construction
public:
  CXPathEvalDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
  //{{AFX_DATA(CXPathEvalDlg)
  enum { IDD = IDD_XPATHEVAL };
  CString  m_sExpression;
  //}}AFX_DATA


// Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CXPathEvalDlg)
  protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  //}}AFX_VIRTUAL

// Implementation
protected:
  int execute_xpath_expression(const xmlChar* xpathExpr, const xmlChar* nsList);
  int register_namespaces(xmlXPathContextPtr xpathCtx, const xmlChar* nsList);
  void print_xpath_nodes(xmlXPathObjectPtr xpathObj);

  HWND getCurrentHScintilla(int which);

  // Generated message map functions
  //{{AFX_MSG(CXPathEvalDlg)
  afx_msg void OnBtnEvaluate();
  virtual BOOL OnInitDialog();
  afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
  DECLARE_MESSAGE_MAP()
public:
  afx_msg void OnEnChangeEditExpression();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_XPATHEVALDLG_H__0800E840_15B1_4508_B0B7_B032F1835764__INCLUDED_)
