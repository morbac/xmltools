#pragma once
#include <string>

#include "Debug.h"

struct struct_proxyoptions {
	bool status;
	std::wstring host = L"192.168.0.1";
	int port = 8080;
	std::wstring username = L"";
	std::wstring password = L"";
};

struct struct_msxmloptions {                // default value
  // msxml features
	int allowDocumentFunction = -1;         // True in 3.0. False in 6.0.
	int allowXsltScript = -1;               // True in 3.0. False in 6.0.
	int forceResync = -1;                   // True
	int maxElementDepth = 0;                // 0 in 3.0. 256 in 6.0.
	int maxXMLSize = -1;                     // 0
	int multipleErrorMessages = -1;         // False
	int newParser = -1;                     // False
	int normalizeAttributeValues = -1;      // False
	int populateElementDefaultValues = -1;  // False
	int prohibitDTD = -1;                   // True in 3.0. False in 6.0.
	int resolveExternals = -1;              // False
	std::wstring selectionLanguage = L"";     // "XSLPattern" in 3.0. "XPath" in 6.0
	std::wstring selectionNamespace = L"";    // ""
	int serverHTTPRequest = -1;             // False
	int useInlineSchema = -1;               // False
	int validateOnParse = -1;               // True

	// xmltools options
};

struct struct_xmltoolsoptions {
	std::wstring formatingEngine = L"QuickXml";
	std::wstring errorDisplayMode = L"Annotation";	// Annotation / Dialog / Alert
	int annotationStyle = 12;                // 12
	int annotationHighlightStyle = 13;
	int maxErrorsNum = 10;
	int maxIndentLevel = 0;

	bool xpathOnStatusbar = true;
	std::wstring identityAttributes = L"id;name";

	bool convertAmp = true;
	bool convertLt = true;
	bool convertGt = true;
	bool convertQuote = true;
	bool convertApos = true;
	bool ppAutoclose = true;
	bool ensureConformity = true;
	bool applySpacePreserve = false;

	bool tbEnabled = true;
	bool tbCheckXML = true;
	bool tbValidateXML = true;
	bool tbFirstError = true;
	bool tbPrevError = true;
	bool tbNextError = true;
	bool tbLastError = true;
	bool tbPrettyPrint = true;
	bool tbPrettyPrintIndentAttr = true;
	bool tbPrettyPrintIndentOnly = true;
	bool tbLinearize = true;
	bool tbCurrentXMLPath = true;
	bool tbCurrentXMLPathNS = true;
	bool tbEvalXPath = true;
	bool tbXSLTransform = true;
	bool tbEscape = true;
	bool tbUnescape = true;
	bool tbComment = true;
	bool tbUncomment = true;
	bool tbOptions = true;
};

extern struct struct_proxyoptions proxyoptions;
extern struct struct_xmltoolsoptions xmltoolsoptions;
extern struct struct_msxmloptions msxmloptions;

struct struct_menuitems {
	int menuitemToggleCheckXML = -1;
	int menuitemToggleValidation = -1;
	/*int menuitemPrettyPrint = -1;*/
	int menuitemToggleCloseTag = -1;
	int menuitemToggleAutoIndent = -1;
	int menuitemToggleAttrAutoComplete = -1;
	int menuitemToggleAutoXMLType = -1;
	int menuitemTogglePreventXXE = -1;
	int menuitemToggleAllowHuge = -1;
	int menuitemTogglePrettyPrintAllFiles = -1;

	int menuitemCheckXML = -1;
	int menuitemValidateXML = -1;
	int menuitemFirstError = -1;
	int menuitemPreviousError = -1;
	int menuitemNextError = -1;
	int menuitemLastError = -1;
	int menuitemPrettyPrint = -1;
	int menuitemPrettyPrintIndentAttr = -1;
	int menuitemPrettyPrintIndentOnly = -1;
	int menuitemLinearize = -1;
	int menuitemCurrentXMLPath = -1;
	int menuitemCurrentXMLPathNS = -1;
	int menuitemEvalXPath = -1;
	int menuitemXSLTransform = -1;
	int menuitemEscape = -1;
	int menuitemUnescape = -1;
	int menuitemComment = -1;
	int menuitemUncomment = -1;
	int menuitemOptions = -1;
	int menuitemDebugWindow = -1;
	int menuitemAbout = -1;
	int menuitemTokenize = -1;
};

extern struct struct_menuitems menuitems;

class XmlToolsConfig {
	const wchar_t* sectionName = L"XML Tools";
	const wchar_t* configFileName = L"XMLTools.ini";

	std::wstring configPath;

public:
	bool doCheckXML = true,
		doValidation = false,
		//doPrettyPrint = false
		doCloseTag = true,
		doAutoIndent = false,
		doAttrAutoComplete = false,
		doAutoXMLType = true,
		doPreventXXE = true,
		doAllowHuge = false,
		doPrettyPrintAllOpenFiles = false;

	DBG_LEVEL dbgLevel = DBG_LEVEL::DBG_INFO;

	XmlToolsConfig(std::wstring path) {
		configPath = path + L"\\" + configFileName;
	}

	XmlToolsConfig() {
	}

	void Read(std::wstring _path);
	void Write();

private:
	void WriteInt(const wchar_t* name, int value);
	void WriteBool(const wchar_t* name, bool value);
	void WriteString(const wchar_t* name, const std::wstring& value);

	void ReadInt(const wchar_t* name, int& value);
	void ReadBool(const wchar_t* name, bool& value);
	void ReadString(const wchar_t* name, std::wstring& value);
};

extern XmlToolsConfig config;