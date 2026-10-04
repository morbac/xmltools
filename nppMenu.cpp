#include "StdAfx.h"
#include "XMLTools.h"
#include "nppMenu.h"
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

int addMenuItem(const wchar_t* title, PFUNCPLUGINCMD action, bool checked, ShortcutKey *shortcut) {
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

ShortcutKey *createShortcut(unsigned char key, bool enableALT, bool enableCTRL, bool enableSHIFT) {
    auto shortcut = new ShortcutKey(); // no parentheses needed as it's Plain Old Data (POD) otherwise C4345
    shortcut->_isAlt = enableALT;
    shortcut->_isCtrl = enableCTRL;
    shortcut->_isShift = enableSHIFT;
    shortcut->_key = key;
    
    return shortcut;
}

void initMenu() {

    dbgln("Building plugin menu entries... ", DBG_LEVEL::DBG_INFO);

    menuitems.menuitemToggleCheckXML = addMenuItem(Lang_Str(IDS_MENU_AUTO_CHECK), insertXMLCheckTag, config.doCheckXML);
    menuitems.menuitemCheckXML = addMenuItem(Lang_Str(IDS_MENU_CHECK_NOW), manualXMLCheck);

    addMenuSeparator();
    
    menuitems.menuitemToggleValidation = addMenuItem(Lang_Str(IDS_MENU_AUTO_VALIDATE), insertValidationTag, config.doValidation);
    menuitems.menuitemValidateXML = addMenuItem(Lang_Str(IDS_MENU_VALIDATE_NOW), manualValidation, false, createShortcut('M'));
    menuitems.menuitemFirstError = addMenuItem(Lang_Str(IDS_MENU_FIRST_ERROR), highlightFirstError);
    menuitems.menuitemPreviousError = addMenuItem(Lang_Str(IDS_MENU_PREV_ERROR), highlightPreviousError);
    menuitems.menuitemNextError = addMenuItem(Lang_Str(IDS_MENU_NEXT_ERROR), highlightNextError);
    menuitems.menuitemLastError = addMenuItem(Lang_Str(IDS_MENU_LAST_ERROR), highlightLastError);
    
    addMenuSeparator();
    
    menuitems.menuitemToggleCloseTag = addMenuItem(Lang_Str(IDS_MENU_TAG_AUTOCLOSE), insertXMLCloseTag, config.doCloseTag);
    
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

    menuitems.menuitemToggleAutoXMLType = addMenuItem(Lang_Str(IDS_MENU_AUTO_XML_TYPE), insertAutoXMLType, config.doAutoXMLType);
    menuitems.menuitemTogglePreventXXE = addMenuItem(Lang_Str(IDS_MENU_PREVENT_XXE), togglePreventXXE, config.doPreventXXE);
    menuitems.menuitemToggleAllowHuge = addMenuItem(Lang_Str(IDS_MENU_ALLOW_HUGE), toggleAllowHuge, config.doAllowHuge);

    addMenuSeparator();

    menuitems.menuitemPrettyPrint = addMenuItem(Lang_Str(IDS_MENU_PRETTY_PRINT), nppPrettyPrintXmlFast, false, createShortcut('B'));
    menuitems.menuitemPrettyPrintIndentAttr = addMenuItem(Lang_Str(IDS_MENU_PP_ATTR), nppPrettyPrintXmlAttrFast, false, createShortcut('A'));
    menuitems.menuitemPrettyPrintIndentOnly = addMenuItem(Lang_Str(IDS_MENU_PP_INDENT_ONLY), nppPrettyPrintXmlIndentOnlyFast);
    menuitems.menuitemLinearize = addMenuItem(Lang_Str(IDS_MENU_LINEARIZE), nppLinearizeXmlFast, false, createShortcut('L'));
    menuitems.menuitemTogglePrettyPrintAllFiles = addMenuItem(Lang_Str(IDS_MENU_APPLY_ALL_FILES), togglePrettyPrintAllFiles, config.doPrettyPrintAllOpenFiles);
    #ifdef _DEBUG
    menuitems.menuitemTokenize = addMenuItem(Lang_Str(IDS_MENU_TOKENIZE), nppTokenizeXmlFast, false);
    #endif

    addMenuSeparator();

    menuitems.menuitemCurrentXMLPath = addMenuItem(Lang_Str(IDS_MENU_CURRENT_PATH), getCurrentXPathStd);
    menuitems.menuitemCurrentXMLPathNS = addMenuItem(Lang_Str(IDS_MENU_CURRENT_PATH_NS), getCurrentXPathPredicate, false, createShortcut('P'));
    menuitems.menuitemEvalXPath = addMenuItem(Lang_Str(IDS_MENU_EVAL_XPATH), evaluateXPath);

    addMenuSeparator();

    menuitems.menuitemXSLTransform = addMenuItem(Lang_Str(IDS_MENU_XSL_TRANSFORM), performXSLTransform);

    addMenuSeparator();

    menuitems.menuitemEscape = addMenuItem(Lang_Str(IDS_MENU_ESCAPE), nppConvertXML2Text);
    menuitems.menuitemUnescape = addMenuItem(Lang_Str(IDS_MENU_UNESCAPE), nppConvertText2XML);

    addMenuSeparator();

    menuitems.menuitemComment = addMenuItem(Lang_Str(IDS_MENU_COMMENT), commentSelection, false, createShortcut('C'));
    menuitems.menuitemUncomment = addMenuItem(Lang_Str(IDS_MENU_UNCOMMENT), uncommentSelection, false, createShortcut('R'));

    addMenuSeparator();

    menuitems.menuitemOptions = addMenuItem(Lang_Str(IDS_MENU_OPTIONS), optionsDlg);
    menuitems.menuitemDebugWindow = addMenuItem(Lang_Str(IDS_MENU_DEBUG_WINDOW), showDebugDlg);
    menuitems.menuitemAbout = addMenuItem(Lang_Str(IDS_MENU_ABOUT), aboutBox);

    dbgln("done.", DBG_LEVEL::DBG_INFO);
}

void destroyMenu() {
    for (size_t i = 0; i < nppMenu.size(); ++i) {
        if (nppMenu[i]._pShKey) delete nppMenu[i]._pShKey;
    }
}