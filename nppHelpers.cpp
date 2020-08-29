#include "StdAfx.h"
#include "nppHelpers.h"
#include "XMLTools.h"

HWND getCurrentHScintilla(int which) {
    return (which == 0) ? nppData._scintillaMainHandle : nppData._scintillaSecondHandle;
};

int nbopenfiles1, nbopenfiles2;

int initDocIterator() {
    dbgln("initDocIterator()");

    nbopenfiles1 = (int) ::SendMessage(nppData._nppHandle, NPPM_GETNBOPENFILES, 0, PRIMARY_VIEW);
    nbopenfiles2 = (int) ::SendMessage(nppData._nppHandle, NPPM_GETNBOPENFILES, 0, SECOND_VIEW);

    if (::SendMessage(nppData._nppHandle, NPPM_GETCURRENTDOCINDEX, 0, MAIN_VIEW) < 0) nbopenfiles1 = 0;
    if (::SendMessage(nppData._nppHandle, NPPM_GETCURRENTDOCINDEX, 0, SUB_VIEW) < 0) nbopenfiles2 = 0;

    //Report::_printf_inf(Report::str_format("%d %d",nbopenfiles1,nbopenfiles2));

    return 0;
}

bool hasNextDoc(int* iter) {
    dbgln("hasNextDoc()");

    if (config.doPrettyPrintAllOpenFiles) {
        if (*iter < 0 || *iter >= (nbopenfiles1 + nbopenfiles2)) return false;

        if (*iter < nbopenfiles1) {
            ::SendMessage(nppData._nppHandle, NPPM_ACTIVATEDOC, MAIN_VIEW, (*iter));
        }
        else if (*iter >= nbopenfiles1 && *iter < nbopenfiles1 + nbopenfiles2) {
            auto idx = (*iter) - nbopenfiles1;
            ::SendMessage(nppData._nppHandle, NPPM_ACTIVATEDOC, SUB_VIEW, idx);
        }
        else {
            return false;
        }

        ++(*iter);
        return true;
    }
    else {
        ++(*iter);
        return ((*iter) == 1);
    }
}


void nppMultiDocumentCommand(const std::wstring &debugname, void (*action)(ScintillaDoc&)) {
    dbgln(L"+ nppMultiDocumentCommand(\"" + debugname + L"\")");

    auto clock_start = clock();

    int docIterator = initDocIterator();
    while (hasNextDoc(&docIterator)) {
        int currentEdit;
        ::SendMessage(nppData._nppHandle, NPPM_GETCURRENTSCINTILLA, 0, (LPARAM)&currentEdit);

        ScintillaDoc doc = ScintillaDoc(getCurrentHScintilla(currentEdit));

        if (doc.IsReadOnly())
            continue;

        wchar_t filename[MAX_PATH]{ 0 };
        ::SendMessage(nppData._nppHandle, NPPM_GETFILENAME, MAX_PATH, (LPARAM)filename);


        auto docclock_start = clock();
        action(doc);
        auto docclock_end = clock();

        {
            std::wstring txt;
            txt = txt + filename + L" => time taken: " + std::to_wstring(docclock_end - docclock_start) + L" ms";
            dbgln(txt,DBG_LEVEL::DBG_INFO);
        }

        // Put scroll at the left of the view
        doc.SetXOffset(0);
    }

    auto clock_end = clock();
    
    std::wstring txt;
    txt = txt + L"- nppMultiDocumentCommand(\"" + debugname + L"\") => time taken: " + std::to_wstring(clock_end - clock_start) + L" ms";
    dbgln(txt);
}

void nppDocumentCommand(const std::wstring& debugname, void (*action)(ScintillaDoc&)) {
    dbgln(L"+ nppDocumentCommand(\"" + debugname + L"\")");

    int currentEdit;
    ::SendMessage(nppData._nppHandle, NPPM_GETCURRENTSCINTILLA, 0, (LPARAM)&currentEdit);

    ScintillaDoc doc = ScintillaDoc(getCurrentHScintilla(currentEdit));

    if (doc.IsReadOnly()) {
        dbgln(L"Operation aborted, read online document");
        return;
    }

    auto clock_start = clock();

    wchar_t filename[MAX_PATH]{ 0 };
    ::SendMessage(nppData._nppHandle, NPPM_GETFILENAME, MAX_PATH, (LPARAM)filename);

    action(doc);

    auto clock_end = clock();

    std::wstring txt;
    txt = txt + L"- nppDocumentCommand(\"" + debugname + L"\") on file '"+filename+L"' => time taken: " + std::to_wstring(clock_end - clock_start) + L" ms";
    dbgln(txt, DBG_LEVEL::DBG_INFO);
}
