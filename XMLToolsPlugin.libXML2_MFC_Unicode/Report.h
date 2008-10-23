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
  
  void static _printf_err(const wchar_t* s, ...);
  void static _printf_inf(const wchar_t* s, ...);
  
  void static _fprintf_err(void * ctx, const wchar_t* s, ...);
  void static _fprintf_inf(void * ctx, const wchar_t* s, ...);

  void static clearLog();
  CString static getLog();
  void static registerError(void * ctx, const wchar_t* s, ...);
  void static registerWarn(void * ctx, const wchar_t* s, ...);
  void static registerMessage(void * ctx, const wchar_t* s, ...);

  #define _printf _printf_inf

  std::wstring static str_format(const wchar_t* s, ...);
  CString static cstring(const wchar_t* s, ...);

  void static strcpy(char* dest, const wchar_t* src);
  void static strcpy(char* dest, std::wstring src);
  void static strcpy(wchar_t* dest, std::wstring src);

  std::string static narrow(const std::wstring& ws);
  std::wstring static widen(const char* s);
  std::wstring static widen(const std::string& s);
};

#endif // !defined(AFX_REPORT_H__A50BA8DF_F1C4_4E30_9CA7_59C3951C0981__INCLUDED_)
