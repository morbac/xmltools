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
#include "MSXMLHelper.h"
#include "XmlWrapperInterface.h"
#include "Config.h"
#include "Debug.h"
#include <string>

//---------------------------------------------------------------------------

#define XMLTOOLS_VERSION_NUMBER L"3.1.1.11"
#define XMLTOOLS_HOMEPAGE_URL L"https://github.com/morbac/xmltools"
#ifdef V64BIT
#define XMLTOOLS_VERSION_STATUS L"unicode 64bit"
#else
#define XMLTOOLS_VERSION_STATUS L"unicode Win32"
#endif

extern ErrorEntryDesc displayXMLError(std::wstring wmsg, HWND view = NULL, size_t line = NULL, size_t linepos = NULL, size_t filepos = NULL, int forcedMode = ERRORS_DISPLAY_MODE_DEFAULT);
extern void displayXMLErrors(std::vector<ErrorEntryType> errors, HWND view = NULL, const wchar_t* szDesc = NULL, int forcedMode = ERRORS_DISPLAY_MODE_DEFAULT);
extern void clearErrors(HWND view = NULL);
extern void registerError(ErrorEntryDesc err);
extern void printCurrentXPathInStatusbar();

void savePluginParams();

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
