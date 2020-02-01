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
#include <string>

//---------------------------------------------------------------------------

#define XMLTOOLS_VERSION_NUMBER L"3.0.0.2 alpha"
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

struct struct_xmltoolsoptions {       // default value
  // msxml features
  bool allowDocumentFunction;         // True in 3.0. False in 6.0.
  bool AllowXsltScript;               // True in 3.0. False in 6.0.
  bool forceResync;                   // True
  bool maxElementDepth;               // 0 in 3.0. 256 in 6.0.
  bool maxXMLSize;                    // 0
  bool multipleErrorMessages;         // False
  bool newParser;                     // False
  bool normalizeAttributeValues;      // False
  bool populateElementDefaultValues;  // False
  bool prohibitDTD;                   // True in 3.0. False in 6.0.
  bool resolveExternals;              // False
  std::wstring selectionLanguage;     // "XSLPattern" in 3.0. "XPath" in 6.0
  std::wstring selectionNamespace;    // ""
  bool serverHTTPRequest;             // False
  bool useInlineSchema;               // False
  bool validateOnParse;               // True

  // xmltools options
  bool useAnnotations;                // False
  int annotationStyle;                // 34
};

extern struct struct_proxyoptions proxyoptions;
extern struct struct_xmltoolsoptions xmltoolsoptions;
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
