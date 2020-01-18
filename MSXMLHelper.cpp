#include "MSXMLHelper.h"
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
HRESULT CreateAndInitDOM(IXMLDOMDocument** ppDoc) {
  HRESULT hr = CoCreateInstance(__uuidof(DOMDocument60), NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(ppDoc));
  if (SUCCEEDED(hr)) {
    // These methods should not fail so don't inspect result
    (*ppDoc)->put_async(VARIANT_FALSE);
    (*ppDoc)->put_validateOnParse(VARIANT_FALSE);
    (*ppDoc)->put_resolveExternals(VARIANT_FALSE);
    (*ppDoc)->put_preserveWhiteSpace(VARIANT_TRUE);
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