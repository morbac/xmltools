#include "StdAfx.h"
#include "XMLTools.h"
#include "nppHelpers.h"
#include "Report.h"

#include <assert.h>

std::vector<FuncItem> nppMenu;

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
    ::CheckMenuItem(::GetMenu(nppData._nppHandle), nppMenu[idx]._cmdID, MF_BYCOMMAND | (value ? MF_CHECKED : MF_UNCHECKED));
    nppMenu[idx]._init2Check = value;
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

void manualXMLCheck();
void manualValidation();
void insertXMLCheckTag();
void insertXMLCloseTag();
void insertAutoXMLType();
void togglePreventXXE();
void toggleAllowHuge();
void nppPrettyPrintXmlFast();
void nppPrettyPrintXmlAttrFast();
void nppPrettyPrintXmlIndentOnlyFast();
void nppLinearizeXmlFast();
//void insertPrettyPrintTag();
void togglePrettyPrintAllFiles();

void nppConvertXML2Text();
void nppConvertText2XML();

void commentSelection();
void uncommentSelection();

void getCurrentXPathStd();
void getCurrentXPathPredicate();
void evaluateXPath();

void performXSLTransform();

void aboutBox();
void optionsDlg();

int addMenuItem(const wchar_t* title, PFUNCPLUGINCMD action, bool checked = false, ShortcutKey *shortcut = NULL) {
    FuncItem item;
    
    wcscpy(item._itemName, title);
    item._pFunc = action;
    item._init2Check = checked;
    item._pShKey = shortcut;

    nppMenu.push_back(item);
    return static_cast<int>(nppMenu.size() - 1);
}

void addMenuSeparator() {
    FuncItem item;

    item._itemName[0] = 0;
    item._pFunc = NULL;
    item._pShKey = NULL;

    nppMenu.push_back(item);
}

ShortcutKey *createShortcut(unsigned char key, bool enableALT = true, bool enableCTRL = true, bool enableSHIFT = true) {
    auto shortcut = new ShortcutKey(); // no parentheses needed as it's Plain Old Data (POD) otherwise C4345
    shortcut->_isAlt = enableALT;
    shortcut->_isCtrl = enableCTRL;
    shortcut->_isShift = enableSHIFT;
    shortcut->_key = key;
    
    return shortcut;
}

void initMenu() {

    dbgln("Building plugin menu entries... ", DBG_LEVEL::DBG_INFO);

    menuitemCheckXML = addMenuItem(L"Enable XML syntax auto-check", insertXMLCheckTag, config.doCheckXML);
    addMenuItem(L"Check XML syntax now", manualXMLCheck);

    addMenuSeparator();
    
    menuitemValidation = addMenuItem(L"Enable auto-validation", insertValidationTag, config.doValidation);
    addMenuItem(L"Validate now", manualValidation, false, createShortcut('M'));
    
    addMenuSeparator();
    
    menuitemCloseTag = addMenuItem(L"Tag auto-close", insertXMLCloseTag, config.doCloseTag);
    
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
    addMenuSeparator();

    menuitemAutoXMLType = addMenuItem(L"Set XML type automatically", insertAutoXMLType, config.doAutoXMLType);
    menuitemPreventXXE = addMenuItem(L"Prevent XXE", togglePreventXXE, config.doPreventXXE);
    menuitemAllowHuge = addMenuItem(L"Allow huge files", toggleAllowHuge, config.doAllowHuge);

    addMenuSeparator();

    addMenuItem(L"Pretty print", nppPrettyPrintXmlFast, false, createShortcut('B'));
    addMenuItem(L"Pretty print (indent attributes) - INACTIVE", nppPrettyPrintXmlAttrFast, false, createShortcut('A'));
    addMenuItem(L"Pretty print - indent only", nppPrettyPrintXmlIndentOnlyFast);
    addMenuItem(L"Linearize", nppLinearizeXmlFast, false, createShortcut('L'));
    menuitemPrettyPrintAllFiles = addMenuItem(L"Apply to all open files", togglePrettyPrintAllFiles, config.doPrettyPrintAllOpenFiles);

    addMenuSeparator();

    addMenuItem(L"Current XML Path", getCurrentXPathStd, false, createShortcut('P'));
    addMenuItem(L"Current XML Path with predicates", getCurrentXPathPredicate);
    addMenuItem(L"Evaluate XPath expression...", evaluateXPath);

    addMenuSeparator();

    addMenuItem(L"XSL Transformation...", performXSLTransform);

    addMenuSeparator();

    addMenuItem(L"Escape characters in selection (<> → &&lt;&&gt;)", nppConvertXML2Text);
    addMenuItem(L"Unescape characters in selection (&&lt;&&gt; → <>)", nppConvertText2XML);

    addMenuSeparator();

    addMenuItem(L"Comment selection", commentSelection, false, createShortcut('C'));
    addMenuItem(L"Uncomment selection", uncommentSelection, false, createShortcut('R'));

    addMenuSeparator();

    addMenuItem(L"Options...", optionsDlg);
    addMenuItem(L"Debug window...", showDebugDlg);
    addMenuItem(L"About XML Tools / Donate...", aboutBox);

    dbgln("done.", DBG_LEVEL::DBG_INFO);
}

void destroyMenu() {
    for (size_t i = 0; i < nppMenu.size(); ++i) {
        if (nppMenu[i]._pShKey) delete nppMenu[i]._pShKey;
    }
}