#include "StdAfx.h"
#include "Config.h"

struct struct_proxyoptions proxyoptions;
struct struct_xmltoolsoptions xmltoolsoptions;
struct struct_msxmloptions msxmloptions;
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
	::GetPrivateProfileString(XmlToolsConfig::sectionName, name, value.c_str(), tmp, sizeof(tmp)/2, configPath.c_str());
	value = tmp;
}


void XmlToolsConfig::Read(std::wstring _configPath) {
	configPath = _configPath + L"\\" + configFileName;

	ReadString(L"xmlEngine", xmltoolsoptions.xmlEngine);
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
	ReadInt(L"annotationHighlightStyle", xmltoolsoptions.annotationHighlightStyle);
	ReadInt(L"maxErrorsNum", xmltoolsoptions.maxErrorsNum);
	ReadInt(L"maxIndentLevel", xmltoolsoptions.maxIndentLevel);
	ReadBool(L"xpathOnStatusbar", xmltoolsoptions.xpathOnStatusbar);
	ReadBool(L"dumpAttributeName", xmltoolsoptions.dumpAttributeName);
	ReadBool(L"printXPathIndex", xmltoolsoptions.printXPathIndex);
	ReadString(L"identityAttributes", xmltoolsoptions.identityAttributes);

	ReadBool(L"convertAmp", xmltoolsoptions.convertAmp);
	ReadBool(L"convertLt", xmltoolsoptions.convertLt);
	ReadBool(L"convertGt", xmltoolsoptions.convertGt);
	ReadBool(L"convertQuote", xmltoolsoptions.convertQuote);
	ReadBool(L"convertApos", xmltoolsoptions.convertApos);
	ReadBool(L"ppAutoclose", xmltoolsoptions.ppAutoclose);
	ReadBool(L"ensureConformity", xmltoolsoptions.ensureConformity);
	ReadBool(L"applySpacePreserve", xmltoolsoptions.applySpacePreserve);

	ReadBool(L"tbEnabled", xmltoolsoptions.tbEnabled);
	ReadBool(L"tbCheckXML", xmltoolsoptions.tbCheckXML);
	ReadBool(L"tbValidateXML", xmltoolsoptions.tbValidateXML);
	ReadBool(L"tbFirstError", xmltoolsoptions.tbFirstError);
	ReadBool(L"tbPrevError", xmltoolsoptions.tbPrevError);
	ReadBool(L"tbNextError", xmltoolsoptions.tbNextError);
	ReadBool(L"tbLastError", xmltoolsoptions.tbLastError);
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

	ReadInt(L"msxml.allowDocumentFunction", msxmloptions.allowDocumentFunction);
	ReadInt(L"msxml.allowXsltScript", msxmloptions.allowXsltScript);
	ReadInt(L"msxml.forceResync", msxmloptions.forceResync);
	ReadInt(L"msxml.maxElementDepth", msxmloptions.maxElementDepth);
	ReadInt(L"msxml.maxXMLSize", msxmloptions.maxXMLSize);
	ReadInt(L"msxml.multipleErrorMessages", msxmloptions.multipleErrorMessages);
	ReadInt(L"msxml.newParser", msxmloptions.newParser);
	ReadInt(L"msxml.normalizeAttributeValues", msxmloptions.normalizeAttributeValues);
	ReadInt(L"msxml.populateElementDefaultValues", msxmloptions.populateElementDefaultValues);
	ReadInt(L"msxml.prohibitDTD", msxmloptions.prohibitDTD);
	ReadInt(L"msxml.resolveExternals", msxmloptions.resolveExternals);
	ReadString(L"msxml.selectionLanguage", msxmloptions.selectionLanguage);
	ReadString(L"msxml.selectionNamespace", msxmloptions.selectionNamespace);
	ReadInt(L"msxml.serverHTTPRequest", msxmloptions.serverHTTPRequest);
	ReadInt(L"msxml.useInlineSchema", msxmloptions.useInlineSchema);
	ReadInt(L"msxml.validateOnParse", msxmloptions.validateOnParse);

	int dbgLevel = static_cast<int>(config.dbgLevel);
	ReadInt(L"dbgLevel", dbgLevel);
	config.dbgLevel = static_cast<DBG_LEVEL>(dbgLevel);
}

void XmlToolsConfig::Write() {
	WriteString(L"xmlEngine", xmltoolsoptions.xmlEngine);
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
	WriteInt(L"annotationStyle", xmltoolsoptions.annotationStyle);
	WriteInt(L"annotationHighlightStyle", xmltoolsoptions.annotationHighlightStyle);
	WriteInt(L"maxIndentLevel", xmltoolsoptions.maxIndentLevel);
	WriteInt(L"maxErrorsNum", xmltoolsoptions.maxErrorsNum);
	WriteBool(L"xpathOnStatusbar", xmltoolsoptions.xpathOnStatusbar);
	WriteBool(L"dumpAttributeName", xmltoolsoptions.dumpAttributeName);
	WriteBool(L"printXPathIndex", xmltoolsoptions.printXPathIndex);
	WriteString(L"identityAttributes", xmltoolsoptions.identityAttributes);

	WriteBool(L"convertAmp", xmltoolsoptions.convertAmp);
	WriteBool(L"convertLt", xmltoolsoptions.convertLt);
	WriteBool(L"convertGt", xmltoolsoptions.convertGt);
	WriteBool(L"convertQuote", xmltoolsoptions.convertQuote);
	WriteBool(L"convertApos", xmltoolsoptions.convertApos);
	WriteBool(L"ppAutoclose", xmltoolsoptions.ppAutoclose);
	WriteBool(L"ensureConformity", xmltoolsoptions.ensureConformity);
	WriteBool(L"applySpacePreserve", xmltoolsoptions.applySpacePreserve);

	WriteBool(L"tbEnabled", xmltoolsoptions.tbEnabled);
	WriteBool(L"tbCheckXML", xmltoolsoptions.tbCheckXML);
	WriteBool(L"tbValidateXML", xmltoolsoptions.tbValidateXML);
	WriteBool(L"tbFirstError", xmltoolsoptions.tbFirstError);
	WriteBool(L"tbPrevError", xmltoolsoptions.tbPrevError);
	WriteBool(L"tbNextError", xmltoolsoptions.tbNextError);
	WriteBool(L"tbLastError", xmltoolsoptions.tbLastError);
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

	WriteInt(L"msxml.allowDocumentFunction", msxmloptions.allowDocumentFunction);
	WriteInt(L"msxml.allowXsltScript", msxmloptions.allowXsltScript);
	WriteInt(L"msxml.forceResync", msxmloptions.forceResync);
	WriteInt(L"msxml.maxElementDepth", msxmloptions.maxElementDepth);
	WriteInt(L"msxml.maxXMLSize", msxmloptions.maxXMLSize);
	WriteInt(L"msxml.multipleErrorMessages", msxmloptions.multipleErrorMessages);
	WriteInt(L"msxml.newParser", msxmloptions.newParser);
	WriteInt(L"msxml.normalizeAttributeValues", msxmloptions.normalizeAttributeValues);
	WriteInt(L"msxml.populateElementDefaultValues", msxmloptions.populateElementDefaultValues);
	WriteInt(L"msxml.prohibitDTD", msxmloptions.prohibitDTD);
	WriteInt(L"msxml.resolveExternals", msxmloptions.resolveExternals);
	WriteString(L"msxml.selectionLanguage", msxmloptions.selectionLanguage);
	WriteString(L"msxml.selectionNamespace", msxmloptions.selectionNamespace);
	WriteInt(L"msxml.serverHTTPRequest", msxmloptions.serverHTTPRequest);
	WriteInt(L"msxml.useInlineSchema", msxmloptions.useInlineSchema);
	WriteInt(L"msxml.validateOnParse", msxmloptions.validateOnParse);

	WriteInt(L"dbgLevel", (int)config.dbgLevel);
}