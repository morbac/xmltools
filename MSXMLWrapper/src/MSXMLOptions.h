#pragma once
#include <string>

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
};
