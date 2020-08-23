#include "StdAfx.h"

#include "Config.h"

void createDebugDlg();
void showDebugDlg();
void dbg(CStringW line, DBG_LEVEL level = DBG_LEVEL::DBG_TRACE);
void dbgln(CStringW line, DBG_LEVEL level = DBG_LEVEL::DBG_TRACE);