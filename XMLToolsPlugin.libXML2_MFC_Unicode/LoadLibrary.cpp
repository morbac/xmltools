#include "stdafx.h"

#include <windows.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <shellapi.h>
#include <shlwapi.h>
#include <shlobj.h>
#include "XMLTools.h"
#include "Report.h"

#include "LoadLibrary.h"

extern HANDLE		g_hModule;
HINSTANCE hInstLibXML = NULL;
HINSTANCE hInstLibXSL = NULL;

//-------------------------------------------------------------------------------------------------

void                   (*pXmlFree)(void *mem);
void                   (*pXmlFreeDoc)(xmlDocPtr cur);

xmlDocPtr              (*pXmlParseMemory)(const char * buffer, int size);
xmlDocPtr              (*pXmlReadMemory)(const char *buffer, int size, const char *URL, const char *encoding, int options);
xmlNodePtr             (*pXmlDocGetRootElement)(xmlDocPtr doc);
xmlDocPtr              (*pXmlParseFile)(const char * filename);
void                   (*pXmlInitParser)(void);
void                   (*pXmlCleanupParser)(void);

xmlSchemaPtr           (*pXmlSchemaParse)(xmlSchemaParserCtxtPtr ctxt);
xmlSchemaParserCtxtPtr (*pXmlSchemaNewParserCtxt)(const char *URL);
void                   (*pXmlSchemaFreeParserCtxt)(xmlSchemaParserCtxtPtr ctxt);
xmlSchemaValidCtxtPtr  (*pXmlSchemaNewValidCtxt)(xmlSchemaPtr schema);
void                   (*pXmlSchemaFree)(xmlSchemaPtr schema);
void                   (*pXmlSchemaSetValidErrors)(xmlSchemaValidCtxtPtr ctxt, xmlSchemaValidityErrorFunc err, xmlSchemaValidityWarningFunc warn, void *ctx);
void                   (*pXmlSchemaFreeValidCtxt)(xmlSchemaValidCtxtPtr ctxt);
int                    (*pXmlSchemaValidateDoc)(xmlSchemaValidCtxtPtr ctxt, xmlDocPtr instance);

xmlValidCtxtPtr        (*pXmlNewValidCtxt)(void);
void                   (*pXmlFreeValidCtxt)(xmlValidCtxtPtr);

xmlDtdPtr              (*pXmlParseDTD)(const xmlChar *ExternalID, const xmlChar *SystemID);
void                   (*pXmlFreeDtd)(xmlDtdPtr cur);
int                    (*pXmlValidateDtd)(xmlValidCtxtPtr ctxt, xmlDocPtr doc, xmlDtdPtr dtd);

xmlXPathContextPtr     (*pXmlXPathNewContext)(xmlDocPtr doc);
void                   (*pXmlXPathFreeContext)(xmlXPathContextPtr ctxt);
xmlXPathObjectPtr      (*pXmlXPathEvalExpression)(const xmlChar *str, xmlXPathContextPtr ctxt);
void                   (*pXmlXPathFreeObject)(xmlXPathObjectPtr obj);
int                    (*pXmlXPathRegisterNs)(xmlXPathContextPtr ctxt, const xmlChar *prefix, const xmlChar *ns_uri);

xmlErrorPtr            (*pXmlGetLastError)(void);
xmlChar *              (*pXmlGetProp)(xmlNodePtr node, const xmlChar *name);
int                    (*pXmlSubstituteEntitiesDefault)(int val);
xmlChar *              (*pXmlStrdup)(const xmlChar *cur);
const xmlChar *        (*pXmlStrchr)(const xmlChar *str, xmlChar val);

//-------------------------------------------------------------------------------------------------

xsltStylesheetPtr      (*pXsltParseStylesheetDoc)(xmlDocPtr doc);
xsltStylesheetPtr      (*pXsltParseStylesheetFile)(const xmlChar * filename);
int                    (*pXsltSaveResultToString)(xmlChar ** doc_txt_ptr, int * doc_txt_len, xmlDocPtr result, xsltStylesheetPtr style);
void                   (*pXsltFreeStylesheet)(xsltStylesheetPtr sheet);
xmlDocPtr              (*pXsltApplyStylesheet)(xsltStylesheetPtr style, xmlDocPtr doc, const char ** params);
void                   (*pXsltCleanupGlobals)(void);

//-------------------------------------------------------------------------------------------------

int loadLibXML(TCHAR* nppPath) {
  BOOL    bRet = FALSE;
  HKEY    hKey = NULL;
  DWORD   size = MAX_PATH;
  TCHAR   pszPath[MAX_PATH] = { '\0' };

  hInstLibXML = LoadLibrary(TEXT("libxml2.dll"));
  if (hInstLibXML == NULL) {
    Report::strcpy(pszPath, nppPath);
    PathAppend(pszPath, TEXT("\\XMLTools"));
    PathAppend(pszPath, _T("\\libxml2.dll"));
    //Report::_printf_inf(pszPath);
  
	  hInstLibXML = LoadLibrary(pszPath);

	  if (hInstLibXML == NULL) {
		  return -1;
    }
  }

  pXmlFree =                      (void (__cdecl *)(void *))*((void (__cdecl **)(void *))GetProcAddress(hInstLibXML, "xmlFree"));
  pXmlFreeDoc =                   (void (__cdecl *)(struct _xmlDoc *))GetProcAddress(hInstLibXML, "xmlFreeDoc");

  pXmlParseMemory =               (xmlDocPtr (__cdecl *)(const char *, int))GetProcAddress(hInstLibXML, "xmlParseMemory");
  pXmlReadMemory =                (xmlDocPtr (__cdecl *)(const char *, int, const char *, const char *, int))GetProcAddress(hInstLibXML, "xmlReadMemory");
  pXmlDocGetRootElement =         (xmlNodePtr (__cdecl *)(xmlDocPtr))GetProcAddress(hInstLibXML, "xmlDocGetRootElement");
  pXmlParseFile =                 (xmlDocPtr (__cdecl *)(const char *))GetProcAddress(hInstLibXML, "xmlParseFile");
  pXmlInitParser =                (void (__cdecl *)(void))GetProcAddress(hInstLibXML, "xmlInitParser");
  pXmlCleanupParser =             (void (__cdecl *)(void))GetProcAddress(hInstLibXML, "xmlCleanupParser");

  pXmlSchemaParse =               (xmlSchemaPtr (__cdecl *)(xmlSchemaParserCtxtPtr))GetProcAddress(hInstLibXML, "xmlSchemaParse");
  pXmlSchemaNewParserCtxt =       (xmlSchemaParserCtxtPtr (__cdecl *)(const char *))GetProcAddress(hInstLibXML, "xmlSchemaNewParserCtxt");
  pXmlSchemaFreeParserCtxt =      (void (__cdecl *)(xmlSchemaParserCtxtPtr))GetProcAddress(hInstLibXML, "xmlSchemaFreeParserCtxt");
  pXmlSchemaNewValidCtxt =        (xmlSchemaValidCtxtPtr (__cdecl *)(xmlSchemaPtr))GetProcAddress(hInstLibXML, "xmlSchemaNewValidCtxt");
  pXmlSchemaFree =                (void (__cdecl *)(xmlSchemaPtr))GetProcAddress(hInstLibXML, "xmlSchemaFree");
  pXmlSchemaSetValidErrors =      (void (__cdecl *)(xmlSchemaValidCtxtPtr, xmlSchemaValidityErrorFunc, xmlSchemaValidityWarningFunc, void *))GetProcAddress(hInstLibXML, "xmlSchemaSetValidErrors");
  pXmlSchemaFreeValidCtxt =       (void (__cdecl *)(xmlSchemaValidCtxtPtr))GetProcAddress(hInstLibXML, "xmlSchemaFreeValidCtxt");
  pXmlSchemaValidateDoc =         (int (__cdecl *)(xmlSchemaValidCtxtPtr, xmlDocPtr))GetProcAddress(hInstLibXML, "xmlSchemaValidateDoc");

  pXmlNewValidCtxt =              (xmlValidCtxtPtr (__cdecl *)(void))GetProcAddress(hInstLibXML, "xmlNewValidCtxt");
  pXmlFreeValidCtxt =             (void (__cdecl *)(xmlValidCtxtPtr))GetProcAddress(hInstLibXML, "xmlFreeValidCtxt");

  pXmlParseDTD =                  (xmlDtdPtr (__cdecl *)(const xmlChar *, const xmlChar *))GetProcAddress(hInstLibXML, "xmlParseDTD");
  pXmlFreeDtd =                   (void (__cdecl *)(xmlDtdPtr))GetProcAddress(hInstLibXML, "xmlFreeDtd");
  pXmlValidateDtd =               (int (__cdecl *)(xmlValidCtxtPtr, xmlDocPtr, xmlDtdPtr))GetProcAddress(hInstLibXML, "xmlValidateDtd");

  pXmlXPathNewContext =           (xmlXPathContextPtr (__cdecl *)(xmlDocPtr))GetProcAddress(hInstLibXML, "xmlXPathNewContext");
  pXmlXPathFreeContext =          (void (__cdecl *)(xmlXPathContextPtr))GetProcAddress(hInstLibXML, "xmlXPathFreeContext");
  pXmlXPathEvalExpression =       (xmlXPathObjectPtr (__cdecl *)(const xmlChar *, xmlXPathContextPtr))GetProcAddress(hInstLibXML, "xmlXPathEvalExpression");
  pXmlXPathFreeObject =           (void (__cdecl *)(xmlXPathObjectPtr))GetProcAddress(hInstLibXML, "xmlXPathFreeObject");
  pXmlXPathRegisterNs =           (int (__cdecl *)(xmlXPathContextPtr, const xmlChar *, const xmlChar *))GetProcAddress(hInstLibXML, "xmlXPathRegisterNs");

  pXmlGetLastError =              (xmlErrorPtr (__cdecl *)(void))GetProcAddress(hInstLibXML, "xmlGetLastError");
  pXmlGetProp =                   (xmlChar * (__cdecl *)(xmlNodePtr, const xmlChar *))GetProcAddress(hInstLibXML, "xmlGetProp");
  pXmlSubstituteEntitiesDefault = (int (__cdecl *)(int))GetProcAddress(hInstLibXML, "xmlSubstituteEntitiesDefault");
  pXmlStrdup =                    (xmlChar * (__cdecl *)(const xmlChar *))GetProcAddress(hInstLibXML, "xmlStrdup");
  pXmlStrchr =                    (const xmlChar * (__cdecl *)(const xmlChar *, xmlChar))GetProcAddress(hInstLibXML, "xmlStrchr");

  //-----------------------------------------------------------------------------------------------

  hInstLibXSL = LoadLibrary(TEXT("libxslt.dll"));
  if (hInstLibXSL == NULL) {
    Report::strcpy(pszPath, nppPath);
    PathAppend(pszPath, TEXT("\\XMLTools"));
    PathAppend(pszPath, TEXT("\\libxslt.dll"));
    //Report::_printf_inf(pszPath);
    
    hInstLibXSL = LoadLibrary(pszPath);
    
    if (hInstLibXSL == NULL) {
      return -1;
    }
  }
  
  /* xmlFree !!! pthreads? */
  
  pXsltParseStylesheetDoc =       (xsltStylesheetPtr (__cdecl *)(xmlDocPtr))GetProcAddress(hInstLibXSL, "xsltParseStylesheetDoc");
  pXsltParseStylesheetFile =      (xsltStylesheetPtr (__cdecl *)(const xmlChar *))GetProcAddress(hInstLibXSL, "xsltParseStylesheetFile");
  pXsltSaveResultToString =       (int (__cdecl *)(xmlChar **, int *, xmlDocPtr, xsltStylesheetPtr))GetProcAddress(hInstLibXSL, "xsltSaveResultToString");
  pXsltFreeStylesheet =           (void (__cdecl *)(xsltStylesheetPtr))GetProcAddress(hInstLibXSL, "xsltFreeStylesheet");
  pXsltApplyStylesheet =          (xmlDocPtr (__cdecl *)(xsltStylesheetPtr, xmlDocPtr, const char **))GetProcAddress(hInstLibXSL, "xsltApplyStylesheet");
  pXsltCleanupGlobals =           (void (__cdecl *)(void))GetProcAddress(hInstLibXSL, "xsltCleanupGlobals");
  
  /* init */
  pXmlInitParser();
  pXmlSubstituteEntitiesDefault(1);
  
  return 0;
}