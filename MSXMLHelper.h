#ifndef __MSXMLHELPER_H__
#define __MSXMLHELPER_H__

#include <msxml6.h>
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
HRESULT CreateAndInitDOM(IXMLDOMDocument2** ppDoc, int options = INIT_OPTION_PRESERVEWHITESPACE);

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


// Overload of pure virtual class ISAXContentHandler; this
// class is used to construct path of current cursor position
class PathBuilder : public ISAXContentHandler {
  std::vector<std::wstring> vPath;

public:
  virtual HRESULT STDMETHODCALLTYPE startElement(
    /* [in] */ const wchar_t* pwchNamespaceUri,
    /* [in] */ int cchNamespaceUri,
    /* [in] */ const wchar_t* pwchLocalName,
    /* [in] */ int cchLocalName,
    /* [in] */ const wchar_t* pwchQName,
    /* [in] */ int cchQName,
    /* [in] */ ISAXAttributes* pAttributes) {
    //prt(L"<%s", pwchLocalName, cchLocalName);
    std::wstring str;
    str.append(pwchQName, cchQName);
    vPath.push_back(str);
    return S_OK;
  }

  virtual HRESULT STDMETHODCALLTYPE endElement(
    /* [in] */ const wchar_t* pwchNamespaceUri,
    /* [in] */ int cchNamespaceUri,
    /* [in] */ const wchar_t* pwchLocalName,
    /* [in] */ int cchLocalName,
    /* [in] */ const wchar_t* pwchQName,
    /* [in] */ int cchQName) {
    vPath.pop_back();
    return S_OK;
  }

  std::wstring getPath(bool withNamespaces) {
    std::wstring res(L"");
    size_t size = vPath.size();

    if (withNamespaces) {
      for (size_t i = 0; i < size; ++i) {
        res.append(L"/");
        res.append(vPath.at(i));
      }
    }
    else {
      for (size_t i = 0; i < size; ++i) {
        res.append(L"/");
        std::wstring tmp = vPath.at(i);
        std::string::size_type p = tmp.find(':');
        if (p != std::string::npos) {
          res.append(tmp.substr(p + 1));
        }
        else {
          res.append(tmp);
        }
      }
    }

    return res;
  }

  // unchanged methods
  virtual HRESULT STDMETHODCALLTYPE putDocumentLocator(
    /* [in] */ ISAXLocator* pLocator) {
    return S_OK;
  }

  virtual HRESULT STDMETHODCALLTYPE startDocument(void) {
    return S_OK;
  }

  virtual HRESULT STDMETHODCALLTYPE endDocument(void) {
    return S_OK;
  }

  virtual HRESULT STDMETHODCALLTYPE startPrefixMapping(
    /* [in] */ const wchar_t* pwchPrefix,
    /* [in] */ int cchPrefix,
    /* [in] */ const wchar_t* pwchUri,
    /* [in] */ int cchUri) {
    return S_OK;
  }

  virtual HRESULT STDMETHODCALLTYPE endPrefixMapping(
    /* [in] */ const wchar_t* pwchPrefix,
    /* [in] */ int cchPrefix) {
    return S_OK;
  }

  virtual HRESULT STDMETHODCALLTYPE characters(
    /* [in] */ const wchar_t* pwchChars,
    /* [in] */ int cchChars) {
    return S_OK;
  }

  virtual HRESULT STDMETHODCALLTYPE ignorableWhitespace(
    /* [in] */ const wchar_t* pwchChars,
    /* [in] */ int cchChars) {
    return S_OK;
  }

  virtual HRESULT STDMETHODCALLTYPE processingInstruction(
    /* [in] */ const wchar_t* pwchTarget,
    /* [in] */ int cchTarget,
    /* [in] */ const wchar_t* pwchData,
    /* [in] */ int cchData) {
    return S_OK;
  }

  virtual HRESULT STDMETHODCALLTYPE skippedEntity(
    /* [in] */ const wchar_t* pwchName,
    /* [in] */ int cchName) {
    return S_OK;
  }

  virtual HRESULT STDMETHODCALLTYPE QueryInterface(
    /* [in] */ REFIID riid,
    /* [iid_is][out] */ _COM_Outptr_ void __RPC_FAR* __RPC_FAR* ppvObject) {
      *ppvObject = NULL;
      return E_NOINTERFACE;
  }

  virtual ULONG STDMETHODCALLTYPE AddRef(void) {
    return 0;
  }

  virtual ULONG STDMETHODCALLTYPE Release(void) {
    return 0;
  }
};

#endif
