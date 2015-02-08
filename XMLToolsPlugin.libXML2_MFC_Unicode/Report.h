// Report.h: interface for the Report class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_REPORT_H__A50BA8DF_F1C4_4E30_9CA7_59C3951C0981__INCLUDED_)
#define AFX_REPORT_H__A50BA8DF_F1C4_4E30_9CA7_59C3951C0981__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <sstream>
#include <libxml/xmlstring.h>

// edit : look at https://msdn.microsoft.com/en-us/library/ms235631(VS.80).aspx
class Report  
{
public:
  Report();
  virtual ~Report();
  
  void static _printf_err(const wchar_t* s, ...);
  void static _printf_inf(const wchar_t* s, ...);
  void static _printf_inf(const std::wstring& ws);
  void static _printf_inf(const std::string& s);
  
  void static _fprintf_err(void * ctx, const wchar_t* s, ...);
  void static _fprintf_inf(void * ctx, const wchar_t* s, ...);

  void static clearLog();
  std::wstring static getLog();

  void static registerError(const char* s);
  void static registerWarn(const char* s);
  void static registerMessage(const char* s);

  void static registerError(void * ctx, const char* s, ...);
  void static registerWarn(void * ctx, const char* s, ...);
  void static registerMessage(void * ctx, const char* s, ...);

  #define _printf _printf_inf

  std::wstring static str_format(const wchar_t* s, ...);
  std::string static str_format(const char* s, ...);
  CString static cstring(const wchar_t* s, ...);

  void static strcpy(char* dest, const wchar_t* src);
  void static strcpy(char* dest, std::wstring src);
  void static strcpy(wchar_t* dest, std::wstring src);

  std::string static narrow(const std::wstring& ws);
  std::wstring static widen(const char* s);
  std::wstring static widen(const xmlChar* s);
  std::wstring static widen(const std::string& s);

  std::string static trimleft(const std::string& s);
  std::string static trimright(const std::string& s);
  std::string static trim(const std::string& s);

  std::wstring static wtrimleft(const std::wstring& s);
  std::wstring static wtrimright(const std::wstring& s);
  std::wstring static wtrim(const std::wstring& s);

  unsigned int static UTF8Length(const wchar_t * uptr, unsigned int tlen);
  void static UTF8FromUCS2(const wchar_t * uptr, unsigned int tlen, char * putf, unsigned int len);
  unsigned int static UCS2Length(const char * s, unsigned int len);
  unsigned int static UCS2FromUTF8(const char * s, unsigned int len, wchar_t * tbuf, unsigned int tlen);
  unsigned int static ascii_to_utf8(const char * pszASCII, unsigned int lenASCII, char * pszUTF8);
  int static utf8_to_ascii(const char * pszUTF8, unsigned int lenUTF8, char * pszASCII);
};

#endif // !defined(AFX_REPORT_H__A50BA8DF_F1C4_4E30_9CA7_59C3951C0981__INCLUDED_)
