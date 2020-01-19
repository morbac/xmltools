#include <msxml6.h>

// Macro that calls a COM method returning HRESULT value.
#define CHK_HR(stmt)        do { hr=(stmt); if (FAILED(hr)) goto CleanUp; } while(0)

// Macro to verify memory allcation.
#define CHK_ALLOC(p)        do { if (!(p)) { hr = E_OUTOFMEMORY; goto CleanUp; } } while(0)

// Macro that releases a COM object if not NULL.
#define SAFE_RELEASE(p)     do { if ((p)) { (p)->Release(); (p) = NULL; } } while(0)

// Helper function packaging an object into a variant:
HRESULT VariantFromObject(IUnknown * pUnk, VARIANT & varObject);

// Helper function to create a DOM instance. 
HRESULT CreateAndInitDOM(IXMLDOMDocument * *ppDoc);

// Helper function to create a sax instance. 
HRESULT CreateAndInitSAX(ISAXXMLReader * *ppDoc);

// Helper function to transform DOM to a string. 
HRESULT TransformDOM2Str(IXMLDOMDocument * pXMLDom, IXMLDOMDocument * pXSLDoc);

// Helper function to display parse error.
// It returns error code of the parse error.
HRESULT ReportParseError(IXMLDOMDocument * pDoc, const char* szDesc);

