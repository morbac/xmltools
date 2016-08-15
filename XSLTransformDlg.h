#if !defined(AFX_XSLTRANSFORMDLG_H__D12987C7_F123_43ED_953D_AAF28DFC852E__INCLUDED_)
#define AFX_XSLTRANSFORMDLG_H__D12987C7_F123_43ED_953D_AAF28DFC852E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// XSLTransformDlg.h : header file
//

#include "PluginInterface.h"

#include <string.h>

#include <libxml/xmlmemory.h>
#include <libxml/debugXML.h>
#include <libxml/HTMLtree.h>
#include <libxml/xmlIO.h>
#include <libxml/DOCBparser.h>
#include <libxml/xinclude.h>
#include <libxml/catalog.h>
#include <libxslt/xslt.h>
#include <libxslt/xsltInternals.h>
#include <libxslt/transform.h>
#include <libxslt/xsltutils.h>

/////////////////////////////////////////////////////////////////////////////
// CXSLTransformDlg dialog

class CXSLTransformDlg : public CDialog
{
// Construction
public:
  CXSLTransformDlg(CWnd* pParent = NULL, unsigned long flags = 0);   // standard constructor

// Dialog Data
  //{{AFX_DATA(CXSLTransformDlg)
  enum { IDD = IDD_XSLTDLG };
  CString  m_sXSLTFile;
  CString  m_sXSLTOptions;
  //}}AFX_DATA

  CString ShowOpenFileDlg(CString filetypes);

// Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CXSLTransformDlg)
  protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  //}}AFX_VIRTUAL

// Implementation
protected:
  unsigned long m_iFlags;

  HWND getCurrentHScintilla(int which);

  // Generated message map functions
  //{{AFX_MSG(CXSLTransformDlg)
  virtual BOOL OnInitDialog();
  afx_msg void OnBtnTransform();
  afx_msg void OnBtnBrowseXSLTFile();
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_XSLTRANSFORMDLG_H__D12987C7_F123_43ED_953D_AAF28DFC852E__INCLUDED_)
