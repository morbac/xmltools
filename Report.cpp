// Report.cpp: implementation of the Report class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "PluginInterface.h"
#include "Report.h"
#include "menuCmdID.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

const int MAX_BUFFER = 4096;
std::wstring currentLog;
UniMode currentEncoding = UniMode::uniEnd;

Report::Report() {
}

Report::~Report() {
}

void Report::_printf_err(const wchar_t* s, ...) {
  if (!s || !wcslen(s)) return;

  va_list msg;
  wchar_t buffer[MAX_BUFFER];


  va_start(msg, s);
  _vsntprintf(buffer, MAX_BUFFER - 1, s, msg);
  va_end(msg);

  buffer[MAX_BUFFER - 1] = 0;
  if (wcslen(buffer) <= 0) return;

  ::MessageBox(nppData._nppHandle, buffer, L"XML Tools plugin", MB_OK | MB_ICONEXCLAMATION);
}

void Report::_fprintf_err(void * ctx, const wchar_t* s, ...) {
  if (!s || !wcslen(s)) return;

  va_list msg;
  wchar_t buffer[MAX_BUFFER];

  va_start(msg, s);
  _vsntprintf(buffer, MAX_BUFFER - 1, s, msg);
  buffer[MAX_BUFFER - 1] = 0;
  va_end(msg);

  if (wcslen(buffer) <= 0) return;

  ::MessageBox(nppData._nppHandle, buffer, L"XML Tools plugin", MB_OK | MB_ICONEXCLAMATION);
}

void Report::_printf_inf(const wchar_t* s, ...) {
  if (!s || !wcslen(s)) return;

  va_list msg;
  wchar_t buffer[MAX_BUFFER];

  va_start(msg, s);
  _vsntprintf(buffer, MAX_BUFFER - 1, s, msg);
  buffer[MAX_BUFFER - 1] = 0;
  va_end(msg);

  if (wcslen(buffer) <= 0) return;

  ::MessageBox(nppData._nppHandle, buffer, L"XML Tools plugin", MB_OK | MB_ICONINFORMATION);
}

void Report::_printf_inf(const std::wstring& ws) {
  ::MessageBox(nppData._nppHandle, ws.c_str(), L"XML Tools plugin", MB_OK | MB_ICONINFORMATION);
}

void Report::_printf_inf(const std::string& s) {
  ::MessageBox(nppData._nppHandle, Report::widen(s).c_str(), L"XML Tools plugin", MB_OK | MB_ICONINFORMATION);
}

void Report::_printf_err(const std::wstring& ws) {
  ::MessageBox(nppData._nppHandle, ws.c_str(), L"XML Tools plugin", MB_OK | MB_ICONEXCLAMATION);
}

void Report::_printf_err(const std::string& s) {
  ::MessageBox(nppData._nppHandle, Report::widen(s).c_str(), L"XML Tools plugin", MB_OK | MB_ICONEXCLAMATION);
}

void Report::_fprintf_inf(void * ctx, const wchar_t* s, ...) {
  if (!s || !wcslen(s)) return;

  va_list msg;
  wchar_t buffer[MAX_BUFFER];

  va_start(msg, s);
  _vsntprintf(buffer, MAX_BUFFER - 1, s, msg);
  buffer[MAX_BUFFER - 1] = 0;
  va_end(msg);

  if (wcslen(buffer) <= 0) return;

  ::MessageBox(nppData._nppHandle, buffer, L"XML Tools plugin", MB_OK | MB_ICONINFORMATION);
}

std::wstring Report::str_format(const wchar_t* s, ...) {
  if (!s || !wcslen(s)) return L"";

  va_list msg;
  wchar_t buffer[MAX_BUFFER];

  va_start(msg, s);
  _vsntprintf(buffer, MAX_BUFFER - 1, s, msg);
  buffer[MAX_BUFFER - 1] = 0;
  va_end(msg);

  if (wcslen(buffer) <= 0) return L"";

  return std::wstring(buffer);
}

std::string Report::str_format(const char* s, ...) {
  if (!s || !strlen(s)) return "";

  char buffer[MAX_BUFFER];

  va_list msg;
  va_start(msg, s);
  //vsnprintf(buffer + strlen(buffer), s, msg);
  _vsnprintf(buffer, MAX_BUFFER - 1, s, msg);
  buffer[MAX_BUFFER - 1] = 0;
  va_end(msg);

  if (strlen(buffer) <= 0) return "";

  return std::string(buffer);
}

CStringW Report::cstring(const wchar_t* s, ...) {
  if (!s || !wcslen(s)) return "";

  va_list msg;
  wchar_t buffer[MAX_BUFFER];

  va_start(msg, s);
  _vsntprintf(buffer, MAX_BUFFER - 1, s, msg);
  buffer[MAX_BUFFER-1] = 0;
  va_end(msg);

  if (wcslen(buffer) <= 0) return "";

  return CStringW(buffer);
}

void Report::clearLog() {
  currentLog.clear();
  /* @V3
  currentEncoding = Report::getEncoding();
  */
}

std::wstring Report::getLog() {
  return currentLog;
}

void Report::registerError(const char* s) {
  currentLog += L"ERROR: ";
  Report::appendToStdString(&currentLog, s, currentEncoding);
  currentLog = currentLog.substr(0, currentLog.length()-1);
  currentLog += L"\r\n";
}

void Report::registerError(void * ctx, const char* s, ...) {
  if (!s || !strlen(s)) return;

  va_list args;
  va_start(args, s);
  char *buffer = va_arg(args,char*);
  va_end(args);

  if (strlen(buffer) <= 0) return;

  Report::registerError(buffer);
}

void Report::registerWarn(const char* s) {
  currentLog += L"WARN: ";
  Report::appendToStdString(&currentLog, s, currentEncoding);
  currentLog = currentLog.substr(0, currentLog.length()-1);
  currentLog += L"\r\n";
}

void Report::registerWarn(void * ctx, const char* s, ...) {
  if (!s || !strlen(s)) return;

  va_list args;
  va_start(args, s);
  char *buffer = va_arg(args,char*);
  va_end(args);

  if (strlen(buffer) <= 0) return;

  Report::registerWarn(buffer);
}

void Report::registerMessage(const char* s) {
  Report::appendToStdString(&currentLog, s, currentEncoding);
  currentLog = currentLog.substr(0, currentLog.length()-1);
  currentLog += L"\r\n";
}
void Report::registerMessage(void * ctx, const char* s, ...) {
  if (!s || !strlen(s)) return;

  va_list args;
  va_start(args, s);
  char *buffer = va_arg(args,char*);
  va_end(args);

  Report::registerMessage(buffer);
}

void Report::strcpy(char* dest, const wchar_t* src) {
  Report::strcpy(dest, std::wstring(src));
}

void Report::strcpy(char* dest, std::wstring& src) {
  //strcpy(params[nbparams], key.c_str());
  wcstombs(dest, src.c_str(), src.length());
}

void Report::strcpy(wchar_t* dest, const wchar_t* src) {
  Report::strcpy(dest, std::wstring(src));
}

void Report::strcpy(wchar_t* dest, std::wstring& src) {
  //strcpy(params[nbparams], key.c_str());
  wcscpy(dest, src.c_str());
}

std::string Report::narrow(const std::wstring& ws) {
  size_t l = 4 * ws.length();
  char* tmp = new char[l];
  wcstombs(tmp, ws.c_str(), l);
  std::string res(tmp);
  delete[] tmp;
  return res;
}

std::wstring Report::widen(const char* s) {
  size_t len = strlen(s);
  std::wstring res(len, L' '); // Make room for characters
  std::copy(s, s+len, res.begin());
  return res;
}

std::wstring Report::widen(const std::string& s) {
  std::wstring res(s.length(), L' '); // Make room for characters
  std::copy(s.begin(), s.end(), res.begin());
  return res;
}

// TODO: Optimiser les fonctions suivantes
std::string Report::trimleft(const std::string& s) {
  if (s.empty()) return s;
  size_t i = 0, l = s.length();
  while (i < l && isspace(s.at(i))) ++i;
  if (i >= l) return "";
  else return s.substr(i);
}

std::string Report::trimright(const std::string& s) {
  if (s.empty()) return s;
  size_t i = s.length()-1;
  while (i >= 0 && isspace(s.at(i))) --i;
  if (i < 0) return "";
  else return s.substr(0, i+1);
}

std::string Report::trim(const std::string& s) {
  return Report::trimleft(Report::trimright(s));
}

// TODO: Optimiser les fonctions suivantes
std::wstring Report::wtrimleft(const std::wstring& s) {
  if (s.empty()) return s;
  size_t i = 0, l = s.length();
  while (i < l && isspace(s.at(i))) ++i;
  if (i >= l) return L"";
  else return s.substr(i);
}

std::wstring Report::wtrimright(const std::wstring& s) {
  if (s.empty()) return s;
  size_t i = s.length()-1;
  while (i >= 0 && isspace(s.at(i))) --i;
  if (i < 0) return L"";
  else return s.substr(0, i+1);
}

std::wstring Report::wtrim(const std::wstring& s) {
  return Report::wtrimleft(Report::wtrimright(s));
}

wchar_t* Report::char2wchar(const char* s) {
  size_t origsize = strlen(s) + 1;
  wchar_t* ws = new wchar_t[origsize];
  mbstowcs(ws, s, _TRUNCATE);
  return ws;
}

char* Report::wchar2char(const wchar_t* ws) {
  size_t origsize = wcslen(ws) + 1;
  char* s = new char[origsize];
  wcstombs(s, ws, _TRUNCATE);
  return s;
}

std::wstring Report::s2ws(const std::string& s) {
  int len;
  int slength = (int)s.length() + 1;
  len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0);
  wchar_t* buf = new wchar_t[len];
  MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
  std::wstring r(buf);
  delete[] buf;
  return r;
}

std::string Report::ws2s(const std::wstring& s) {
  int len;
  int slength = (int)s.length() + 1;
  len = WideCharToMultiByte(CP_ACP, 0, s.c_str(), slength, 0, 0, 0, 0);
  char* buf = new char[len];
  WideCharToMultiByte(CP_ACP, 0, s.c_str(), slength, buf, len, 0, 0);
  std::string r(buf);
  delete[] buf;
  return r;
}


UniMode Report::getEncoding(HWND npp) {
  HWND nppwnd = (npp == NULL ? nppData._nppHandle : npp);
  LRESULT bufferid = ::SendMessage(nppwnd, NPPM_GETCURRENTBUFFERID, 0, 0);
  return UniMode(::SendMessage(nppwnd, NPPM_GETBUFFERENCODING, bufferid, 0));
}

/*
  Npp mapping is following:
    uni8Bit   - IDM_FORMAT_ANSI
    uniCookie - IDM_FORMAT_AS_UTF_8
    uniUTF8   - IDM_FORMAT_UTF_8    (utf8 with bom)
    uni16BE   - IDM_FORMAT_UCS_2BE
    uni16LE   - IDM_FORMAT_UCS_2LE
*/
UniMode Report::getEncoding(BSTR encoding) {
  CStringW cstring(encoding);
  if (0 == cstring.Left(4).CompareNoCase(L"iso-")) {
    return UniMode::uni8Bit;
  }
  return UniMode::uniCookie;
}

void Report::setEncoding(UniMode encoding, HWND npp /* = NULL */) {
  HWND nppwnd = (npp == NULL ? nppData._nppHandle : npp);
  LRESULT bufferid = ::SendMessage(nppwnd, NPPM_GETCURRENTBUFFERID, 0, 0);
  UniMode(::SendMessage(nppwnd, NPPM_SETBUFFERENCODING, bufferid, (int)encoding));

  // uni8Bit=0, uniUTF8=1, uni16BE=2, uni16LE=3, uniCookie=4, uni7Bit=5, uni16BE_NoBOM=6, uni16LE_NoBOM=7
  switch (encoding) {
    case UniMode::uni8Bit: ::SendMessage(nppData._nppHandle, NPPM_MENUCOMMAND, 0, IDM_FORMAT_ANSI); break;
    case UniMode::uniCookie: ::SendMessage(nppData._nppHandle, NPPM_MENUCOMMAND, 0, IDM_FORMAT_AS_UTF_8); break;
    case UniMode::uniUTF8: ::SendMessage(nppData._nppHandle, NPPM_MENUCOMMAND, 0, IDM_FORMAT_UTF_8); break;
    case UniMode::uni16BE: ::SendMessage(nppData._nppHandle, NPPM_MENUCOMMAND, 0, IDM_FORMAT_UCS_2BE); break;
    case UniMode::uni16LE: ::SendMessage(nppData._nppHandle, NPPM_MENUCOMMAND, 0, IDM_FORMAT_UCS_2LE); break;
    default: ::SendMessage(nppData._nppHandle, NPPM_MENUCOMMAND, 0, IDM_FORMAT_ANSI); break;
  }
}

std::string Report::castChar(std::wstring text, UniMode encoding) {
  switch (encoding) {
  case UniMode::uniCookie:
  case UniMode::uniUTF8:
  case UniMode::uni16BE:
  case UniMode::uni16LE:
    // utf-8
    return Report::ucs2ToUtf8(text.c_str());
    break;
  default:
    // ansi
    return Report::ws2s(text);
    break;
  }

  return NULL;
}

wchar_t* Report::castChar(const char* orig, UniMode encoding /*= uniEnd*/) {
  UniMode enc = encoding;
  /* @V3
  if (encoding == uniEnd) {
    enc = Report::getEncoding();
  }
  */
  if (enc == UniMode::uni8Bit) {
    return Report::char2wchar(orig);
  } else {
    size_t osize = strlen(orig),
           wsize = 4*(osize+1);
    wchar_t* wbuffer = new wchar_t[wsize];
    memset(wbuffer, '\0', wsize);
    Report::UCS2FromUTF8(orig, static_cast<unsigned int>(osize+1), wbuffer, static_cast<unsigned int>(wsize));
    return wbuffer;
  }
}

char* Report::castWChar(const wchar_t* orig, UniMode encoding /*= uniEnd*/) {
  UniMode enc = encoding;
  /* @V3
  if (encoding == uniEnd) {
    enc = Report::getEncoding();
  }
  */
  if (enc == UniMode::uni8Bit) {
    return Report::wchar2char(orig);
  } else {
    size_t osize = wcslen(orig),
           size = 4*(osize+1);
    char* buffer = new char[size];
    memset(buffer, '\0', size);
    Report::UTF8FromUCS2(orig, static_cast<unsigned int>(osize+1), buffer, static_cast<unsigned int>(size));
    return buffer;
  }
}

void Report::appendToStdString(std::wstring* dest, const char* source, UniMode encoding) {
  if (source == NULL) {
    Report::appendToStdString(dest, "(null)", encoding);
  } else {
    wchar_t* buffer = Report::castChar(source, encoding);
    *dest += buffer;
    delete[] buffer;
  }
}

void Report::appendToCString(CStringW* dest, const char* source, UniMode encoding) {
  if (source == NULL) {
    Report::appendToCString(dest, "(null)", encoding);
  } else {
    wchar_t* buffer = Report::castChar(source, encoding);
    *dest += buffer;
    delete[] buffer;
  }
}

unsigned int Report::UTF8Length(const wchar_t *uptr, unsigned int tlen) {
  unsigned int len = 0;
  for (unsigned int i = 0; i < tlen && uptr[i]; ++i) {
    unsigned int uch = uptr[i];
    if (uch < 0x80)
      ++len;
    else if (uch < 0x800)
      len += 2;
    else
      len +=3;
  }
  return len;
}

void Report::UTF8FromUCS2(const wchar_t *uptr, unsigned int tlen, char *putf, unsigned int len) {
  int k = 0;
  for (unsigned int i = 0; i < tlen && uptr[i]; ++i) {
    unsigned int uch = uptr[i];
    if (uch < 0x80) {
      putf[k++] = static_cast<char>(uch);
    } else if (uch < 0x800) {
      putf[k++] = static_cast<char>(0xC0 | (uch >> 6));
      putf[k++] = static_cast<char>(0x80 | (uch & 0x3f));
    } else {
      putf[k++] = static_cast<char>(0xE0 | (uch >> 12));
      putf[k++] = static_cast<char>(0x80 | ((uch >> 6) & 0x3f));
      putf[k++] = static_cast<char>(0x80 | (uch & 0x3f));
    }
  }
  putf[len] = '\0';
}

unsigned int Report::UCS2Length(const char *s, unsigned int len) {
  unsigned int ulen = 0;
  for (unsigned int i=0; i<len; ++i) {
    UCHAR ch = static_cast<UCHAR>(s[i]);
    if ((ch < 0x80) || (ch > (0x80 + 0x40)))
      ++ulen;
  }
  return ulen;
}

unsigned int Report::UCS2FromUTF8(const char *s, unsigned int len, wchar_t *tbuf, unsigned int tlen) {
  unsigned int ui=0;
  const UCHAR *us = reinterpret_cast<const UCHAR *>(s);
  unsigned int i=0;
  while ((i<len) && (ui<tlen)) {
    UCHAR ch = us[i++];
    if (ch < 0x80) {
      tbuf[ui] = ch;
    } else if (ch < 0x80 + 0x40 + 0x20) {
      tbuf[ui] = static_cast<wchar_t>((ch & 0x1F) << 6);
      ch = us[i++];
      tbuf[ui] = static_cast<wchar_t>(tbuf[ui] + (ch & 0x7F));
    } else {
      tbuf[ui] = static_cast<wchar_t>((ch & 0xF) << 12);
      ch = us[i++];
      tbuf[ui] = static_cast<wchar_t>(tbuf[ui] + ((ch & 0x7F) << 6));
      ch = us[i++];
      tbuf[ui] = static_cast<wchar_t>(tbuf[ui] + (ch & 0x7F));
    }
    ui++;
  }
  return ui;
}
/*
unsigned int Report::ascii_to_utf8(const char * pszASCII, unsigned int lenASCII, char * pszUTF8) {
  // length of pszUTF8 must be enough;
  // its maximum is (lenASCII*3 + 1)

  if (!lenASCII || !pszASCII)
  {
    pszUTF8[0] = 0;
    return 0;
  }

  unsigned int lenUCS2;
  unsigned int lenUTF8;
  wchar_t *pszUCS2 = new wchar_t[lenASCII * 3 + 1];
  if (!pszUCS2)
  {
    pszUTF8[0] = 0;
    return 0;
  }

  lenUCS2 = ::MultiByteToWideChar(CP_ACP, 0, pszASCII, lenASCII, pszUCS2, lenASCII + 1);
  lenUTF8 = UTF8Length(pszUCS2, lenUCS2);
  // length of pszUTF8 must be >= (lenUTF8 + 1)
  UTF8FromUCS2(pszUCS2, lenUCS2, pszUTF8, lenUTF8);
  delete [] pszUCS2;
  return lenUTF8;
}
*/
/*
int Report::utf8_to_ascii(const char * pszUTF8, unsigned int lenUTF8, char * pszASCII) {
  // length of pszASCII must be enough;
  // its maximum is (lenUTF8 + 1)

  if (!lenUTF8 || !pszUTF8)
  {
    pszASCII[0] = 0;
    return 0;
  }

  unsigned int lenUCS2;
  wchar_t*     pszUCS2;

  pszUCS2 = new wchar_t[lenUTF8 + 1];
  if (!pszUCS2)
  {
    pszASCII[0] = 0;
    return 0;
  }

  lenUCS2 = UCS2FromUTF8(pszUTF8, lenUTF8, pszUCS2, lenUTF8);
  pszUCS2[lenUCS2] = 0;
  // length of pszASCII must be >= (lenUCS2 + 1)
  int nbByte = ::WideCharToMultiByte(CP_ACP, 0, pszUCS2, lenUCS2, pszASCII, lenUCS2 + 1, NULL, NULL);
  delete [] pszUCS2;
  return nbByte;
}
*/

/*
void Report::getEOLChar(HWND hwnd, int* eolmode, std::string* eolchar) {
  int eol = (int) ::SendMessage(hwnd, SCI_GETEOLMODE, 0, 0);
  *eolmode = eol;
  switch (eol) {
    case SC_EOL_CR:
      *eolchar = "\r";
      break;
    case SC_EOL_LF:
      *eolchar = "\n";
      break;
    case SC_EOL_CRLF:
    default:
      *eolchar = "\r\n";
  }
}
*/

/*
bool Report::isEOL(const std::string& txt, const std::string::size_type txtlength, unsigned int pos, int mode) {
  switch (mode) {
    case SC_EOL_CR:
      return (txt.at(pos) == '\r');
      break;
    case SC_EOL_LF:
      return (txt.at(pos) == '\n');
      break;
    case SC_EOL_CRLF:
    default:
      return (txtlength > pos && txt.at(pos) == '\r' && txt.at(pos+1) == '\n');
      break;
  }
}
*/

/*
bool Report::isEOL(const char cc, const char nc, int mode) {
  switch (mode) {
  case SC_EOL_CR:
    return (cc == '\r');
    break;
  case SC_EOL_LF:
    return (cc == '\n');
    break;
  case SC_EOL_CRLF:
  default:
    return (cc == '\r' && nc == '\n');
    break;
  }
}
*/

/*
void Report::char2BSTR(char* inParam, BSTR * outParam) {
  std::string tmp(inParam);
  *outParam = SysAllocString((Report::utf8ToUcs2(tmp)).c_str());
  tmp.clear();

}
*/

void Report::char2VARIANT(const char* inParam, VARIANT* outParam) {
  // from https://stackoverflow.com/questions/1822914/load-xml-into-c-msxml-from-byte-array?rq=1
  // https://docs.microsoft.com/en-us/previous-versions/aa468560(v=msdn.10)?redirectedfrom=MSDN
  size_t len = strlen(inParam);
  SAFEARRAYBOUND rgsabound[1];
  rgsabound[0].lLbound = 0;
  rgsabound[0].cElements = (ULONG) len;

  SAFEARRAY* psa = SafeArrayCreate(VT_UI1, 1, rgsabound);
  memcpy(psa->pvData, inParam, len);

  VariantInit(outParam);
  V_VT(outParam) = VT_ARRAY | VT_UI1;
  V_ARRAY(outParam) = psa;
}

void Report::char2BSTR(const CStringW& inParam, BSTR* outParam) {
  *outParam = SysAllocString(inParam);
}

// below methods got from https://github.com/RoelofBerg/Utf8Ucs2Converter

bool Report::utf8CharToUcs2Char(const char* utf8Tok, wchar_t* ucs2Char, uint32_t* utf8TokLen)
{
  //We do math, that relies on unsigned data types
  const unsigned char* utf8TokUs = reinterpret_cast<const unsigned char*>(utf8Tok);

  //Initialize return values for 'return false' cases.
  *ucs2Char = L'?';
  *utf8TokLen = 1;

  //Decode
  if (0x80 > utf8TokUs[0])
  {
    //Tokensize: 1 byte
    *ucs2Char = static_cast<const wchar_t>(utf8TokUs[0]);
  }
  else if (0xC0 == (utf8TokUs[0] & 0xE0))
  {
    //Tokensize: 2 bytes
    if (0x80 != (utf8TokUs[1] & 0xC0))
    {
      return false;
    }
    *utf8TokLen = 2;
    *ucs2Char = static_cast<const wchar_t>(
      (utf8TokUs[0] & 0x1F) << 6
      | (utf8TokUs[1] & 0x3F)
      );
  }
  else if (0xE0 == (utf8TokUs[0] & 0xF0))
  {
    //Tokensize: 3 bytes
    if ((0x80 != (utf8TokUs[1] & 0xC0))
      || (0x80 != (utf8TokUs[2] & 0xC0))
      )
    {
      return false;
    }
    *utf8TokLen = 3;
    *ucs2Char = static_cast<const wchar_t>(
      (utf8TokUs[0] & 0x0F) << 12
      | (utf8TokUs[1] & 0x3F) << 6
      | (utf8TokUs[2] & 0x3F)
      );
  }
  else if (0xF0 == (utf8TokUs[0] & 0xF8))
  {
    //Tokensize: 4 bytes
    *utf8TokLen = 4;
    return false;                        //Character exceeds the UCS-2 range (UCS-4 would be necessary)
  }
  else if (0xF8 == (utf8TokUs[0] & 0xFC))
  {
    //Tokensize: 5 bytes
    *utf8TokLen = 5;
    return false;                        //Character exceeds the UCS-2 range (UCS-4 would be necessary)
  }
  else if (0xFC == (utf8TokUs[0] & 0xFE))
  {
    //Tokensize: 6 bytes
    *utf8TokLen = 6;
    return false;                        //Character exceeds the UCS-2 range (UCS-4 would be necessary)
  }
  else
  {
    return false;
  }

  return true;
}

void Report::ucs2CharToUtf8Char(const wchar_t ucs2Char, char* utf8Tok)
{
  //We do math, that relies on unsigned data types
  uint32_t ucs2CharValue = static_cast<uint32_t>(ucs2Char);   //The standard doesn't specify the signed/unsignedness of wchar_t
  unsigned char* utf8TokUs = reinterpret_cast<unsigned char*>(utf8Tok);

  //Decode
  if (0x80 > ucs2CharValue)
  {
    //Tokensize: 1 byte
    utf8TokUs[0] = static_cast<unsigned char>(ucs2CharValue);
    utf8TokUs[1] = '\0';
  }
  else if (0x800 > ucs2CharValue)
  {
    //Tokensize: 2 bytes
    utf8TokUs[2] = '\0';
    utf8TokUs[1] = static_cast<unsigned char>(0x80 | (ucs2CharValue & 0x3F));
    ucs2CharValue = (ucs2CharValue >> 6);
    utf8TokUs[0] = static_cast<unsigned char>(0xC0 | ucs2CharValue);
  }
  else
  {
    //Tokensize: 3 bytes
    utf8TokUs[3] = '\0';
    utf8TokUs[2] = static_cast<unsigned char>(0x80 | (ucs2CharValue & 0x3F));
    ucs2CharValue = (ucs2CharValue >> 6);
    utf8TokUs[1] = static_cast<unsigned char>(0x80 | (ucs2CharValue & 0x3F));
    ucs2CharValue = (ucs2CharValue >> 6);
    utf8TokUs[0] = static_cast<unsigned char>(0xE0 | ucs2CharValue);
  }
}

std::wstring Report::utf8ToUcs2(const std::string& utf8Str)
{
  std::wstring ucs2Result;
  wchar_t ucs2CharToStrBuf[] = { 0, 0 };
  const char* cursor = utf8Str.c_str();
  const char* const end = utf8Str.c_str() + utf8Str.length();

  while (end > cursor)
  {
    uint32_t utf8TokLen = 1;
    utf8CharToUcs2Char(cursor, &ucs2CharToStrBuf[0], &utf8TokLen);
    ucs2Result.append(ucs2CharToStrBuf);
    cursor += utf8TokLen;
  }

  return ucs2Result;
}

std::string Report::ucs2ToUtf8(const std::wstring& ucs2Str)
{
  std::string utf8Result;
  char utf8Sequence[] = { 0, 0, 0, 0, 0 };
  const wchar_t* cursor = ucs2Str.c_str();
  const wchar_t* const end = ucs2Str.c_str() + ucs2Str.length();

  while (end > cursor)
  {
    const wchar_t ucs2Char = *cursor;
    ucs2CharToUtf8Char(ucs2Char, utf8Sequence);
    utf8Result.append(utf8Sequence);
    cursor++;
  }

  return utf8Result;
}