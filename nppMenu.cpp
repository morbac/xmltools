#include "StdAfx.h"
#include "XMLTools.h"
#include "nppHelpers.h"
#include "Report.h"

#include <assert.h>

std::vector<FuncItem> nppMenu;
struct struct_menuitems menuitems = {};

void ToggleMenuItem(int idx, bool& value) {
    value = !value;
    ::CheckMenuItem(::GetMenu(nppData._nppHandle), nppMenu[idx]._cmdID, MF_BYCOMMAND | (value ? MF_CHECKED : MF_UNCHECKED));
    nppMenu[idx]._init2Check = value;
    savePluginParams();
}

void insertXMLCheckTag() {
    dbgln("insertXMLCheckTag()");
    ToggleMenuItem(menuitems.menuitemToggleCheckXML, config.doCheckXML);
}

void insertValidationTag() {
    dbgln("insertValidationTag()");
    ToggleMenuItem(menuitems.menuitemToggleValidation, config.doValidation);
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
    ToggleMenuItem(menuitems.menuitemToggleCloseTag, config.doCloseTag);
}

void insertTagAutoIndent() {
    dbgln("insertTagAutoIndent()");

    static bool tagAutoIndentWarningDisplayed = false;
    if (!tagAutoIndentWarningDisplayed) {
        Report::_printf_inf(L"This function is in alpha state and might disappear in future release.");
        tagAutoIndentWarningDisplayed = true;
    }
    ToggleMenuItem(menuitems.menuitemToggleAutoIndent, config.doAutoIndent);
}

void insertAttributeAutoComplete() {
    dbgln("insertAttributeAutoComplete()");

    static bool insertAttributeAutoCompleteWarningDisplayed = false;
    if (!insertAttributeAutoCompleteWarningDisplayed) {
        Report::_printf_inf(L"This function is in alpha state and might disappear in future release.");
        insertAttributeAutoCompleteWarningDisplayed = true;
    }

    ToggleMenuItem(menuitems.menuitemToggleAttrAutoComplete, config.doAttrAutoComplete);
}

void insertAutoXMLType() {
    dbgln("insertAutoXMLType()");
    ToggleMenuItem(menuitems.menuitemToggleAutoXMLType, config.doAutoXMLType);
}

void togglePreventXXE() {
    dbgln("togglePreventXXE()");
    ToggleMenuItem(menuitems.menuitemTogglePreventXXE, config.doPreventXXE);
}

void toggleAllowHuge() {
    dbgln("toggleAllowHuge()");
    ToggleMenuItem(menuitems.menuitemToggleAllowHuge, config.doAllowHuge);
}

void togglePrettyPrintAllFiles() {
    dbgln("togglePrettyPrintAllFiles()");
    ToggleMenuItem(menuitems.menuitemTogglePrettyPrintAllFiles, config.doPrettyPrintAllOpenFiles);
}

void manualXMLCheck();
void manualValidation();
void firstError();
void previousError();
void nextError();
void lastError();
void insertXMLCheckTag();
void insertXMLCloseTag();
void insertAutoXMLType();
void togglePreventXXE();
void toggleAllowHuge();
void nppPrettyPrintXmlFast();
void nppPrettyPrintXmlAttrFast();
void nppPrettyPrintXmlIndentOnlyFast();
void nppLinearizeXmlFast();
void nppTokenizeXmlFast();
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

    menuitems.menuitemToggleCheckXML = addMenuItem(L"Enable XML syntax auto-check", insertXMLCheckTag, config.doCheckXML);
    menuitems.menuitemCheckXML = addMenuItem(L"Check XML syntax now", manualXMLCheck);

    addMenuSeparator();
    
    menuitems.menuitemToggleValidation = addMenuItem(L"Enable auto-validation", insertValidationTag, config.doValidation);
    menuitems.menuitemValidateXML = addMenuItem(L"Validate now", manualValidation, false, createShortcut('M'));
    menuitems.menuitemFirstError = addMenuItem(L"First error", firstError);
    menuitems.menuitemPreviousError = addMenuItem(L"Previous error", previousError);
    menuitems.menuitemNextError = addMenuItem(L"Next error", nextError);
    menuitems.menuitemLastError = addMenuItem(L"Last error", lastError);
    
    addMenuSeparator();
    
    menuitems.menuitemToggleCloseTag = addMenuItem(L"Tag auto-close", insertXMLCloseTag, config.doCloseTag);
    
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

    menuitems.menuitemToggleAutoXMLType = addMenuItem(L"Set XML type automatically", insertAutoXMLType, config.doAutoXMLType);
    menuitems.menuitemTogglePreventXXE = addMenuItem(L"Prevent XXE", togglePreventXXE, config.doPreventXXE);
    menuitems.menuitemToggleAllowHuge = addMenuItem(L"Allow huge files", toggleAllowHuge, config.doAllowHuge);

    addMenuSeparator();

    menuitems.menuitemPrettyPrint = addMenuItem(L"Pretty print", nppPrettyPrintXmlFast, false, createShortcut('B'));
    menuitems.menuitemPrettyPrintIndentAttr = addMenuItem(L"Pretty print - indent attributes", nppPrettyPrintXmlAttrFast, false, createShortcut('A'));
    menuitems.menuitemPrettyPrintIndentOnly = addMenuItem(L"Pretty print - indent only", nppPrettyPrintXmlIndentOnlyFast);
    menuitems.menuitemLinearize = addMenuItem(L"Linearize", nppLinearizeXmlFast, false, createShortcut('L'));
    menuitems.menuitemTogglePrettyPrintAllFiles = addMenuItem(L"Apply to all open files", togglePrettyPrintAllFiles, config.doPrettyPrintAllOpenFiles);
    #ifdef _DEBUG
    menuitems.menuitemTokenize = addMenuItem(L"Tokenize (debug)", nppTokenizeXmlFast, false);
    #endif

    addMenuSeparator();

    menuitems.menuitemCurrentXMLPath = addMenuItem(L"Current XML Path", getCurrentXPathStd);
    menuitems.menuitemCurrentXMLPathNS = addMenuItem(L"Current XML Path with predicates", getCurrentXPathPredicate, false, createShortcut('P'));
    menuitems.menuitemEvalXPath = addMenuItem(L"Evaluate XPath expression...", evaluateXPath);

    addMenuSeparator();

    menuitems.menuitemXSLTransform = addMenuItem(L"XSL Transformation...", performXSLTransform);

    addMenuSeparator();

    menuitems.menuitemEscape = addMenuItem(L"Escape characters in selection (<> → &&lt;&&gt;)", nppConvertXML2Text);
    menuitems.menuitemUnescape = addMenuItem(L"Unescape characters in selection (&&lt;&&gt; → <>)", nppConvertText2XML);

    addMenuSeparator();

    menuitems.menuitemComment = addMenuItem(L"Comment selection", commentSelection, false, createShortcut('C'));
    menuitems.menuitemUncomment = addMenuItem(L"Uncomment selection", uncommentSelection, false, createShortcut('R'));

    addMenuSeparator();

    menuitems.menuitemOptions = addMenuItem(L"Options...", optionsDlg);
    menuitems.menuitemDebugWindow = addMenuItem(L"Debug window...", showDebugDlg);
    menuitems.menuitemAbout = addMenuItem(L"About XML Tools / Donate...", aboutBox);

    dbgln("done.", DBG_LEVEL::DBG_INFO);
}

void destroyMenu() {
    for (size_t i = 0; i < nppMenu.size(); ++i) {
        if (nppMenu[i]._pShKey) delete nppMenu[i]._pShKey;
    }
}