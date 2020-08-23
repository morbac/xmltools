//#include "StdAfx.h"

#include <string>

//#include "XMLTools.h"

struct struct_proxyoptions {
	bool status;
	std::wstring host = L"";
	int port;
	std::wstring username = L"";
	std::wstring password = L"";
};

struct struct_xmltoolsoptions {       // default value
  // msxml features
	int allowDocumentFunction;         // True in 3.0. False in 6.0.
	int allowXsltScript;               // True in 3.0. False in 6.0.
	int forceResync;                   // True
	int maxElementDepth;                // 0 in 3.0. 256 in 6.0.
	int maxXMLSize;                     // 0
	int multipleErrorMessages;         // False
	int newParser;                     // False
	int normalizeAttributeValues;      // False
	int populateElementDefaultValues;  // False
	int prohibitDTD;                   // True in 3.0. False in 6.0.
	int resolveExternals;              // False
	std::wstring selectionLanguage = L"";     // "XSLPattern" in 3.0. "XPath" in 6.0
	std::wstring selectionNamespace = L"";    // ""
	int serverHTTPRequest;             // False
	int useInlineSchema;               // False
	int validateOnParse;               // True

	// xmltools options
	bool useAnnotations;                // False
	int annotationStyle;                // 12
	bool convertAmp;
	bool convertLt;
	bool convertGt;
	bool convertQuote;
	bool convertApos;
	bool ppAutoclose;
};

extern struct struct_proxyoptions proxyoptions;
extern struct struct_xmltoolsoptions xmltoolsoptions;

class XmlToolsConfig {
	const wchar_t* sectionName = L"XML Tools";
	const wchar_t* configFileName = L"XMLTools.ini";

	std::wstring configPath;

public:
	bool doCheckXML = false,
		doValidation = false,
		//doPrettyPrint = false
		doCloseTag = false,
		doAutoIndent = false,
		doAttrAutoComplete = false,
		doAutoXMLType = false,
		doPreventXXE = true,
		doAllowHuge = false,
		doPrettyPrintAllOpenFiles = false;

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