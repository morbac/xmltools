#include "nppHelpers.h"
#include "XMLTools.h"

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
            ::SendMessage(nppData._nppHandle, NPPM_ACTIVATEDOC, SUB_VIEW, (*iter) - nbopenfiles1);
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


void nppDocumentCommand(char* debugname, void (*action)(ScintillaDoc&)) {
    dbg("+ nppDocumentCommand(\"");
    dbg(debugname);
    dbgln("\")");

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

        dbg(filename);
        dbg(" => time taken: ");
        dbg(std::to_string(docclock_end - docclock_start).c_str());
        dbgln(" ms");

        // Put scroll at the left of the view
        doc.SetXOffset(0);
    }

    auto clock_end = clock();
    dbg("- nppDocumentCommand(\"");
    dbg(debugname);
    dbg("\") => time taken: ");
    dbg(std::to_string(clock_end - clock_start).c_str());
    dbgln(" ms");
}