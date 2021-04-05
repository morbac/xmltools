#pragma once
#include "nppHelpers.h"

void ToggleMenuItem(int idx, bool& value);
void insertXMLCheckTag();

void insertValidationTag();
//void insertPrettyPrintTag();
void insertXMLCloseTag();
void insertTagAutoIndent();
void insertAttributeAutoComplete();
void insertAutoXMLType();

void togglePreventXXE();
void toggleAllowHuge();
void togglePrettyPrintAllFiles();

void manualXMLCheck();
void manualValidation();
void highlightFirstError();
void highlightPreviousError();
void highlightNextError();
void highlightLastError();
void highlightError(size_t num);
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

int addMenuItem(const wchar_t* title, PFUNCPLUGINCMD action, bool checked = false, ShortcutKey* shortcut = NULL);

void addMenuSeparator();

ShortcutKey* createShortcut(unsigned char key, bool enableALT = true, bool enableCTRL = true, bool enableSHIFT = true);

void initMenu();

void destroyMenu();