// Report.cpp: implementation of the Report class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "PluginInterface.h"
#include "Report.h"
#include "menuCmdID.h"
#include "LoadLibrary.h"

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
UniMode currentEncoding = uniEnd;

Report::Report() {
}

Report::~Report() {
}

void Report::_printf_err(const wchar_t* s, ...) {
  if (!s || !wcslen(s)) return;

  va_list msg;
  wchar_t buffer[MAX_BUFFER] = { '\0' };

  va_start(msg, s);
  _vsntprintf(buffer, MAX_BUFFER - 1, s, msg);
  va_end(msg);

  if (wcslen(buffer) <= 0) return;

  ::MessageBox(nppData._nppHandle, buffer, L"XML Tools plugin", MB_OK | MB_ICONEXCLAMATION);
}

void Report::_fprintf_err(void * ctx, const wchar_t* s, ...) {
  if (!s || !wcslen(s)) return;
  
  va_list msg;
  wchar_t buffer[MAX_BUFFER] = { '\0' };
  
  va_start(msg, s);
  _vsntprintf(buffer, MAX_BUFFER - 1, s, msg);
  va_end(msg);
  
  if (wcslen(buffer) <= 0) return;
  
  ::MessageBox(nppData._nppHandle, buffer, L"XML Tools plugin", MB_OK | MB_ICONEXCLAMATION);
}

void Report::_printf_inf(const wchar_t* s, ...) {
  if (!s || !wcslen(s)) return;

  va_list msg;
  wchar_t buffer[MAX_BUFFER] = { '\0' };

  va_start(msg, s);
  _vsntprintf(buffer, MAX_BUFFER - 1, s, msg);
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

void Report::_fprintf_inf(void * ctx, const wchar_t* s, ...) {
  if (!s || !wcslen(s)) return;
  
  va_list msg;
  wchar_t buffer[MAX_BUFFER] = { '\0' };
  
  va_start(msg, s);
  _vsntprintf(buffer, MAX_BUFFER - 1, s, msg);
  va_end(msg);
  
  if (wcslen(buffer) <= 0) return;
  
  ::MessageBox(nppData._nppHandle, buffer, L"XML Tools plugin", MB_OK | MB_ICONINFORMATION);
}

std::wstring Report::str_format(const wchar_t* s, ...) {
  if (!s || !wcslen(s)) return L"";

  va_list msg;
  wchar_t buffer[MAX_BUFFER] = { '\0' };

  va_start(msg, s);
  _vsntprintf(buffer, MAX_BUFFER - 1, s, msg);
  va_end(msg);

  if (wcslen(buffer) <= 0) return L"";

  return std::wstring(buffer);
}

std::string Report::str_format(const char* s, ...) {
  if (!s || !strlen(s)) return "";
  
  size_t buffersize = 2*strlen(s);
  char * buffer = (char*) malloc(buffersize*sizeof(char));
  memset(buffer, '\0', buffersize);

  va_list msg;
  va_start(msg, s);
  //vsnprintf(buffer + strlen(buffer), s, msg);
  _vsnprintf(buffer, MAX_BUFFER - 1, s, msg);
  va_end(msg);

  if (strlen(buffer) <= 0) return "";

  return std::string(buffer);
}

CStringW Report::cstring(const wchar_t* s, ...) {
  if (!s || !wcslen(s)) return "";

  va_list msg;
  wchar_t buffer[MAX_BUFFER] = { '\0' };

  va_start(msg, s);
  _vsntprintf(buffer, MAX_BUFFER - 1, s, msg);
  va_end(msg);

  if (wcslen(buffer) <= 0) return "";

  return CStringW(buffer);
}

void Report::clearLog() {
  currentLog.clear();
  currentEncoding = Report::getEncoding();
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
  std::wstring s = std::wstring(src);
  std::copy(s.c_str(), s.c_str()+s.length()+1, dest);
}

void Report::strcpy(char* dest, std::wstring& src) {
  //strcpy(params[nbparams], key.c_str());
  std::copy(src.c_str(), src.c_str()+src.length()+1, dest);
}

void Report::strcpy(wchar_t* dest, const wchar_t* src) {
  std::wstring s = std::wstring(src);
  std::copy(s.c_str(), s.c_str()+s.length()+1, dest);
}

void Report::strcpy(wchar_t* dest, std::wstring& src) {
  //strcpy(params[nbparams], key.c_str());
  wcscpy(dest, src.c_str());
}

std::string Report::narrow(const std::wstring& ws) {
  std::string res(ws.length(), ' '); // Make room for characters
  std::copy(ws.begin(), ws.end(), res.begin());
  return res;
}

std::wstring Report::widen(const char* s) {
  size_t len = strlen(s);
  std::wstring res(len, L' '); // Make room for characters
  std::copy(s, s+len, res.begin());
  return res;
}

std::wstring Report::widen(const xmlChar* s) {
  size_t len = _mbslen(s);
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
  int i = 0, l = s.length();
  while (i < l && isspace(s.at(i))) ++i;
  if (i >= l) return "";
  else return s.substr(i);
}

std::string Report::trimright(const std::string& s) {
  if (s.empty()) return s;
  int i = s.length()-1;
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
  int i = 0, l = s.length();
  while (i < l && isspace(s.at(i))) ++i;
  if (i >= l) return L"";
  else return s.substr(i);
}

std::wstring Report::wtrimright(const std::wstring& s) {
  if (s.empty()) return s;
  int i = s.length()-1;
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

UniMode Report::getEncoding(HWND npp /* = NULL */) {
  return Report::getEncoding(XML_CHAR_ENCODING_NONE, npp);
}

UniMode Report::getEncoding(const xmlChar* xmlencoding, HWND npp) {
  if (xmlencoding != NULL) {
    xmlCharEncoding enc = pXmlParseCharEncoding(reinterpret_cast<const char*>(xmlencoding));
    return Report::getEncoding(enc, npp);
  } else {
    return Report::getEncoding(XML_CHAR_ENCODING_NONE, npp);
  }
}

UniMode Report::getEncoding(xmlCharEncoding xmlencoding, HWND npp) {
  if (xmlencoding == XML_CHAR_ENCODING_NONE) {
    HWND nppwnd = (npp == NULL ? nppData._nppHandle : npp);
    LRESULT bufferid = ::SendMessage(nppwnd, NPPM_GETCURRENTBUFFERID, 0, 0);
    return UniMode(::SendMessage(nppwnd, NPPM_GETBUFFERENCODING, bufferid, 0));
  } else {
    // uni8Bit=0, uniUTF8=1, uni16BE=2, uni16LE=3, uniCookie=4, uni7Bit=5, uni16BE_NoBOM=6, uni16LE_NoBOM=7

    switch (xmlencoding) {
      case XML_CHAR_ENCODING_UTF8: // UTF-8
        return uniUTF8;
        break;
      case XML_CHAR_ENCODING_UTF16LE: // UTF-16 little endian
        return uni16LE;
        break;
      case XML_CHAR_ENCODING_UTF16BE: // UTF-16 big endian
        return uni16BE;
        break;
      case XML_CHAR_ENCODING_UCS4LE: // UCS-4 little endian
      case XML_CHAR_ENCODING_UCS4_3412: // UCS-4 unusual ordering
        return uni16LE_NoBOM;
        break;
      case XML_CHAR_ENCODING_UCS4BE: // UCS-4 big endian
      case XML_CHAR_ENCODING_UCS4_2143: // UCS-4 unusual ordering
        return uni16BE_NoBOM;
        break;
      case XML_CHAR_ENCODING_ASCII:  // pure ASCII
        return uni8Bit;
        break;
      /*case XML_CHAR_ENCODING_EBCDIC: // EBCDIC uh!
        break;
      
      case XML_CHAR_ENCODING_UCS2: // UCS-2
        break;
      case XML_CHAR_ENCODING_8859_1: // ISO-8859-1 ISO Latin 1
        break;
      case XML_CHAR_ENCODING_8859_2: // ISO-8859-2 ISO Latin 2
        break;
      case XML_CHAR_ENCODING_8859_3: // ISO-8859-3
        break;
      case XML_CHAR_ENCODING_8859_4: // ISO-8859-4
        break;
      case XML_CHAR_ENCODING_8859_5: // ISO-8859-5
        break;
      case XML_CHAR_ENCODING_8859_6: // ISO-8859-6
        break;
      case XML_CHAR_ENCODING_8859_7: // ISO-8859-7
        break;
      case XML_CHAR_ENCODING_8859_8: // ISO-8859-8
        break;
      case XML_CHAR_ENCODING_8859_9: // ISO-8859-9
        break;
      case XML_CHAR_ENCODING_2022_JP: // ISO-2022-JP
        break;
      case XML_CHAR_ENCODING_SHIFT_JIS: // Shift_JIS
        break;
      case XML_CHAR_ENCODING_EUC_JP: // EUC-JP
        break;
      
      case XML_CHAR_ENCODING_ERROR: // No char encoding detected
      case XML_CHAR_ENCODING_NONE: // No char encoding detected
      */
      default:
        return uniUTF8;

    }
  }
}

void Report::setEncoding(UniMode encoding, HWND npp /* = NULL */) {
  HWND nppwnd = (npp == NULL ? nppData._nppHandle : npp);
  int bufferid = int(::SendMessage(nppwnd, NPPM_GETCURRENTBUFFERID, 0, 0));
  UniMode(::SendMessage(nppwnd, NPPM_SETBUFFERENCODING, bufferid, encoding));
  
  // uni8Bit=0, uniUTF8=1, uni16BE=2, uni16LE=3, uniCookie=4, uni7Bit=5, uni16BE_NoBOM=6, uni16LE_NoBOM=7
  switch (encoding) {
    case uni8Bit: ::SendMessage(nppData._nppHandle, NPPM_MENUCOMMAND, 0, IDM_FORMAT_ANSI); break;
    case uniUTF8: ::SendMessage(nppData._nppHandle, NPPM_MENUCOMMAND, 0, IDM_FORMAT_UTF_8); break;
    case uni16BE: ::SendMessage(nppData._nppHandle, NPPM_MENUCOMMAND, 0, IDM_FORMAT_UCS_2BE); break;
    case uni16LE: ::SendMessage(nppData._nppHandle, NPPM_MENUCOMMAND, 0, IDM_FORMAT_UCS_2LE); break;
    case uniCookie: 
    default: ::SendMessage(nppData._nppHandle, NPPM_MENUCOMMAND, 0, IDM_FORMAT_AS_UTF_8); break;
  }
}

wchar_t* Report::castChar(const char* orig, UniMode encoding /*= uniEnd*/) {
  UniMode enc = encoding;
  if (encoding == uniEnd) {
    enc = Report::getEncoding();
  }
  if (enc == uni8Bit) {
    return Report::char2wchar(orig);
  } else {
    size_t osize = strlen(orig),
           wsize = 4*(osize+1);
    wchar_t* wbuffer = new wchar_t[wsize];
    memset(wbuffer, '\0', wsize);
    Report::UCS2FromUTF8(orig, osize+1, wbuffer, wsize);
    return wbuffer;
  }
}

char* Report::castWChar(const wchar_t* orig, UniMode encoding /*= uniEnd*/) {
  UniMode enc = encoding;
  if (encoding == uniEnd) {
    enc = Report::getEncoding();
  }
  if (enc == uni8Bit) {
    return Report::wchar2char(orig);
  } else {
    size_t osize = wcslen(orig),
           size = 4*(osize+1);
    char* buffer = new char[size];
    memset(buffer, '\0', size);
    Report::UTF8FromUCS2(orig, osize+1, buffer, size);
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

void Report::appendToWStdString(std::wstring* dest, const xmlChar* source, UniMode encoding) {
  if (source == NULL) {
    Report::appendToStdString(dest, "(null)", encoding);
  } else {
    wchar_t* buffer = Report::castChar(reinterpret_cast<const char*>(source), encoding);
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

void Report::appendToCString(CStringW* dest, const xmlChar* source, UniMode encoding) {
  if (source == NULL) {
    Report::appendToCString(dest, "(null)", encoding);
  } else {
    wchar_t* buffer = Report::castChar(reinterpret_cast<const char*>(source), encoding);
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

CStringA Report::UTF16toUTF8(const CStringW& utf16) {
  return CW2A(utf16, CP_UTF8);
}

CStringW Report::UTF8toUTF16(const CStringA& utf8) {
  return CA2W(utf8, CP_UTF8);
}

void Report::getEOLChar(HWND hwnd, int* eolmode, std::string* eolchar) {
  *eolmode = ::SendMessage(hwnd, SCI_GETEOLMODE , 0, 0);
  switch (*eolmode) {
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