// XMLTools.h : main header file for the XMLTOOLS DLL
//

#if !defined(AFX_XMLTOOLS_H__53D72BB2_86AD_4964_AF25_DAB5E6403704__INCLUDED_)
#define AFX_XMLTOOLS_H__53D72BB2_86AD_4964_AF25_DAB5E6403704__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
  #error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"    // main symbols

#define XMLTOOLS_ABOUTINFO "XML Tools Plugin\r\nversion 2.4 unicode r1047"
#define XMLTOOLS_DLLNAME   L"XMLTools.dll"

/////////////////////////////////////////////////////////////////////////////
// CXMLToolsApp
// See XMLTools.cpp for the implementation of this class
//

class CXMLToolsApp : public CWinApp
{
public:
  CXMLToolsApp();
  ~CXMLToolsApp();

// Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CXMLToolsApp)
  //}}AFX_VIRTUAL

  //{{AFX_MSG(CXMLToolsApp)
    // NOTE - the ClassWizard will add and remove member functions here.
    //    DO NOT EDIT what you see in these blocks of generated code !
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_XMLTOOLS_H__53D72BB2_86AD_4964_AF25_DAB5E6403704__INCLUDED_)
