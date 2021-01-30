#include "StdAfx.h"

#include "MSXMLHelper.h"
#include "XMLTools.h"
#include "Report.h"

// Helper function to create a VT_BSTR variant from a null terminated string.
HRESULT VariantFromString(PCWSTR wszValue, VARIANT& Variant) {
  HRESULT hr = S_OK;
  BSTR bstr = SysAllocString(wszValue);
  CHK_ALLOC(bstr);

  V_VT(&Variant) = VT_BSTR;
  V_BSTR(&Variant) = bstr;

CleanUp:
  return hr;
}

// Helper function packaging an object into a variant:
HRESULT VariantFromObject(IUnknown* pUnk, VARIANT& varObject) {
  IDispatch* pDisp = NULL;
  HRESULT hr = S_OK;
  VariantInit(&varObject);
  CHK_HR(pUnk->QueryInterface(IID_IDispatch, (void**)&pDisp));
  varObject.pdispVal = pDisp;
  varObject.vt = VT_DISPATCH;

CleanUp:
  return hr;
}

void ApplyOptions(IXMLDOMDocument3** ppDoc, int options) {
    // These methods should not fail so don't inspect result
    (*ppDoc)->put_async(options & INIT_OPTION_ASYNC ? VARIANT_TRUE : VARIANT_FALSE);
    (*ppDoc)->put_validateOnParse(options & INIT_OPTION_VALIDATEONPARSE ? VARIANT_TRUE : VARIANT_FALSE);
    (*ppDoc)->put_resolveExternals(options & INIT_OPTION_RESOLVEEXTERNALS ? VARIANT_TRUE : VARIANT_FALSE);
    (*ppDoc)->put_preserveWhiteSpace(options & INIT_OPTION_PRESERVEWHITESPACE ? VARIANT_TRUE : VARIANT_FALSE);

    if (msxmloptions.allowDocumentFunction >= 0) (*ppDoc)->setProperty(L"AllowDocumentFunction", _variant_t(msxmloptions.allowDocumentFunction > 0 ? VARIANT_TRUE : VARIANT_FALSE));
    if (msxmloptions.allowXsltScript >= 0) (*ppDoc)->setProperty(L"AllowXsltScript", _variant_t(msxmloptions.allowXsltScript > 0 ? VARIANT_TRUE : VARIANT_FALSE));
    if (msxmloptions.forceResync >= 0) (*ppDoc)->setProperty(L"ForceResync", _variant_t(msxmloptions.forceResync > 0 ? VARIANT_TRUE : VARIANT_FALSE));
    if (msxmloptions.maxElementDepth >= 0) (*ppDoc)->setProperty(L"MaxElementDepth", _variant_t(msxmloptions.maxElementDepth));
    if (msxmloptions.maxXMLSize >= 0) (*ppDoc)->setProperty(L"MaxXMLSize", _variant_t(msxmloptions.maxXMLSize));
    if (msxmloptions.multipleErrorMessages >= 0) (*ppDoc)->setProperty(L"MultipleErrorMessages", _variant_t(msxmloptions.multipleErrorMessages > 0 ? VARIANT_TRUE : VARIANT_FALSE));
    if (msxmloptions.newParser >= 0) (*ppDoc)->setProperty(L"NewParser", _variant_t(msxmloptions.newParser > 0 ? VARIANT_TRUE : VARIANT_FALSE));
    if (msxmloptions.normalizeAttributeValues >= 0) (*ppDoc)->setProperty(L"NormalizeAttributeValues", _variant_t(msxmloptions.normalizeAttributeValues > 0 ? VARIANT_TRUE : VARIANT_FALSE));
    if (msxmloptions.populateElementDefaultValues >= 0) (*ppDoc)->setProperty(L"PopulateElementDefaultValues", _variant_t(msxmloptions.populateElementDefaultValues > 0 ? VARIANT_TRUE : VARIANT_FALSE));
    if (msxmloptions.prohibitDTD >= 0) (*ppDoc)->setProperty(L"ProhibitDTD", _variant_t(msxmloptions.prohibitDTD > 0 ? VARIANT_TRUE : VARIANT_FALSE));
    if (msxmloptions.resolveExternals >= 0) (*ppDoc)->setProperty(L"ResolveExternals", _variant_t(msxmloptions.resolveExternals > 0 ? VARIANT_TRUE : VARIANT_FALSE));
    if (msxmloptions.selectionLanguage.length() >= 0) (*ppDoc)->setProperty(L"SelectionLanguage", _variant_t(msxmloptions.selectionLanguage.c_str()));
    if (msxmloptions.selectionNamespace.length() >= 0) (*ppDoc)->setProperty(L"SelectionNamespace", _variant_t(msxmloptions.selectionNamespace.c_str()));
    if (msxmloptions.serverHTTPRequest >= 0) (*ppDoc)->setProperty(L"ServerHTTPRequest", _variant_t(msxmloptions.serverHTTPRequest > 0 ? VARIANT_TRUE : VARIANT_FALSE));
    if (msxmloptions.useInlineSchema >= 0) (*ppDoc)->setProperty(L"UseInlineSchema", _variant_t(msxmloptions.useInlineSchema > 0 ? VARIANT_TRUE : VARIANT_FALSE));
    if (msxmloptions.validateOnParse >= 0) (*ppDoc)->setProperty(L"ValidateOnParse", _variant_t(msxmloptions.validateOnParse > 0 ? VARIANT_TRUE : VARIANT_FALSE));
}

// Helper function to create a DOM instance.
HRESULT CreateAndInitDOM(IXMLDOMDocument3** ppDoc, int options) {
    HRESULT hr;
    if (options & INIT_OPTION_FREETHREADED) {
        hr = CoCreateInstance(CLSID_FreeThreadedDOMDocument60, NULL, CLSCTX_SERVER, IID_PPV_ARGS(ppDoc));
    }
    else {
        hr = CoCreateInstance(CLSID_DOMDocument60, NULL, CLSCTX_INPROC_SERVER, IID_IXMLDOMDocument3, (LPVOID*)(ppDoc));
    }
    if (SUCCEEDED(hr)) {
        ApplyOptions(ppDoc, options);
    }
    return hr;
}

// Helper function to create a XSL instance.
HRESULT CreateAndInitSAX(ISAXXMLReader** ppDoc) {
  HRESULT hr = CoCreateInstance(CLSID_SAXXMLReader60, NULL, CLSCTX_ALL, IID_PPV_ARGS(ppDoc));
  if (SUCCEEDED(hr)) {
    (*ppDoc)->putFeature(L"prohibit-dtd", _variant_t(msxmloptions.prohibitDTD ? VARIANT_TRUE : VARIANT_FALSE));
  }
  return hr;
}

// Helper function to create a SAX instance.
HRESULT CreateAndInitXSLTemplate(IXSLTemplate** pIXSLTemplate) {
  HRESULT hr = CoCreateInstance(CLSID_XSLTemplate60, NULL, CLSCTX_SERVER, IID_IXSLTemplate, (LPVOID*)(pIXSLTemplate));
  if (SUCCEEDED(hr)) {
  }
  return hr;
}

// Helper function to create a schema collection instance.
HRESULT CreateAndInitSchema(IXMLDOMSchemaCollection2** pISchema) {
  HRESULT hr = CoCreateInstance(CLSID_XMLSchemaCache60, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(pISchema));
  if (SUCCEEDED(hr)) {
  }
  return hr;
}

// Helper function to transform DOM to a string.
HRESULT TransformDOM2Str(IXMLDOMDocument* pXMLDom, IXMLDOMDocument* pXSLDoc) {
  HRESULT hr = S_OK;
  BSTR bstrResult = NULL;
  CHK_HR(pXMLDom->transformNode(pXSLDoc, &bstrResult));
  Report::_printf_inf(L"Output from transformNode:\n%S\n", bstrResult);

CleanUp:
  SysFreeString(bstrResult);
  return hr;
}

// Helper function to display parse error.
// It returns error code of the parse error.
HRESULT ReportParseError(IXMLDOMDocument* pDoc, const char* szDesc) {
  HRESULT hr = S_OK;
  HRESULT hrRet = E_FAIL; // Default error code if failed to get from parse error.
  IXMLDOMParseError* pXMLErr = NULL;
  BSTR bstrReason = NULL;
  long line = 0;
  long linepos = 0;

  CHK_HR(pDoc->get_parseError(&pXMLErr));
  CHK_HR(pXMLErr->get_line(&line));
  CHK_HR(pXMLErr->get_linepos(&linepos));
  CHK_HR(pXMLErr->get_errorCode(&hrRet));
  CHK_HR(pXMLErr->get_reason(&bstrReason));
  Report::_printf_err(L"%s\n%S\nline %d, pos %d\n", szDesc, bstrReason, line, linepos);

CleanUp:
  SAFE_RELEASE(pXMLErr);
  SysFreeString(bstrReason);
  return hrRet;
}