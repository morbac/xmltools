// Report.cpp: implementation of the Report class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "PluginInterface.h"
#include "Report.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CString currentLog;

Report::Report() {
}

Report::~Report() {
}

void Report::_printf_err(const TCHAR* s, ...) {
  if (!s || !wcslen(s)) return;

  va_list msg;
  const int MAX_BUFFER = 1024;
  TCHAR buffer[MAX_BUFFER] = { '\0' };

  va_start(msg, s);
  _vsntprintf(buffer, MAX_BUFFER - 1, s, msg);
  va_end(msg);

  if (wcslen(buffer) <= 0) return;

  ::MessageBox(nppData._nppHandle, buffer, TEXT("XML Tools plugin"), MB_OK | MB_ICONEXCLAMATION);
}

void Report::_fprintf_err(void * ctx, const TCHAR* s, ...) {
  if (!s || !wcslen(s)) return;
  
  va_list msg;
  const int MAX_BUFFER = 1024;
  TCHAR buffer[MAX_BUFFER] = { '\0' };
  
  va_start(msg, s);
  _vsntprintf(buffer, MAX_BUFFER - 1, s, msg);
  va_end(msg);
  
  if (wcslen(buffer) <= 0) return;
  
  ::MessageBox(nppData._nppHandle, buffer, TEXT("XML Tools plugin"), MB_OK | MB_ICONEXCLAMATION);
}

void Report::_printf_inf(const TCHAR* s, ...) {
  if (!s || !wcslen(s)) return;

  va_list msg;
  const int MAX_BUFFER = 1024;
  TCHAR buffer[MAX_BUFFER] = { '\0' };

  va_start(msg, s);
  _vsntprintf(buffer, MAX_BUFFER - 1, s, msg);
  va_end(msg);

  if (wcslen(buffer) <= 0) return;

  ::MessageBox(nppData._nppHandle, buffer, TEXT("XML Tools plugin"), MB_OK | MB_ICONINFORMATION);
}

void Report::_fprintf_inf(void * ctx, const TCHAR* s, ...) {
  if (!s || !wcslen(s)) return;
  
  va_list msg;
  const int MAX_BUFFER = 1024;
  TCHAR buffer[MAX_BUFFER] = { '\0' };
  
  va_start(msg, s);
  _vsntprintf(buffer, MAX_BUFFER - 1, s, msg);
  va_end(msg);
  
  if (wcslen(buffer) <= 0) return;
  
  ::MessageBox(nppData._nppHandle, buffer, TEXT("XML Tools plugin"), MB_OK | MB_ICONINFORMATION);
}

std::wstring Report::str_format(const TCHAR* s, ...) {
  if (!s || !wcslen(s)) return TEXT("");

  va_list msg;
  const int MAX_BUFFER = 1024;
  TCHAR buffer[MAX_BUFFER] = { '\0' };

  va_start(msg, s);
  _vsntprintf(buffer, MAX_BUFFER - 1, s, msg);
  va_end(msg);

  if (wcslen(buffer) <= 0) return TEXT("");

  return std::wstring(buffer);
}

CString Report::cstring(const TCHAR* s, ...) {
  if (!s || !wcslen(s)) return "";

  va_list msg;
  const int MAX_BUFFER = 1024;
  TCHAR buffer[MAX_BUFFER] = { '\0' };

  va_start(msg, s);
  _vsntprintf(buffer, MAX_BUFFER - 1, s, msg);
  va_end(msg);

  if (wcslen(buffer) <= 0) return "";

  return CString(buffer);
}

void Report::clearLog() {
  currentLog.Empty();
}

CString Report::getLog() {
  return currentLog;
}

void Report::registerError(void * ctx, const TCHAR* s, ...) {
  if (!s || !wcslen(s)) return;
  
  va_list msg;
  const int MAX_BUFFER = 1024;
  TCHAR buffer[MAX_BUFFER] = { '\0' };
  
  va_start(msg, s);
  _vsntprintf(buffer, MAX_BUFFER - 1, s, msg);
  va_end(msg);
  
  if (wcslen(buffer) <= 0) return;
  
  currentLog += "ERROR: ";
  currentLog += buffer;
  currentLog = currentLog.Mid(0, currentLog.GetLength()-1);
  currentLog += "\r\n";
}

void Report::registerWarn(void * ctx, const TCHAR* s, ...) {
  if (!s || !wcslen(s)) return;
  
  va_list msg;
  const int MAX_BUFFER = 1024;
  TCHAR buffer[MAX_BUFFER] = { '\0' };
  
  va_start(msg, s);
  _vsntprintf(buffer, MAX_BUFFER - 1, s, msg);
  va_end(msg);
  
  if (wcslen(buffer) <= 0) return;
  
  currentLog += "WARN: ";
  currentLog += buffer;
  currentLog = currentLog.Mid(0, currentLog.GetLength()-1);
  currentLog += "\r\n";
}

void Report::registerMessage(void * ctx, const TCHAR* s, ...) {
  if (!s || !wcslen(s)) return;
  
  va_list msg;
  const int MAX_BUFFER = 1024;
  TCHAR buffer[MAX_BUFFER] = { '\0' };
  
  va_start(msg, s);
  _vsntprintf(buffer, MAX_BUFFER - 1, s, msg);
  va_end(msg);
  
  if (wcslen(buffer) <= 0) return;
  
  currentLog += buffer;
  currentLog = currentLog.Mid(0, currentLog.GetLength()-1);
  currentLog += "\r\n";
}

void Report::strcpy(char* dest, const TCHAR* src) {
  std::wstring s = std::wstring(src);
  std::copy(s.c_str(), s.c_str()+s.length()+1, dest);
}

void Report::strcpy(char* dest, std::wstring src) {
  //strcpy(params[nbparams], key.c_str());
	std::copy(src.c_str(), src.c_str()+src.length()+1, dest);
}

void Report::strcpy(TCHAR* dest, std::wstring src) {
    //strcpy(params[nbparams], key.c_str());
	std::copy(src.c_str(), src.c_str()+src.length()+1, dest);
}