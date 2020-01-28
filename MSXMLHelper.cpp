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

// Helper function to create a DOM instance. 
HRESULT CreateAndInitDOM(IXMLDOMDocument2** ppDoc, int options) {
  HRESULT hr;
  if (options & INIT_OPTION_FREETHREADED) {
    hr = CoCreateInstance(CLSID_FreeThreadedDOMDocument60, NULL, CLSCTX_SERVER, IID_PPV_ARGS(ppDoc));
  } else {
    hr = CoCreateInstance(CLSID_DOMDocument60, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(ppDoc));
  }
  if (SUCCEEDED(hr)) {
    // These methods should not fail so don't inspect result
    (*ppDoc)->put_async(options & INIT_OPTION_ASYNC ? VARIANT_TRUE : VARIANT_FALSE);
    (*ppDoc)->put_validateOnParse(options & INIT_OPTION_VALIDATEONPARSE ? VARIANT_TRUE : VARIANT_FALSE);
    (*ppDoc)->put_resolveExternals(options & INIT_OPTION_RESOLVEEXTERNALS ? VARIANT_TRUE : VARIANT_FALSE);
    (*ppDoc)->put_preserveWhiteSpace(options & INIT_OPTION_PRESERVEWHITESPACE ? VARIANT_TRUE : VARIANT_FALSE);

    (*ppDoc)->setProperty(L"ProhibitDTD", _variant_t(xmlfeatures.prohibitDTD ? VARIANT_TRUE : VARIANT_FALSE));
  }
  return hr;
}

// Helper function to create a XSL instance. 
HRESULT CreateAndInitSAX(ISAXXMLReader** ppDoc) {
  HRESULT hr = CoCreateInstance(CLSID_SAXXMLReader60, NULL, CLSCTX_ALL, IID_PPV_ARGS(ppDoc));
  if (SUCCEEDED(hr)) {
    (*ppDoc)->putFeature(L"prohibit-dtd", _variant_t(xmlfeatures.prohibitDTD ? VARIANT_TRUE : VARIANT_FALSE));
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

// Helper function to create a SAX instance. 
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