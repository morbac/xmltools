#pragma once
#include "StdAfx.h"

#include <string>

enum class DBG_LEVEL :int {
	DBG_TRACE = 0,
	DBG_INFO = 1,
	DBG_WARNING = 2,
	DBG_ERROR = 3
};

void createDebugDlg();
void showDebugDlg();
void dbg(const CStringW line, DBG_LEVEL level = DBG_LEVEL::DBG_TRACE);
void dbgln(const CStringW line, DBG_LEVEL level = DBG_LEVEL::DBG_TRACE);
void dbgln(const std::wstring &line, DBG_LEVEL level = DBG_LEVEL::DBG_TRACE);
void dbgln(const TCHAR *line, DBG_LEVEL level = DBG_LEVEL::DBG_TRACE);