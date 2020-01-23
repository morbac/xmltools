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

//---------------------------------------------------------------------------

#define XMLTOOLS_VERSION_NUMBER L"3.0.0.0 alpha 20200121b"
#ifdef V64BIT
#define XMLTOOLS_VERSION_STATUS L"unicode 64bit"
#else
#define XMLTOOLS_VERSION_STATUS L"unicode Win32"
#endif

struct struct_proxyoptions {
  bool status;
  wchar_t host[255];
  long port;
  wchar_t username[255];
  wchar_t password[255];
};

struct struct_xmlfeatures {
  bool prohibitDTD;
};

extern struct struct_proxyoptions proxyoptions;
extern struct struct_xmlfeatures xmlfeatures;
extern void displayXMLError(IXMLDOMParseError* pXMLErr, HWND view, const wchar_t* szDesc = NULL);
extern void dbg(CStringW line);
extern void dbgln(CStringW line);

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
