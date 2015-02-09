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

// from NPP source "Parameters.h"
enum UniMode {uni8Bit=0, uniUTF8=1, uni16BE=2, uni16LE=3, uniCookie=4, uni7Bit=5, uni16BE_NoBOM=6, uni16LE_NoBOM=7, uniEnd};

// edit : look at https://msdn.microsoft.com/en-us/library/ms235631(VS.80).aspx
class Report  
{
public:
  Report();
  virtual ~Report();
  
  static void _printf_err(const wchar_t* s, ...);
  static void _printf_inf(const wchar_t* s, ...);
  static void _printf_inf(const std::wstring& ws);
  static void _printf_inf(const std::string& s);
  
  static void _fprintf_err(void * ctx, const wchar_t* s, ...);
  static void _fprintf_inf(void * ctx, const wchar_t* s, ...);

  static void clearLog();
  static std::wstring getLog();

  static void registerError(const char* s);
  static void registerWarn(const char* s);
  static void registerMessage(const char* s);

  static void registerError(void * ctx, const char* s, ...);
  static void registerWarn(void * ctx, const char* s, ...);
  static void registerMessage(void * ctx, const char* s, ...);

  #define _printf _printf_inf

  static std::wstring str_format(const wchar_t* s, ...);
  static std::string str_format(const char* s, ...);
  static CString cstring(const wchar_t* s, ...);

  static void strcpy(char* dest, const wchar_t* src);
  static void strcpy(char* dest, std::wstring src);
  static void strcpy(wchar_t* dest, std::wstring src);

  static std::string narrow(const std::wstring& ws);
  static std::wstring widen(const char* s);
  static std::wstring widen(const xmlChar* s);
  static std::wstring widen(const std::string& s);

  static std::string trimleft(const std::string& s);
  static std::string trimright(const std::string& s);
  static std::string trim(const std::string& s);

  static std::wstring wtrimleft(const std::wstring& s);
  static std::wstring wtrimright(const std::wstring& s);
  static std::wstring wtrim(const std::wstring& s);

  static wchar_t* char2wchar(const char* s);
  static char* wchar2char(const wchar_t* ws);

  static unsigned int UTF8Length(const wchar_t * uptr, unsigned int tlen);
  static void UTF8FromUCS2(const wchar_t * uptr, unsigned int tlen, char * putf, unsigned int len);
  static unsigned int UCS2Length(const char * s, unsigned int len);
  static unsigned int UCS2FromUTF8(const char * s, unsigned int len, wchar_t * tbuf, unsigned int tlen);
  static unsigned int ascii_to_utf8(const char * pszASCII, unsigned int lenASCII, char * pszUTF8);
  static int utf8_to_ascii(const char * pszUTF8, unsigned int lenUTF8, char * pszASCII);
};

#endif // !defined(AFX_REPORT_H__A50BA8DF_F1C4_4E30_9CA7_59C3951C0981__INCLUDED_)
