#include "StdAfx.h"
#include "XMLTools.h"
#include "nppHelpers.h"
#include "Report.h"

#include <assert.h>

// The number of functionality
//#ifdef _DEBUG
const int TOTAL_FUNCS = 32 + 2;
//#else
//  const int TOTAL_FUNCS = 31+2;
//#endif
int nbFunc = TOTAL_FUNCS;

FuncItem funcItem[TOTAL_FUNCS];

int menuitemCheckXML = -1,
    menuitemValidation = -1,
    /*menuitemPrettyPrint = -1,*/
    menuitemCloseTag = -1,
    menuitemAutoIndent = -1,
    menuitemAttrAutoComplete = -1,
    menuitemAutoXMLType = -1,
    menuitemPreventXXE = -1,
    menuitemAllowHuge = -1,
    menuitemPrettyPrintAllFiles = -1;

void ToggleMenuItem(int idx, bool& value) {
    value = !value;
    ::CheckMenuItem(::GetMenu(nppData._nppHandle), funcItem[menuitemPrettyPrintAllFiles]._cmdID, MF_BYCOMMAND | (value ? MF_CHECKED : MF_UNCHECKED));
    funcItem[idx]._init2Check = value;
    savePluginParams();
}

void insertXMLCheckTag() {
    dbgln("insertXMLCheckTag()");
    ToggleMenuItem(menuitemCheckXML, config.doCheckXML);
}

void insertValidationTag() {
    dbgln("insertValidationTag()");
    ToggleMenuItem(menuitemValidation, config.doValidation);
}

/*
void insertPrettyPrintTag() {
  dbgln("insertPrettyPrintTag()");

  doPrettyPrint = !doPrettyPrint;
  ::CheckMenuItem(::GetMenu(nppData._nppHandle), funcItem[menuitemPrettyPrint]._cmdID, MF_BYCOMMAND | (doPrettyPrint?MF_CHECKED:MF_UNCHECKED));
  savePluginParams();
}
*/

void insertXMLCloseTag() {
    dbgln("insertXMLCloseTag()");
    ToggleMenuItem(menuitemCloseTag, config.doCloseTag);
}

void insertTagAutoIndent() {
    dbgln("insertTagAutoIndent()");

    static bool tagAutoIndentWarningDisplayed = false;
    if (!tagAutoIndentWarningDisplayed) {
        Report::_printf_inf(L"This function is in alpha state and might disappear in future release.");
        tagAutoIndentWarningDisplayed = true;
    }
    ToggleMenuItem(menuitemAutoIndent, config.doAutoIndent);
}

void insertAttributeAutoComplete() {
    dbgln("insertAttributeAutoComplete()");

    static bool insertAttributeAutoCompleteWarningDisplayed = false;
    if (!insertAttributeAutoCompleteWarningDisplayed) {
        Report::_printf_inf(L"This function is in alpha state and might disappear in future release.");
        insertAttributeAutoCompleteWarningDisplayed = true;
    }

    ToggleMenuItem(menuitemAttrAutoComplete, config.doAttrAutoComplete);
}

void insertAutoXMLType() {
    dbgln("insertAutoXMLType()");
    ToggleMenuItem(menuitemAutoXMLType, config.doAutoXMLType);
}

void togglePreventXXE() {
    dbgln("togglePreventXXE()");
    ToggleMenuItem(menuitemPreventXXE, config.doPreventXXE);
}

void toggleAllowHuge() {
    dbgln("toggleAllowHuge()");
    ToggleMenuItem(menuitemAllowHuge, config.doAllowHuge);
}

void togglePrettyPrintAllFiles() {
    dbgln("togglePrettyPrintAllFiles()");
    ToggleMenuItem(menuitemPrettyPrintAllFiles, config.doPrettyPrintAllOpenFiles);
}

void registerShortcut(FuncItem* item, bool enableALT, bool enableCTRL, bool enableSHIFT, unsigned char key) {
    if (!item) return;
    item->_pShKey = new ShortcutKey; // no parentheses needed as it's Plain Old Data (POD) otherwise C4345
    item->_pShKey->_isAlt = enableALT;
    item->_pShKey->_isCtrl = enableCTRL;
    item->_pShKey->_isShift = enableSHIFT;
    item->_pShKey->_key = key;
}

void manualXMLCheck();
void manualValidation();
void insertXMLCheckTag();
void insertXMLCloseTag();
void insertAutoXMLType();
void togglePreventXXE();
void toggleAllowHuge();
void nppPrettyPrintXML();
void nppPrettyPrintXmlFast();
void nppLinearizeXmlFast();
void nppPrettyPrintAttributes();
//void insertPrettyPrintTag();
void nppLinearizeXML();
void togglePrettyPrintAllFiles();

void convertXML2Text();
void convertText2XML();

void commentSelection();
void uncommentSelection();

void getCurrentXPathStd();
void getCurrentXPathPredicate();
void evaluateXPath();

void performXSLTransform();

void aboutBox();
void optionsDlg();
//void debugDlg();

void initMenu() {

    int menuentry = 0;
    for (int i = 0; i < nbFunc; ++i) {
        funcItem[i]._init2Check = false;
    }

    dbgln("Building plugin menu entries... ", DBG_LEVEL::DBG_INFO);

    wcscpy(funcItem[menuentry]._itemName, L"Enable XML syntax auto-check");
    funcItem[menuentry]._pFunc = insertXMLCheckTag;
    funcItem[menuentry]._init2Check = config.doCheckXML;
    menuitemCheckXML = menuentry;
    ++menuentry;

    wcscpy(funcItem[menuentry]._itemName, L"Check XML syntax now");
    funcItem[menuentry]._pFunc = manualXMLCheck;
    ++menuentry;

    funcItem[menuentry++]._pFunc = NULL;  //----------------------------------------

    wcscpy(funcItem[menuentry]._itemName, L"Enable auto-validation");
    funcItem[menuentry]._pFunc = insertValidationTag;
    funcItem[menuentry]._init2Check = config.doValidation;
    menuitemValidation = menuentry;
    ++menuentry;

    wcscpy(funcItem[menuentry]._itemName, L"Validate now");
    funcItem[menuentry]._pFunc = manualValidation;
    registerShortcut(funcItem + menuentry, true, true, true, 'M');
    ++menuentry;

    funcItem[menuentry++]._pFunc = NULL;  //----------------------------------------

    wcscpy(funcItem[menuentry]._itemName, L"Tag auto-close");
    funcItem[menuentry]._pFunc = insertXMLCloseTag;
    funcItem[menuentry]._init2Check = config.doCloseTag;
    menuitemCloseTag = menuentry;
    ++menuentry;
    /*
    Report::strcpy(funcItem[menuentry]._itemName, L"Tag auto-indent");
    funcItem[menuentry]._pFunc = insertTagAutoIndent;
    funcItem[menuentry]._init2Check = doAutoIndent = (::GetPrivateProfileInt(sectionName, L"doAutoIndent", 0, iniFilePath) != 0);
    doAutoIndent = funcItem[menuentry]._init2Check;
    menuitemAutoIndent = menuentry;
    ++menuentry;

    Report::strcpy(funcItem[menuentry]._itemName, L"Auto-complete attributes");
    funcItem[menuentry]._pFunc = insertAttributeAutoComplete;
    funcItem[menuentry]._init2Check = doAttrAutoComplete = (::GetPrivateProfileInt(sectionName, L"doAttrAutoComplete", 0, iniFilePath) != 0);
    doAttrAutoComplete = funcItem[menuentry]._init2Check;
    menuitemAttrAutoComplete = menuentry;
    ++menuentry;
    */
    funcItem[menuentry++]._pFunc = NULL;  //----------------------------------------

    wcscpy(funcItem[menuentry]._itemName, L"Set XML type automatically");
    funcItem[menuentry]._pFunc = insertAutoXMLType;
    funcItem[menuentry]._init2Check = config.doAutoXMLType;
    menuitemAutoXMLType = menuentry;
    ++menuentry;

    wcscpy(funcItem[menuentry]._itemName, L"Prevent XXE");
    funcItem[menuentry]._pFunc = togglePreventXXE;
    funcItem[menuentry]._init2Check = config.doPreventXXE;
    menuitemPreventXXE = menuentry;
    ++menuentry;

    wcscpy(funcItem[menuentry]._itemName, L"Allow huge files");
    funcItem[menuentry]._pFunc = toggleAllowHuge;
    funcItem[menuentry]._init2Check = config.doAllowHuge;
    menuitemAllowHuge = menuentry;
    ++menuentry;

    funcItem[menuentry++]._pFunc = NULL;  //----------------------------------------

    wcscpy(funcItem[menuentry]._itemName, L"Pretty print");
    registerShortcut(funcItem + menuentry, true, true, true, 'B');
    funcItem[menuentry]._pFunc = nppPrettyPrintXML;
    ++menuentry;

    wcscpy(funcItem[menuentry]._itemName, L"Pretty print (indent attributes)");
    registerShortcut(funcItem + menuentry, true, true, true, 'A');
    funcItem[menuentry]._pFunc = nppPrettyPrintAttributes;
    ++menuentry;
    /*
    Report::strcpy(funcItem[menuentry]._itemName, L"Enable auto pretty print (libXML) [experimental]");
    funcItem[menuentry]._pFunc = insertPrettyPrintTag;
    funcItem[menuentry]._init2Check = doPrettyPrint = (::GetPrivateProfileInt(sectionName, L"doPrettyPrint", 0, iniFilePath) != 0);
    doPrettyPrint = funcItem[menuentry]._init2Check;
    menuitemPrettyPrint = menuentry;
    ++menuentry;
    */
    wcscpy(funcItem[menuentry]._itemName, L"Linearize XML");
    registerShortcut(funcItem + menuentry, true, true, true, 'L');
    funcItem[menuentry]._pFunc = nppLinearizeXML;
    ++menuentry;

    wcscpy(funcItem[menuentry]._itemName, L"Apply to all open files");
    funcItem[menuentry]._pFunc = togglePrettyPrintAllFiles;
    funcItem[menuentry]._init2Check = config.doPrettyPrintAllOpenFiles;
    menuitemPrettyPrintAllFiles = menuentry;
    ++menuentry;

    wcscpy(funcItem[menuentry]._itemName, L"Pretty print (fast)");
    funcItem[menuentry]._pFunc = nppPrettyPrintXmlFast;
    ++menuentry;

    wcscpy(funcItem[menuentry]._itemName, L"Linearize (fast)");
    funcItem[menuentry]._pFunc = nppLinearizeXmlFast;
    ++menuentry;

    funcItem[menuentry++]._pFunc = NULL;  //----------------------------------------

    wcscpy(funcItem[menuentry]._itemName, L"Current XML Path");
    registerShortcut(funcItem + menuentry, true, true, true, 'P');
    funcItem[menuentry]._pFunc = getCurrentXPathStd;
    ++menuentry;

    wcscpy(funcItem[menuentry]._itemName, L"Current XML Path with predicates");
    funcItem[menuentry]._pFunc = getCurrentXPathPredicate;
    ++menuentry;

    wcscpy(funcItem[menuentry]._itemName, L"Evaluate XPath expression...");
    funcItem[menuentry]._pFunc = evaluateXPath;
    ++menuentry;

    funcItem[menuentry++]._pFunc = NULL;  //----------------------------------------

    wcscpy(funcItem[menuentry]._itemName, L"XSL Transformation...");
    funcItem[menuentry]._pFunc = performXSLTransform;
    ++menuentry;

    funcItem[menuentry++]._pFunc = NULL;  //----------------------------------------

    wcscpy(funcItem[menuentry]._itemName, L"Escape characters in selection (<> → &&lt;&&gt;)");
    funcItem[menuentry]._pFunc = convertXML2Text;
    ++menuentry;

    wcscpy(funcItem[menuentry]._itemName, L"Unescape characters in selection (&&lt;&&gt; → <>)");
    funcItem[menuentry]._pFunc = convertText2XML;
    ++menuentry;

    funcItem[menuentry++]._pFunc = NULL;  //----------------------------------------

    wcscpy(funcItem[menuentry]._itemName, L"Comment selection");
    registerShortcut(funcItem + menuentry, true, true, true, 'C');
    funcItem[menuentry]._pFunc = commentSelection;
    ++menuentry;

    wcscpy(funcItem[menuentry]._itemName, L"Uncomment selection");
    registerShortcut(funcItem + menuentry, true, true, true, 'R');
    funcItem[menuentry]._pFunc = uncommentSelection;
    ++menuentry;

    funcItem[menuentry++]._pFunc = NULL;  //----------------------------------------

    wcscpy(funcItem[menuentry]._itemName, L"Options...");
    funcItem[menuentry]._pFunc = optionsDlg;
    ++menuentry;

    //#ifdef _DEBUG
    wcscpy(funcItem[menuentry]._itemName, L"Debug window...");
    funcItem[menuentry]._pFunc = showDebugDlg;
    ++menuentry;
    //#endif

    wcscpy(funcItem[menuentry]._itemName, L"About XML Tools / Donate...");
    funcItem[menuentry]._pFunc = aboutBox;
    ++menuentry;

    assert(menuentry == nbFunc);

    dbgln("done.", DBG_LEVEL::DBG_INFO);
}

void destroyMenu() {
    for (int i = 0; i < nbFunc; ++i) {
        if (funcItem[i]._pShKey) delete funcItem[i]._pShKey;
    }
}