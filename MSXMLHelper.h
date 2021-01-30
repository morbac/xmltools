#ifndef __MSXMLHELPER_H__
#define __MSXMLHELPER_H__

#include <MsXml6.h>
#include <string>
#include <vector>

/*
https://docs.microsoft.com/en-us/previous-versions/windows/desktop/ms757065(v%3Dvs.85)

All XML processors are required to understand two transformations of the Unicode character
encoding, UTF-8 and UTF-16. Microsoft XML Core Services (MSXML) supports more encodings,
but all text in XML documents is treated internally as the Unicode UCS-2 character encoding.
*/

// Macro that calls a COM method returning HRESULT value.
#define CHK_HR(stmt)        do { hr=(stmt); if (FAILED(hr)) goto CleanUp; } while(0)

// Macro to verify memory allcation.
#define CHK_ALLOC(p)        do { if (!(p)) { hr = E_OUTOFMEMORY; goto CleanUp; } } while(0)

// Macro that releases a COM object if not NULL.
#define SAFE_RELEASE(p)     do { if ((p)) { (p)->Release(); (p) = NULL; } } while(0)

typedef enum {
  INIT_OPTION_ASYNC              = 1 << 0,
  INIT_OPTION_VALIDATEONPARSE    = 1 << 1,
  INIT_OPTION_RESOLVEEXTERNALS   = 1 << 2,
  INIT_OPTION_PRESERVEWHITESPACE = 1 << 3,
  INIT_OPTION_ONDATAAVAILABLE    = 1 << 4,  // not used
  INIT_OPTION_ONTRANSFORMNODE    = 1 << 5,  // not used

  INIT_OPTION_FREETHREADED       = 1 << 9
} xmlInitOptions;

// Helper function to create a VT_BSTR variant from a null terminated string.
HRESULT VariantFromString(PCWSTR wszValue, VARIANT& Variant);

// Helper function packaging an object into a variant.
HRESULT VariantFromObject(IUnknown* pUnk, VARIANT& varObject);

// Helper function to create a DOM instance.
HRESULT CreateAndInitDOM(IXMLDOMDocument3** ppDoc, int options = INIT_OPTION_PRESERVEWHITESPACE);

// Helper function to create a sax instance.
HRESULT CreateAndInitSAX(ISAXXMLReader** ppDoc);

// Helper function to create a xsl instance.
HRESULT CreateAndInitXSLTemplate(IXSLTemplate** pIXSLTemplate);

// Helper function to create a xsd schema cache.
HRESULT CreateAndInitSchema(IXMLDOMSchemaCollection2** pISchema);

// Helper function to transform DOM to a string.
HRESULT TransformDOM2Str(IXMLDOMDocument* pXMLDom, IXMLDOMDocument* pXSLDoc);

// Helper function to display parse error.
// It returns error code of the parse error.
HRESULT ReportParseError(IXMLDOMDocument* pDoc, const char* szDesc);

#endif
