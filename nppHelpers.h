#pragma once
#include "StdAfx.h"
#include "PluginInterface.h"

extern int initDocIterator();
extern bool hasNextDoc(int* iter);
extern NppData nppData;
extern HWND getCurrentHScintilla(int which);

#include "ScintillaDoc.h"
void nppMultiDocumentCommand(const std::wstring &debugname, void (*action)(ScintillaDoc&));
void nppDocumentCommand(const std::wstring& debugname, void (*action)(ScintillaDoc&));