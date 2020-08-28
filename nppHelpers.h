#pragma once
#include "StdAfx.h"
#include "PluginInterface.h"

extern int initDocIterator();
extern bool hasNextDoc(int* iter);
extern NppData nppData;
extern HWND getCurrentHScintilla(int which);

#include "ScintillaDoc.h"
void nppDocumentCommand(char* debugname, void (*action)(ScintillaDoc&));