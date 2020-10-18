#if !defined(AFX_XPATHEVALDLG_H__0800E840_15B1_4508_B0B7_B032F1835764__INCLUDED_)
#define AFX_XPATHEVALDLG_H__0800E840_15B1_4508_B0B7_B032F1835764__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// XpathEvalDlg.h : header file
//

#include "PluginInterface.h"
#include "XmlWrapperInterface.h"

#include <string>

/////////////////////////////////////////////////////////////////////////////
// CXPathEvalDlg dialog

class CXPathEvalDlg : public CDialog {
    // Construction
public:
    CXPathEvalDlg(CWnd* pParent = NULL, unsigned long flags = 0);   // standard constructor

  // Dialog Data
    //{{AFX_DATA(CXPathEvalDlg)
    enum { IDD = IDD_XPATHEVAL };
    CStringW m_sExpression;
    CStringW m_sNamespace;
    CStringW m_sResult;
    //}}AFX_DATA


  // Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CXPathEvalDlg)
protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL

  // Implementation
protected:
    unsigned long m_iFlags;

    int execute_xpath_expression(CStringW xpathExpr);
    void print_xpath_nodes(std::vector<XPathResultEntryType> nodes);
    //int register_namespaces(xmlXPathContextPtr xpathCtx, const xmlChar* nsList);
    void AddToList(CListCtrl* list, CStringW type, CStringW name, CStringW value);

    HWND getCurrentHScintilla(int which);

    // Generated message map functions
    //{{AFX_MSG(CXPathEvalDlg)
    afx_msg void OnBtnEvaluate();
    virtual BOOL OnInitDialog();
    afx_msg void OnSize(UINT nType, int cx, int cy);
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnBnClickedBtnCopy2clipboard();
    //  afx_msg void OnDestroy();
    //  afx_msg void OnClose();
    afx_msg void OnBnClickedBtnClearlist();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_XPATHEVALDLG_H__0800E840_15B1_4508_B0B7_B032F1835764__INCLUDED_)
