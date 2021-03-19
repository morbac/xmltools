#include "StdAfx.h"
#include "Config.h"

struct struct_proxyoptions proxyoptions = {};
struct struct_xmltoolsoptions xmltoolsoptions = {};
struct struct_msxmloptions msxmloptions = {};
XmlToolsConfig config;

void XmlToolsConfig::WriteString(const wchar_t* name, const std::wstring& value) {
	::WritePrivateProfileString(XmlToolsConfig::sectionName, name, value.c_str(), configPath.c_str());
}

void XmlToolsConfig::WriteInt(const wchar_t* name, int value) {
	WriteString(name, std::to_wstring(static_cast<int>(value)));
}

void XmlToolsConfig::WriteBool(const wchar_t* name, bool value) {
	WriteString(name, value? L"1" : L"0");
}

void XmlToolsConfig::ReadInt(const wchar_t* name, int &value) {
	value = ::GetPrivateProfileInt(XmlToolsConfig::sectionName, name, value, configPath.c_str());
}

void XmlToolsConfig::ReadBool(const wchar_t* name, bool &value) {
	value = ::GetPrivateProfileInt(XmlToolsConfig::sectionName, name, value, configPath.c_str()) == 1;
}

void XmlToolsConfig::ReadString(const wchar_t* name, std::wstring &value) {
	wchar_t tmp[4096];
	::GetPrivateProfileString(XmlToolsConfig::sectionName, name, value.c_str(), tmp,sizeof(tmp)/2, configPath.c_str());
	value = tmp;
}


void XmlToolsConfig::Read(std::wstring _configPath) {
	configPath = _configPath + L"\\" + configFileName;
	
	ReadBool(L"doCheckXML", doCheckXML);
	ReadBool(L"doValidation", doValidation);
	//::ReadPrivateProfileString(sectionName, L"doPrettyPrint", doPrettyPrint?L"1":L"0", iniFilePath);
	ReadBool(L"doCloseTag", doCloseTag);
	//::ReadPrivateProfileString(sectionName, L"doAutoIndent", doAutoIndent?L"1":L"0", iniFilePath);
	//::ReadPrivateProfileString(sectionName, L"doAttrAutoComplete", doAttrAutoComplete?L"1":L"0", iniFilePath);
	ReadBool(L"doAutoXMLType", doAutoXMLType);
	ReadBool(L"doPreventXXE", doPreventXXE);
	ReadBool(L"doAllowHuge", doAllowHuge);
	ReadBool(L"doPrettyPrintAllOpenFiles", doPrettyPrintAllOpenFiles);

	ReadBool(L"proxyEnabled", proxyoptions.status);
	ReadString(L"proxyHost", proxyoptions.host);
	ReadInt(L"proxyPort", proxyoptions.port);
	ReadString(L"proxyUser", proxyoptions.username);
	ReadString(L"proxyPass", proxyoptions.password);

	ReadString(L"formatingEngine", xmltoolsoptions.formatingEngine);
	ReadString(L"errorDisplayMode", xmltoolsoptions.errorDisplayMode);
	ReadInt(L"annotationStyle", xmltoolsoptions.annotationStyle);
	ReadInt(L"maxIndentLevel", xmltoolsoptions.maxIndentLevel);
	ReadBool(L"convertAmp", xmltoolsoptions.convertAmp);
	ReadBool(L"convertLt", xmltoolsoptions.convertLt);
	ReadBool(L"convertGt", xmltoolsoptions.convertGt);
	ReadBool(L"convertQuote", xmltoolsoptions.convertQuote);
	ReadBool(L"convertApos", xmltoolsoptions.convertApos);
	ReadBool(L"ppAutoclose", xmltoolsoptions.ppAutoclose);
	ReadBool(L"ensureConformity", xmltoolsoptions.ensureConformity);

	ReadBool(L"tbCheckXML", xmltoolsoptions.tbCheckXML);
	ReadBool(L"tbValidateXML", xmltoolsoptions.tbValidateXML);
	ReadBool(L"tbPrevError", xmltoolsoptions.tbPrevError);
	ReadBool(L"tbNextError", xmltoolsoptions.tbNextError);
	ReadBool(L"tbPrettyPrint", xmltoolsoptions.tbPrettyPrint);
	ReadBool(L"tbPrettyPrintIndentAttr", xmltoolsoptions.tbPrettyPrintIndentAttr);
	ReadBool(L"tbPrettyPrintIndentOnly", xmltoolsoptions.tbPrettyPrintIndentOnly);
	ReadBool(L"tbLinearize", xmltoolsoptions.tbLinearize);
	ReadBool(L"tbCurrentXMLPath", xmltoolsoptions.tbCurrentXMLPath);
	ReadBool(L"tbCurrentXMLPathNS", xmltoolsoptions.tbCurrentXMLPathNS);
	ReadBool(L"tbEvalXPath", xmltoolsoptions.tbEvalXPath);
	ReadBool(L"tbXSLTransform", xmltoolsoptions.tbXSLTransform);
	ReadBool(L"tbEscape", xmltoolsoptions.tbEscape);
	ReadBool(L"tbUnescape", xmltoolsoptions.tbUnescape);
	ReadBool(L"tbComment", xmltoolsoptions.tbComment);
	ReadBool(L"tbUncomment", xmltoolsoptions.tbUncomment);
	ReadBool(L"tbOptions", xmltoolsoptions.tbOptions);

	ReadInt(L"allowDocumentFunction", msxmloptions.allowDocumentFunction);
	ReadInt(L"allowXsltScript", msxmloptions.allowXsltScript);
	ReadInt(L"forceResync", msxmloptions.forceResync);
	ReadInt(L"maxElementDepth", msxmloptions.maxElementDepth);
	ReadInt(L"maxXMLSize", msxmloptions.maxXMLSize);
	ReadInt(L"multipleErrorMessages", msxmloptions.multipleErrorMessages);
	ReadInt(L"newParser", msxmloptions.newParser);
	ReadInt(L"normalizeAttributeValues", msxmloptions.normalizeAttributeValues);
	ReadInt(L"populateElementDefaultValues", msxmloptions.populateElementDefaultValues);
	ReadInt(L"prohibitDTD", msxmloptions.prohibitDTD);
	ReadInt(L"resolveExternals", msxmloptions.resolveExternals);
	ReadString(L"selectionLanguage", msxmloptions.selectionLanguage);
	ReadString(L"selectionNamespace", msxmloptions.selectionNamespace);
	ReadInt(L"serverHTTPRequest", msxmloptions.serverHTTPRequest);
	ReadInt(L"useInlineSchema", msxmloptions.useInlineSchema);
	ReadInt(L"validateOnParse", msxmloptions.validateOnParse);
	int dbgLevel = static_cast<int>(config.dbgLevel);
	ReadInt(L"dbgLevel", dbgLevel);
	config.dbgLevel = static_cast<DBG_LEVEL>(dbgLevel);
}

void XmlToolsConfig::Write() {
	WriteBool(L"doCheckXML", doCheckXML);
	WriteBool(L"doValidation", doValidation);
	//::WritePrivateProfileString(sectionName, L"doPrettyPrint", doPrettyPrint?L"1":L"0", iniFilePath);
	WriteBool(L"doCloseTag", doCloseTag);
	//::WritePrivateProfileString(sectionName, L"doAutoIndent", doAutoIndent?L"1":L"0", iniFilePath);
	//::WritePrivateProfileString(sectionName, L"doAttrAutoComplete", doAttrAutoComplete?L"1":L"0", iniFilePath);
	WriteBool(L"doAutoXMLType", doAutoXMLType);
	WriteBool(L"doPreventXXE", doPreventXXE);
	WriteBool(L"doAllowHuge", doAllowHuge);
	WriteBool(L"doPrettyPrintAllOpenFiles", doPrettyPrintAllOpenFiles);

	WriteBool(L"proxyEnabled", proxyoptions.status);
	WriteString(L"proxyHost", proxyoptions.host);
	WriteInt(L"proxyPort", proxyoptions.port);
	WriteString(L"proxyUser", proxyoptions.username);
	WriteString(L"proxyPass", proxyoptions.password);

	WriteString(L"formatingEngine", xmltoolsoptions.formatingEngine);
	WriteString(L"errorDisplayMode", xmltoolsoptions.errorDisplayMode);
	WriteInt( L"annotationStyle", xmltoolsoptions.annotationStyle);
	WriteInt(L"maxIndentLevel", xmltoolsoptions.maxIndentLevel);
	WriteBool(L"convertAmp", xmltoolsoptions.convertAmp);
	WriteBool(L"convertLt", xmltoolsoptions.convertLt);
	WriteBool(L"convertGt", xmltoolsoptions.convertGt);
	WriteBool(L"convertQuote", xmltoolsoptions.convertQuote);
	WriteBool(L"convertApos", xmltoolsoptions.convertApos);
	WriteBool(L"ppAutoclose", xmltoolsoptions.ppAutoclose);
	WriteBool(L"ensureConformity", xmltoolsoptions.ensureConformity);

	WriteBool(L"tbCheckXML", xmltoolsoptions.tbCheckXML);
	WriteBool(L"tbValidateXML", xmltoolsoptions.tbValidateXML);
	WriteBool(L"tbPrevError", xmltoolsoptions.tbPrevError);
	WriteBool(L"tbNextError", xmltoolsoptions.tbNextError);
	WriteBool(L"tbPrettyPrint", xmltoolsoptions.tbPrettyPrint);
	WriteBool(L"tbPrettyPrintIndentAttr", xmltoolsoptions.tbPrettyPrintIndentAttr);
	WriteBool(L"tbPrettyPrintIndentOnly", xmltoolsoptions.tbPrettyPrintIndentOnly);
	WriteBool(L"tbLinearize", xmltoolsoptions.tbLinearize);
	WriteBool(L"tbCurrentXMLPath", xmltoolsoptions.tbCurrentXMLPath);
	WriteBool(L"tbCurrentXMLPathNS", xmltoolsoptions.tbCurrentXMLPathNS);
	WriteBool(L"tbEvalXPath", xmltoolsoptions.tbEvalXPath);
	WriteBool(L"tbXSLTransform", xmltoolsoptions.tbXSLTransform);
	WriteBool(L"tbEscape", xmltoolsoptions.tbEscape);
	WriteBool(L"tbUnescape", xmltoolsoptions.tbUnescape);
	WriteBool(L"tbComment", xmltoolsoptions.tbComment);
	WriteBool(L"tbUncomment", xmltoolsoptions.tbUncomment);
	WriteBool(L"tbOptions", xmltoolsoptions.tbOptions);

	WriteInt(L"allowDocumentFunction", msxmloptions.allowDocumentFunction);
	WriteInt(L"allowXsltScript", msxmloptions.allowXsltScript);
	WriteInt(L"forceResync", msxmloptions.forceResync);
	WriteInt(L"maxElementDepth", msxmloptions.maxElementDepth);
	WriteInt(L"maxXMLSize", msxmloptions.maxXMLSize);
	WriteInt(L"multipleErrorMessages", msxmloptions.multipleErrorMessages);
	WriteInt(L"newParser", msxmloptions.newParser);
	WriteInt(L"normalizeAttributeValues", msxmloptions.normalizeAttributeValues);
	WriteInt(L"populateElementDefaultValues", msxmloptions.populateElementDefaultValues);
	WriteInt(L"prohibitDTD", msxmloptions.prohibitDTD);
	WriteInt(L"resolveExternals", msxmloptions.resolveExternals);
	WriteString(L"selectionLanguage", msxmloptions.selectionLanguage);
	WriteString(L"selectionNamespace", msxmloptions.selectionNamespace);
	WriteInt(L"serverHTTPRequest", msxmloptions.serverHTTPRequest);
	WriteInt(L"useInlineSchema", msxmloptions.useInlineSchema);
	WriteInt(L"validateOnParse", msxmloptions.validateOnParse);

	WriteInt(L"dbgLevel", (int)config.dbgLevel);
}