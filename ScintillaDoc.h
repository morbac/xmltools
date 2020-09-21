#pragma once
#include "StdAfx.h"

#include <string>

struct ScintillaDoc
{
	HWND hCurrentEditView;

	bool inSelection;

	struct sciWorkText
	{
		char* text;
		long length;
		long selstart;

		operator bool() {
			return text != NULL;
		}

		void FreeMemory() {
			if (text) {
				delete[] text;
				text = NULL;
			}
		}
	};

	ScintillaDoc(HWND scHandle) {
		hCurrentEditView = scHandle;
		inSelection = false;
	}

	bool IsReadOnly() {
		return 0 != (int) ::SendMessage(hCurrentEditView, SCI_GETREADONLY, 0, 0);
	}

	int GetTabWidth() {
		int tabwidth = (int) ::SendMessage(hCurrentEditView, SCI_GETTABWIDTH, 0, 0);
		if (tabwidth <= 0) tabwidth = 4;
		
		return tabwidth;
	}

	bool UseTabs() {
		return (bool) ::SendMessage(hCurrentEditView, SCI_GETUSETABS, 0, 0);
	}

	std::string Tab()
	{
		if (UseTabs())
		{
			return "\t";
		}
		else
		{
			std::string tab;
			int tabwidth = GetTabWidth();
			for (int i = 0; i < tabwidth; i++)
				tab.append(" ");

			return tab;
		}
	}
	
	std::string EOL() {
		int eol = (int) ::SendMessage(hCurrentEditView, SCI_GETEOLMODE, 0, 0);
		switch (eol) {
			case SC_EOL_CR:		return "\r";
			case SC_EOL_LF:		return "\n";
			case SC_EOL_CRLF:
			default:			return "\r\n";
		}
	}

	long SelectionStart() {
		return (long) ::SendMessage(hCurrentEditView, SCI_GETSELECTIONSTART, 0, 0);
	}

	long SelectionEnd() {
		return (long) ::SendMessage(hCurrentEditView, SCI_GETSELECTIONEND, 0, 0);
	}

	void SetCurrentPosition(size_t selstart) {
		::SendMessage(hCurrentEditView, SCI_SETCURRENTPOS, selstart, 0);
	}

	void SetAnchor(size_t selend) {
		::SendMessage(hCurrentEditView, SCI_SETANCHOR, selend, 0);
	}

	void SetText(const char* text) {
		::SendMessage(hCurrentEditView, SCI_SETTEXT, 0, reinterpret_cast<LPARAM>(text));
	}

	void ReplaceSelection(const char* text) {
		::SendMessage(hCurrentEditView, SCI_REPLACESEL, 0, reinterpret_cast<LPARAM>(text));
	}

	long GetTextLength() {
		return (long) ::SendMessage(hCurrentEditView, SCI_GETLENGTH, 0, 0);
	}

	char *GetSelectedText(long length) {
		char *text = new char[length + sizeof(char)];
		if (text == NULL) return false;  // allocation error, abort check;
		text[length] = 0;
		::SendMessage(hCurrentEditView, SCI_GETSELTEXT, 0, reinterpret_cast<LPARAM>(text));
		return text;
	}

	sciWorkText GetSelectedText() {
		auto selstart = SelectionStart();
		auto selend = SelectionEnd();
		if (selend > selstart)
		{
			auto len = selend - selstart;
			return { GetSelectedText(len), len, selstart };
		}
		return { NULL,0,0 };
	}

	char *GetText(long length) {
		char *text = new char[length + sizeof(char)];
		if (text == NULL) return false;  // allocation error, abort check
		text[length] = 0;
		::SendMessage(hCurrentEditView, SCI_GETTEXT, length + sizeof(char), reinterpret_cast<LPARAM>(text));
		return text;
	}

	size_t GetText(Sci_PositionCR offset, char *target, Sci_PositionCR length) {
		if (target == NULL) return 0;  // allocation error, abort check
		auto max = GetTextLength();

		if (offset >= max) return 0;

		Sci_TextRange range;
		range.chrg.cpMin = offset;
		range.chrg.cpMax = offset+length;
		range.lpstrText = target;

		if (range.chrg.cpMax > (int)max)
			range.chrg.cpMax = (int)max;
		

		return ::SendMessage(hCurrentEditView, SCI_GETTEXTRANGE, 0, reinterpret_cast<LPARAM>(&range));
	}

	sciWorkText GetWorkText() {
		auto selstart = this->SelectionStart();
		auto selend = this->SelectionEnd();

		sciWorkText ret;

		if (selend > selstart) { // selection
			ret.length = selend - selstart;
			inSelection = true;
			ret.text = this->GetSelectedText(ret.length);
			ret.selstart = selstart;
		}
		else {
			ret.length = this->GetTextLength();
			inSelection = false;
			ret.text = this->GetText(ret.length);
			ret.selstart = -1;
		}

		return ret;
	}

	void SetWorkText(const char* text) {
		if (inSelection) {
			this->ReplaceSelection(text);
		}
		else {
			this->SetText(text);
		}
	}

	void SetXOffset(int offset = 0) {
		::SendMessage(hCurrentEditView, SCI_SETXOFFSET, offset, 0);
	}

	void SetScrollWidth(int width) {
		::SendMessage(hCurrentEditView, SCI_SETSCROLLWIDTH, width, 0);
	}
};
