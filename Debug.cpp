#include "StdAfx.h"
#include "DebugDlg.h"


CDebugDlg* debugdlg = new CDebugDlg();

void createDebugDlg() {
    debugdlg->Create(CDebugDlg::IDD, NULL);
}

void showDebugDlg() {
    debugdlg->ShowWindow(SW_SHOW);
}

void dbg(CStringW line) {
    //#ifdef DEBUG
    debugdlg->addLine(line);
    //#endif
}

void dbgln(CStringW line) {
    //#ifdef DEBUG
    debugdlg->addLine(line + "\r\n");
    //#endif
}

/*
// get given language as string (for debug purposes only)
char* getLangType(LangType lg) {
  if (lg == L_TEXT        ) return "L_TEXT";
  if (lg == L_PHP         ) return "L_PHP";
  if (lg == L_C           ) return "L_C";
  if (lg == L_CPP         ) return "L_CPP";
  if (lg == L_CS          ) return "L_CS";
  if (lg == L_OBJC        ) return "L_OBJC";
  if (lg == L_JAVA        ) return "L_JAVA";
  if (lg == L_RC          ) return "L_RC";
  if (lg == L_HTML        ) return "L_HTML";
  if (lg == L_XML         ) return "L_XML";
  if (lg == L_MAKEFILE    ) return "L_MAKEFILE";
  if (lg == L_PASCAL      ) return "L_PASCAL";
  if (lg == L_BATCH       ) return "L_BATCH";
  if (lg == L_INI         ) return "L_INI";
  if (lg == L_ASCII       ) return "L_ASCII";
  if (lg == L_USER        ) return "L_USER";
  if (lg == L_ASP         ) return "L_ASP";
  if (lg == L_SQL         ) return "L_SQL";
  if (lg == L_VB          ) return "L_VB";
  if (lg == L_JS          ) return "L_JS";
  if (lg == L_CSS         ) return "L_CSS";
  if (lg == L_PERL        ) return "L_PERL";
  if (lg == L_PYTHON      ) return "L_PYTHON";
  if (lg == L_LUA         ) return "L_LUA";
  if (lg == L_TEX         ) return "L_TEX";
  if (lg == L_FORTRAN     ) return "L_FORTRAN";
  if (lg == L_BASH        ) return "L_BASH";
  if (lg == L_FLASH       ) return "L_FLASH";
  if (lg == L_NSIS        ) return "L_NSIS";
  if (lg == L_TCL         ) return "L_TCL";
  if (lg == L_LISP        ) return "L_LISP";
  if (lg == L_SCHEME      ) return "L_SCHEME";
  if (lg == L_ASM         ) return "L_ASM";
  if (lg == L_DIFF        ) return "L_DIFF";
  if (lg == L_CAML        ) return "L_CAML";
  if (lg == L_ADA         ) return "L_ADA";
  if (lg == L_VERILOG     ) return "L_VERILOG";
  if (lg == L_MATLAB      ) return "L_MATLAB";
  if (lg == L_HASKELL     ) return "L_HASKELL";
  if (lg == L_INNO        ) return "L_INNO";
  if (lg == L_SEARCHRESULT) return "L_SEARCHRESULT";
  if (lg == L_CMAKE       ) return "L_CMAKE";
  if (lg == L_YAML        ) return "L_YAML";
  if (lg == L_COBOL       ) return "L_COBOL";
  if (lg == L_GUI4CLI     ) return "L_GUI4CLI";
  if (lg == L_D           ) return "L_D";
  if (lg == L_POWERSHELL  ) return "L_POWERSHELL";
  if (lg == L_R           ) return "L_R";
  if (lg == L_JSP         ) return "L_JSP";
  if (lg == L_COFFEESCRIPT) return "L_COFFEESCRIPT";
  if (lg == L_EXTERNAL    ) return "L_EXTERNAL";

  return "";
}
*/