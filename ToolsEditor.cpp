#include "stdAfx.h"

#include "XMLTools.h"
#include "Scintilla.h"
#include "nppHelpers.h"

void closeXMLTag() {
    dbgln("closeXMLTag()");

    char buf[512];
    int currentEdit;
    ::SendMessage(nppData._nppHandle, NPPM_GETCURRENTSCINTILLA, 0, (LPARAM)&currentEdit);
    HWND hCurrentEditView = getCurrentHScintilla(currentEdit);
    int currentPos = int(::SendMessage(hCurrentEditView, SCI_GETCURRENTPOS, 0, 0));
    int beginPos = currentPos - (sizeof(buf) - 1);
    int startPos = (beginPos > 0) ? beginPos : 0;
    int size = currentPos - startPos;
    int insertStringSize = 2;

#define MAX_TAGNAME_LENGTH 516
    char insertString[MAX_TAGNAME_LENGTH] = "</";

    if (size >= 3) {
        struct TextRange tr = { {startPos, currentPos}, buf };
        ::SendMessage(hCurrentEditView, SCI_GETTEXTRANGE, 0, (LPARAM)&tr);

        if (buf[size - 2] != '/' && buf[size - 2] != '-') {
            const char* pBegin = &buf[0];
            const char* pCur = &buf[size - 2];
            int insertStringSize = 2;

            // search the beginning of tag
            // TODO: optimize by not looping on every char !
            for (; pCur > pBegin && *pCur != '<' && *pCur != '>';) {
                --pCur;
            }

            if (*pCur == '<') {
                ++pCur;
                if (*pCur == '/' || *pCur == '!') return;

                // search attributes of
                while (*pCur != '>' && *pCur != ' ' && *pCur != '\n' && *pCur != '\r') {
                    //while (IsCharAlphaNumeric(*pCur) || strchr(":_-.", *pCur) != NULL) {
                    if (insertStringSize == MAX_TAGNAME_LENGTH - 1) return;
                    insertString[insertStringSize++] = *pCur;
                    ++pCur;
                }
            }

            if (insertStringSize == MAX_TAGNAME_LENGTH - 1) return;
            insertString[insertStringSize++] = '>';
            insertString[insertStringSize] = '\0';

            if (insertStringSize > 3) {
                ::SendMessage(hCurrentEditView, SCI_BEGINUNDOACTION, 0, 0);
                ::SendMessage(hCurrentEditView, SCI_REPLACESEL, 0, (LPARAM)insertString);
                ::SendMessage(hCurrentEditView, SCI_SETSEL, currentPos, currentPos);
                ::SendMessage(hCurrentEditView, SCI_ENDUNDOACTION, 0, 0);
            }
        }
    }

#undef MAX_TAGNAME_LENGTH
}

///////////////////////////////////////////////////////////////////////////////

void tagAutoIndent() {
    dbgln("tagAutoIndent()");

    // On n'indente que si l'on est dans un noeud (au niveau de l'attribut ou
    // au niveau du contenu. Donc on recherche le dernier < ou >. S'il s'agit
    // d'un >, on regarde qu'il n'y ait pas de / avant (sinon on se retrouve
    // au même niveau et il n'y a pas d'indentation à faire)
    // Si le dernier symbole que l'on trouve est un <, alors on indente.

    char buf[512];
    int currentEdit;
    ::SendMessage(nppData._nppHandle, NPPM_GETCURRENTSCINTILLA, 0, (LPARAM)&currentEdit);
    HWND hCurrentEditView = getCurrentHScintilla(currentEdit);
    int currentPos = int(::SendMessage(hCurrentEditView, SCI_GETCURRENTPOS, 0, 0));
    int beginPos = currentPos - (sizeof(buf) - 1);
    int startPos = (beginPos > 0) ? beginPos : 0;
    int size = currentPos - startPos;

    struct TextRange tr = { {startPos, currentPos}, buf };
    ::SendMessage(hCurrentEditView, SCI_GETTEXTRANGE, 0, (LPARAM)&tr);

    int tabwidth = (int) ::SendMessage(hCurrentEditView, SCI_GETTABWIDTH, 0, 0);
    bool usetabs = (bool) ::SendMessage(hCurrentEditView, SCI_GETUSETABS, 0, 0);
    if (tabwidth <= 0) tabwidth = 4;

    bool ignoreIndentation = false;
    if (size >= 1) {
        const char* pBegin = &buf[0];
        const char* pCur = &buf[size - 1];

        for (; pCur > pBegin && *pCur != '>';) --pCur;
        if (pCur > pBegin) {
            if (*(pCur - 1) == '/') ignoreIndentation = true;  // si on a "/>", on abandonne l'indentation
            // maintenant, on recherche le <
            while (pCur > pBegin && *pCur != '<') --pCur;
            if (*pCur == '<' && *(pCur + 1) == '/') ignoreIndentation = true; // si on a "</", on abandonne aussi

            int insertStringSize = 0;
            char insertString[516] = { '\0' };

            --pCur;
            // on récupère l'indentation actuelle
            while (pCur > pBegin && *pCur != '\n' && *pCur != '\r') {
                if (*pCur == '\t') insertString[insertStringSize++] = '\t';
                else insertString[insertStringSize++] = ' ';

                --pCur;
            }

            // et on ajoute une indentation
            if (!ignoreIndentation) {
                if (usetabs) insertString[insertStringSize++] = '\t';
                else {
                    for (int i = 0; i < tabwidth; ++i) insertString[insertStringSize++] = ' ';
                }
            }

            currentPos += insertStringSize;

            // on a trouvé le <, il reste à insérer une indentation après le curseur
            ::SendMessage(hCurrentEditView, SCI_REPLACESEL, 0, (LPARAM)insertString);
            ::SendMessage(hCurrentEditView, SCI_SETSEL, currentPos, currentPos);
        }
    }
}

bool setAutoXMLType() {
    dbgln("setAutoXMLType()");

    int currentEdit;
    ::SendMessage(nppData._nppHandle, NPPM_GETCURRENTSCINTILLA, 0, (LPARAM)&currentEdit);
    HWND hCurrentEditView = getCurrentHScintilla(currentEdit);

    // on récupère les 6 premiers caractères du fichier
    char head[8] = { '\0' };
    ::SendMessage(hCurrentEditView, SCI_GETTEXT, 7, reinterpret_cast<LPARAM>(&head));

    if (strlen(head) >= 6 && !strcmp(head, "<?xml ")) {
        ::SendMessage(nppData._nppHandle, NPPM_SETCURRENTLANGTYPE, 0, (LPARAM)LangType::L_XML);
        return true;
    }

    return false;
}
