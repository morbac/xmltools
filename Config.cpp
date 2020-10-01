#include "StdAfx.h"
#include "Config.h"

struct struct_proxyoptions proxyoptions = {};
struct struct_xmltoolsoptions xmltoolsoptions = {};
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
	ReadBool(L"useAnnotations", xmltoolsoptions.useAnnotations);
	ReadInt(L"annotationStyle", xmltoolsoptions.annotationStyle);
	ReadBool(L"convertAmp", xmltoolsoptions.convertAmp);
	ReadBool(L"convertLt", xmltoolsoptions.convertLt);
	ReadBool(L"convertGt", xmltoolsoptions.convertGt);
	ReadBool(L"convertQuote", xmltoolsoptions.convertQuote);
	ReadBool(L"convertApos", xmltoolsoptions.convertApos);
	ReadBool(L"ppAutoclose", xmltoolsoptions.ppAutoclose);
	ReadBool(L"trimTextWhitespace", xmltoolsoptions.trimTextWhitespace);

	ReadInt(L"allowDocumentFunction", xmltoolsoptions.allowDocumentFunction);
	ReadInt(L"allowXsltScript", xmltoolsoptions.allowXsltScript);
	ReadInt(L"forceResync", xmltoolsoptions.forceResync);
	ReadInt(L"maxElementDepth", xmltoolsoptions.maxElementDepth);
	ReadInt(L"maxXMLSize", xmltoolsoptions.maxXMLSize);
	ReadInt(L"multipleErrorMessages", xmltoolsoptions.multipleErrorMessages);
	ReadInt(L"newParser", xmltoolsoptions.newParser);
	ReadInt(L"normalizeAttributeValues", xmltoolsoptions.normalizeAttributeValues);
	ReadInt(L"populateElementDefaultValues", xmltoolsoptions.populateElementDefaultValues);
	ReadInt(L"prohibitDTD", xmltoolsoptions.prohibitDTD);
	ReadInt(L"resolveExternals", xmltoolsoptions.resolveExternals);
	ReadString(L"selectionLanguage", xmltoolsoptions.selectionLanguage);
	ReadString(L"selectionNamespace", xmltoolsoptions.selectionNamespace);
	ReadInt(L"serverHTTPRequest", xmltoolsoptions.serverHTTPRequest);
	ReadInt(L"useInlineSchema", xmltoolsoptions.useInlineSchema);
	ReadInt(L"validateOnParse", xmltoolsoptions.validateOnParse);
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
	WriteBool(L"useAnnotations", xmltoolsoptions.useAnnotations);
	WriteInt( L"annotationStyle", xmltoolsoptions.annotationStyle);
	WriteBool(L"convertAmp", xmltoolsoptions.convertAmp);
	WriteBool(L"convertLt", xmltoolsoptions.convertLt);
	WriteBool(L"convertGt", xmltoolsoptions.convertGt);
	WriteBool(L"convertQuote", xmltoolsoptions.convertQuote);
	WriteBool(L"convertApos", xmltoolsoptions.convertApos);
	WriteBool(L"ppAutoclose", xmltoolsoptions.ppAutoclose);
	WriteBool(L"trimTextWhitespace", xmltoolsoptions.trimTextWhitespace);

	WriteInt(L"allowDocumentFunction", xmltoolsoptions.allowDocumentFunction);
	WriteInt(L"allowXsltScript", xmltoolsoptions.allowXsltScript);
	WriteInt(L"forceResync", xmltoolsoptions.forceResync);
	WriteInt(L"maxElementDepth", xmltoolsoptions.maxElementDepth);
	WriteInt(L"maxXMLSize", xmltoolsoptions.maxXMLSize);
	WriteInt(L"multipleErrorMessages", xmltoolsoptions.multipleErrorMessages);
	WriteInt(L"newParser", xmltoolsoptions.newParser);
	WriteInt(L"normalizeAttributeValues", xmltoolsoptions.normalizeAttributeValues);
	WriteInt(L"populateElementDefaultValues",xmltoolsoptions.populateElementDefaultValues);
	WriteInt(L"prohibitDTD", xmltoolsoptions.prohibitDTD);
	WriteInt(L"resolveExternals", xmltoolsoptions.resolveExternals);
	WriteString(L"selectionLanguage", xmltoolsoptions.selectionLanguage);
	WriteString(L"selectionNamespace", xmltoolsoptions.selectionNamespace);
	WriteInt(L"serverHTTPRequest", xmltoolsoptions.serverHTTPRequest);
	WriteInt(L"useInlineSchema", xmltoolsoptions.useInlineSchema);
	WriteInt(L"validateOnParse", xmltoolsoptions.validateOnParse);

	WriteInt(L"dbgLevel", (int)config.dbgLevel);
}