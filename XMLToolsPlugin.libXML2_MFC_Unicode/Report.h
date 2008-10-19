// Report.h: interface for the Report class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_REPORT_H__A50BA8DF_F1C4_4E30_9CA7_59C3951C0981__INCLUDED_)
#define AFX_REPORT_H__A50BA8DF_F1C4_4E30_9CA7_59C3951C0981__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <sstream>

class Report  
{
public:
  Report();
  virtual ~Report();
  
  void static _printf_err(const TCHAR* s, ...);
  void static _printf_inf(const TCHAR* s, ...);
  
  void static _fprintf_err(void * ctx, const TCHAR* s, ...);
  void static _fprintf_inf(void * ctx, const TCHAR* s, ...);

  void static clearLog();
  CString static getLog();
  void static registerError(void * ctx, const TCHAR* s, ...);
  void static registerWarn(void * ctx, const TCHAR* s, ...);
  void static registerMessage(void * ctx, const TCHAR* s, ...);

  #define _printf _printf_inf

  std::wstring static str_format(const TCHAR* s, ...);
  CString static cstring(const TCHAR* s, ...);

  void static strcpy(char* dest, const TCHAR* src);
  void static strcpy(char* dest, std::wstring src);
  void static strcpy(TCHAR* dest, std::wstring src);
};

#endif // !defined(AFX_REPORT_H__A50BA8DF_F1C4_4E30_9CA7_59C3951C0981__INCLUDED_)
