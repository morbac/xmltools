#include "stdafx.h"

#include <windows.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <shellapi.h>
#include <shlwapi.h>
#include <shlobj.h>
#include <assert.h>
#include "XMLTools.h"
#include "Report.h"

#include "LoadLibrary.h"

extern HANDLE  g_hModule;
HINSTANCE hInstLibXML  = NULL;
HINSTANCE hInstLibXSL  = NULL;

//-------------------------------------------------------------------------------------------------

void                   (*pXmlFree)(void *mem);
void                   (*pXmlFreeDoc)(xmlDocPtr cur);
void                   (*pXmlFreeNs)(xmlNsPtr cur);

xmlDocPtr              (*pXmlParseMemory)(const char * buffer, int size);
xmlDocPtr              (*pXmlReadMemory)(const char *buffer, int size, const char *URL, const char *encoding, int options);
const char *           (*pXmlGetCharEncodingName)(xmlCharEncoding enc);
const char *           (*pXmlGetEncodingAlias)(const char * alias);
xmlCharEncoding        (*pXmlParseCharEncoding)(const char * name);
int                    (*pUTF8Toisolat1)(unsigned char * out, int * outlen, const unsigned char * in, int * inlen);
int                    (*pisolat1ToUTF8)(unsigned char * out, int * outlen, const unsigned char * in, int * inlen);

int                     (*pXmlSaveFormatFile)(const char * filename, xmlDocPtr cur, int format);
void                   (*pXmlDocDumpFormatMemory)(xmlDocPtr cur, xmlChar ** mem, int * size, int format);
xmlNodePtr             (*pXmlDocGetRootElement)(xmlDocPtr doc);

void                   (*pXmlUnlinkNode)(xmlNodePtr node);
void                   (*pXmlFreeNode)(xmlNodePtr node);

xmlDocPtr              (*pXmlParseFile)(const char * filename);
void                   (*pXmlInitParser)(void);
void                   (*pXmlCleanupParser)(void);

xmlSchemaPtr           (*pXmlSchemaParse)(xmlSchemaParserCtxtPtr ctxt);
xmlSchemaParserCtxtPtr (*pXmlSchemaNewParserCtxt)(const char *URL);
void                   (*pXmlSchemaFreeParserCtxt)(xmlSchemaParserCtxtPtr ctxt);
xmlSchemaValidCtxtPtr  (*pXmlSchemaNewValidCtxt)(xmlSchemaPtr schema);
void                   (*pXmlSchemaFree)(xmlSchemaPtr schema);
xmlParserCtxtPtr       (*pXmlNewParserCtxt)(void);
void                   (*pXmlSchemaSetValidErrors)(xmlSchemaValidCtxtPtr ctxt, xmlSchemaValidityErrorFunc err, xmlSchemaValidityWarningFunc warn, void *ctx);
void                   (*pXmlSchemaFreeValidCtxt)(xmlSchemaValidCtxtPtr ctxt);
int                    (*pXmlSchemaValidateDoc)(xmlSchemaValidCtxtPtr ctxt, xmlDocPtr instance);
void                   (*pXmlSchemaValidateSetLocator)(xmlSchemaValidCtxtPtr vctxt, xmlSchemaValidityLocatorFunc f, void * ctxt);
//int                    (*pXmlSchemaValidityLocatorFunc)(void * ctx, const char ** file, unsigned long * line);

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
void                   (*pXmlResetLastError)(void);
void                   (*pXmlResetError)(xmlErrorPtr);
xmlChar *              (*pXmlGetProp)(xmlNodePtr node, const xmlChar *name);
int                    (*pXmlSubstituteEntitiesDefault)(int val);
xmlChar *              (*pXmlStrdup)(const xmlChar *cur);
const xmlChar *        (*pXmlStrchr)(const xmlChar *str, xmlChar val);

xmlGlobalStatePtr      (*pXmlGetGlobalState)(void);
int                    (*pXmlKeepBlanksDefault)(int val);
int                    (*pXmlThrDefIndentTreeOutput)(int v);
const char *           (*pXmlThrDefTreeIndentString)(const char * v);

void                   (*pXmlNanoHTTPInit)(void);
void                   (*pXmlNanoHTTPScanProxy)(const char * URL);
const char *           (*pXmlNanoHTTPAuthHeader)(void *ctx);


//-------------------------------------------------------------------------------------------------

xsltStylesheetPtr      (*pXsltParseStylesheetDoc)(xmlDocPtr doc);
xsltStylesheetPtr      (*pXsltParseStylesheetFile)(const xmlChar * filename);
int                    (*pXsltSaveResultToString)(xmlChar ** doc_txt_ptr, int * doc_txt_len, xmlDocPtr result, xsltStylesheetPtr style);
void                   (*pXsltFreeStylesheet)(xsltStylesheetPtr sheet);
xmlDocPtr              (*pXsltApplyStylesheet)(xsltStylesheetPtr style, xmlDocPtr doc, const char ** params);
void                   (*pXsltCleanupGlobals)(void);
void                   (*pXsltSetGenericErrorFunc)(void * ctx, xmlGenericErrorFunc handler);
xsltTransformContextPtr(*pXsltNewTransformContext)(xsltStylesheetPtr style, xmlDocPtr doc);

//-------------------------------------------------------------------------------------------------

HINSTANCE loadExtLib(const wchar_t* libFilename, const wchar_t* nppMainPath, const wchar_t* appDataPath) {
  wchar_t pszPath[MAX_PATH] = { '\0' };

  // try loading from NPP plugins path (standard NPP location)
  Report::strcpy(pszPath, nppMainPath);
  PathAppend(pszPath, L"plugins\\XMLTools\\");
  PathAppend(pszPath, libFilename);
  HINSTANCE res = LoadLibrary(pszPath);

  if (res == NULL) {
    // try loading from %appdata% path (standard NPP UAC/AppData location)
    Report::strcpy(pszPath, appDataPath);
    PathAppend(pszPath, L"Notepad++\\");
    PathAppend(pszPath, libFilename);
    res = LoadLibrary(pszPath);

    if (res == NULL) {
      // try loading from NPP sub path
      Report::strcpy(pszPath, nppMainPath);
      PathAppend(pszPath, L"XMLTools\\");
      PathAppend(pszPath, libFilename);
      res = LoadLibrary(pszPath);
    
      if (res == NULL) {
        // try loading (from NPP main path)
        res = LoadLibrary(libFilename);
      }
    }
  }
  return res;  
}

int loadLibraries(wchar_t* nppMainPath, wchar_t* appDataPath) {
  BOOL    bRet = FALSE;
  HKEY    hKey = NULL;
  DWORD   size = MAX_PATH;

  // loading dependencies
  if (loadExtLib(L"libiconv-2.dll", nppMainPath, appDataPath) == NULL) return -1;
  if (loadExtLib(L"zlib1.dll", nppMainPath, appDataPath) == NULL) return -1;
  if (loadExtLib(L"libwinpthread-1.dll", nppMainPath, appDataPath) == NULL) return -1;
  
  // loading LIBXML
  hInstLibXML = loadExtLib(L"libxml2-2.dll", nppMainPath, appDataPath);
  if (hInstLibXML == NULL) {
    return -1;
  }

  // loading LIBXSLT
  hInstLibXSL = loadExtLib(L"libxslt-1.dll", nppMainPath, appDataPath);
  if (hInstLibXSL == NULL) {
    return -1;
  }

  pXmlFree =                      (void (__cdecl *)(void *))*((void (__cdecl **)(void *))GetProcAddress(hInstLibXML, "xmlFree")); assert(pXmlFree);
  pXmlFreeDoc =                   (void (__cdecl *)(xmlDocPtr))GetProcAddress(hInstLibXML, "xmlFreeDoc"); assert(pXmlFreeDoc);
  pXmlFreeNs =                    (void (__cdecl *)(xmlNsPtr))GetProcAddress(hInstLibXML, "xmlFreeNs"); assert(pXmlFreeNs);

  pXmlParseMemory =               (xmlDocPtr (__cdecl *)(const char *, int))GetProcAddress(hInstLibXML, "xmlParseMemory"); assert(pXmlParseMemory);
  pXmlReadMemory =                (xmlDocPtr (__cdecl *)(const char *, int, const char *, const char *, int))GetProcAddress(hInstLibXML, "xmlReadMemory"); assert(pXmlReadMemory);
  
  pXmlGetCharEncodingName =       (const char * (__cdecl *)(xmlCharEncoding))GetProcAddress(hInstLibXML, "xmlGetCharEncodingName"); assert(pXmlGetCharEncodingName);
  pXmlGetEncodingAlias =          (const char * (__cdecl *)(const char *))GetProcAddress(hInstLibXML, "xmlGetEncodingAlias"); assert(pXmlGetEncodingAlias);
  pXmlParseCharEncoding =         (xmlCharEncoding (__cdecl *)(const char *))GetProcAddress(hInstLibXML, "xmlParseCharEncoding"); assert(pXmlParseCharEncoding);

  pUTF8Toisolat1 =                (int (__cdecl *)(unsigned char *, int *, const unsigned char *, int *))GetProcAddress(hInstLibXML, "UTF8Toisolat1"); assert(pUTF8Toisolat1);
  pisolat1ToUTF8 =                (int (__cdecl *)(unsigned char *, int *, const unsigned char *, int *))GetProcAddress(hInstLibXML, "isolat1ToUTF8"); assert(pisolat1ToUTF8);

  pXmlSaveFormatFile =            (int (__cdecl *)(const char *, xmlDocPtr, int))GetProcAddress(hInstLibXML, "xmlSaveFormatFile"); assert(pXmlSaveFormatFile);
  pXmlDocDumpFormatMemory =       (void  (__cdecl *)(xmlDocPtr, xmlChar **, int *, int))GetProcAddress(hInstLibXML, "xmlDocDumpFormatMemory"); assert(pXmlDocDumpFormatMemory);
  pXmlDocGetRootElement =         (xmlNodePtr (__cdecl *)(xmlDocPtr))GetProcAddress(hInstLibXML, "xmlDocGetRootElement"); assert(pXmlDocGetRootElement);
  pXmlUnlinkNode =                (void (__cdecl *)(xmlNodePtr))GetProcAddress(hInstLibXML, "xmlUnlinkNode"); assert(pXmlUnlinkNode);
  pXmlFreeNode =                  (void (__cdecl *)(xmlNodePtr))GetProcAddress(hInstLibXML, "xmlFreeNode"); assert(pXmlFreeNode);

  pXmlParseFile =                 (xmlDocPtr (__cdecl *)(const char *))GetProcAddress(hInstLibXML, "xmlParseFile"); assert(pXmlParseFile);
  pXmlInitParser =                (void (__cdecl *)(void))GetProcAddress(hInstLibXML, "xmlInitParser"); assert(pXmlInitParser);
  pXmlCleanupParser =             (void (__cdecl *)(void))GetProcAddress(hInstLibXML, "xmlCleanupParser"); assert(pXmlCleanupParser);

  pXmlSchemaParse =               (xmlSchemaPtr (__cdecl *)(xmlSchemaParserCtxtPtr))GetProcAddress(hInstLibXML, "xmlSchemaParse"); assert(pXmlSchemaParse);
  pXmlSchemaNewParserCtxt =       (xmlSchemaParserCtxtPtr (__cdecl *)(const char *))GetProcAddress(hInstLibXML, "xmlSchemaNewParserCtxt"); assert(pXmlSchemaNewParserCtxt);
  pXmlSchemaFreeParserCtxt =      (void (__cdecl *)(xmlSchemaParserCtxtPtr))GetProcAddress(hInstLibXML, "xmlSchemaFreeParserCtxt"); assert(pXmlSchemaFreeParserCtxt);
  pXmlSchemaNewValidCtxt =        (xmlSchemaValidCtxtPtr (__cdecl *)(xmlSchemaPtr))GetProcAddress(hInstLibXML, "xmlSchemaNewValidCtxt"); assert(pXmlSchemaNewValidCtxt);
  pXmlSchemaFree =                (void (__cdecl *)(xmlSchemaPtr))GetProcAddress(hInstLibXML, "xmlSchemaFree"); assert(pXmlSchemaFree);
  pXmlNewParserCtxt =             (xmlParserCtxtPtr (__cdecl *)(void))GetProcAddress(hInstLibXML, "xmlNewParserCtxt"); assert(pXmlNewParserCtxt);
  pXmlSchemaSetValidErrors =      (void (__cdecl *)(xmlSchemaValidCtxtPtr, xmlSchemaValidityErrorFunc, xmlSchemaValidityWarningFunc, void *))GetProcAddress(hInstLibXML, "xmlSchemaSetValidErrors"); assert(pXmlSchemaSetValidErrors);
  pXmlSchemaFreeValidCtxt =       (void (__cdecl *)(xmlSchemaValidCtxtPtr))GetProcAddress(hInstLibXML, "xmlSchemaFreeValidCtxt"); assert(pXmlSchemaFreeValidCtxt);
  pXmlSchemaValidateDoc =         (int (__cdecl *)(xmlSchemaValidCtxtPtr, xmlDocPtr))GetProcAddress(hInstLibXML, "xmlSchemaValidateDoc"); assert(pXmlSchemaValidateDoc);
  pXmlSchemaValidateSetLocator =  (void (__cdecl *)(xmlSchemaValidCtxtPtr, xmlSchemaValidityLocatorFunc, void *))GetProcAddress(hInstLibXML, "xmlSchemaValidateSetLocator"); assert(pXmlSchemaValidateSetLocator);
  //pXmlSchemaValidityLocatorFunc = (int (__cdecl *)(void *, const char **, unsigned long *))GetProcAddress(hInstLibXML, "xmlSchemaValidityLocatorFunc"); assert(pXmlSchemaValidityLocatorFunc);

  pXmlNewValidCtxt =              (xmlValidCtxtPtr (__cdecl *)(void))GetProcAddress(hInstLibXML, "xmlNewValidCtxt"); assert(pXmlNewValidCtxt);
  pXmlFreeValidCtxt =             (void (__cdecl *)(xmlValidCtxtPtr))GetProcAddress(hInstLibXML, "xmlFreeValidCtxt"); assert(pXmlFreeValidCtxt);

  pXmlParseDTD =                  (xmlDtdPtr (__cdecl *)(const xmlChar *, const xmlChar *))GetProcAddress(hInstLibXML, "xmlParseDTD"); assert(pXmlParseDTD);
  pXmlFreeDtd =                   (void (__cdecl *)(xmlDtdPtr))GetProcAddress(hInstLibXML, "xmlFreeDtd"); assert(pXmlFreeDtd);
  pXmlValidateDtd =               (int (__cdecl *)(xmlValidCtxtPtr, xmlDocPtr, xmlDtdPtr))GetProcAddress(hInstLibXML, "xmlValidateDtd"); assert(pXmlValidateDtd);

  pXmlXPathNewContext =           (xmlXPathContextPtr (__cdecl *)(xmlDocPtr))GetProcAddress(hInstLibXML, "xmlXPathNewContext"); assert(pXmlXPathNewContext);
  pXmlXPathFreeContext =          (void (__cdecl *)(xmlXPathContextPtr))GetProcAddress(hInstLibXML, "xmlXPathFreeContext"); assert(pXmlXPathFreeContext);
  pXmlXPathEvalExpression =       (xmlXPathObjectPtr (__cdecl *)(const xmlChar *, xmlXPathContextPtr))GetProcAddress(hInstLibXML, "xmlXPathEvalExpression"); assert(pXmlXPathEvalExpression);
  pXmlXPathFreeObject =           (void (__cdecl *)(xmlXPathObjectPtr))GetProcAddress(hInstLibXML, "xmlXPathFreeObject"); assert(pXmlXPathFreeObject);
  pXmlXPathRegisterNs =           (int (__cdecl *)(xmlXPathContextPtr, const xmlChar *, const xmlChar *))GetProcAddress(hInstLibXML, "xmlXPathRegisterNs"); assert(pXmlXPathRegisterNs);

  pXmlGetLastError =              (xmlErrorPtr (__cdecl *)(void))GetProcAddress(hInstLibXML, "xmlGetLastError"); assert(pXmlGetLastError);
  pXmlResetLastError =            (void (__cdecl *)(void))GetProcAddress(hInstLibXML, "xmlResetLastError"); assert(pXmlResetLastError);
  pXmlResetError =                (void (__cdecl *)(xmlErrorPtr))GetProcAddress(hInstLibXML, "xmlResetError"); assert(pXmlResetError);
  pXmlGetProp =                   (xmlChar * (__cdecl *)(xmlNodePtr, const xmlChar *))GetProcAddress(hInstLibXML, "xmlGetProp"); assert(pXmlGetProp);
  pXmlSubstituteEntitiesDefault = (int (__cdecl *)(int))GetProcAddress(hInstLibXML, "xmlSubstituteEntitiesDefault"); assert(pXmlSubstituteEntitiesDefault);
  pXmlStrdup =                    (xmlChar * (__cdecl *)(const xmlChar *))GetProcAddress(hInstLibXML, "xmlStrdup"); assert(pXmlStrdup);
  pXmlStrchr =                    (const xmlChar * (__cdecl *)(const xmlChar *, xmlChar))GetProcAddress(hInstLibXML, "xmlStrchr"); assert(pXmlStrchr);

  pXmlGetGlobalState =            (xmlGlobalStatePtr (__cdecl *)(void))GetProcAddress(hInstLibXML, "xmlGetGlobalState"); assert(pXmlGetGlobalState);
  pXmlKeepBlanksDefault =         (int (__cdecl *)(int))GetProcAddress(hInstLibXML, "xmlKeepBlanksDefault"); assert(pXmlKeepBlanksDefault);
  pXmlThrDefIndentTreeOutput =    (int (__cdecl *)(int))GetProcAddress(hInstLibXML, "xmlThrDefIndentTreeOutput"); assert(pXmlThrDefIndentTreeOutput);
  pXmlThrDefTreeIndentString =    (const char * (__cdecl *)(const char *))GetProcAddress(hInstLibXML, "xmlThrDefTreeIndentString"); assert(pXmlThrDefTreeIndentString);

  pXmlNanoHTTPInit =              (void (__cdecl *)(void)) GetProcAddress(hInstLibXML, "xmlNanoHTTPInit"); assert(pXmlNanoHTTPInit);
  pXmlNanoHTTPScanProxy =         (void (__cdecl *)(const char *)) GetProcAddress(hInstLibXML, "xmlNanoHTTPScanProxy"); assert(pXmlNanoHTTPScanProxy);
  pXmlNanoHTTPAuthHeader =        (const char * (__cdecl *)(void *)) GetProcAddress(hInstLibXML, "xmlNanoHTTPAuthHeader"); assert(pXmlNanoHTTPAuthHeader);

  //-----------------------------------------------------------------------------------------------
  
  /* xmlFree !!! pthreads? */
  
  pXsltParseStylesheetDoc =       (xsltStylesheetPtr (__cdecl *)(xmlDocPtr))GetProcAddress(hInstLibXSL, "xsltParseStylesheetDoc"); assert(pXsltParseStylesheetDoc);
  pXsltParseStylesheetFile =      (xsltStylesheetPtr (__cdecl *)(const xmlChar *))GetProcAddress(hInstLibXSL, "xsltParseStylesheetFile"); assert(pXsltParseStylesheetFile);
  pXsltSaveResultToString =       (int (__cdecl *)(xmlChar **, int *, xmlDocPtr, xsltStylesheetPtr))GetProcAddress(hInstLibXSL, "xsltSaveResultToString"); assert(pXsltSaveResultToString);
  pXsltFreeStylesheet =           (void (__cdecl *)(xsltStylesheetPtr))GetProcAddress(hInstLibXSL, "xsltFreeStylesheet"); assert(pXsltFreeStylesheet);
  pXsltApplyStylesheet =          (xmlDocPtr (__cdecl *)(xsltStylesheetPtr, xmlDocPtr, const char **))GetProcAddress(hInstLibXSL, "xsltApplyStylesheet"); assert(pXsltApplyStylesheet);
  pXsltCleanupGlobals =           (void (__cdecl *)(void))GetProcAddress(hInstLibXSL, "xsltCleanupGlobals"); assert(pXsltCleanupGlobals);
  pXsltSetGenericErrorFunc =      (void (__cdecl *)(void *, xmlGenericErrorFunc))GetProcAddress(hInstLibXSL, "xsltSetGenericErrorFunc"); assert(pXsltSetGenericErrorFunc);
  pXsltNewTransformContext =      (xsltTransformContextPtr (__cdecl *)(xsltStylesheetPtr, xmlDocPtr))GetProcAddress(hInstLibXSL, "xsltNewTransformContext"); assert(pXsltNewTransformContext);

  /* init */
  pXmlInitParser();

  return 0;
}

/*
xmlNodePtr pXmlRemoveNs(xmlNodePtr tree, xmlNsPtr ns) {
  xmlNsPtr nsDef,prev;
  xmlNodePtr node = tree;
  xmlNodePtr declNode = NULL;
  xmlAttrPtr attr;

  if (ns == NULL) {
    //pXmlGenericErrorFunc(xmlGenericErrorContext, "xmlRemoveNs : NULL namespace\n");
    return(NULL);
  }
  while (node != NULL) {
    //Check if the namespace is in use by the node
    if (node->ns == ns) {
      //pXmlGenericErrorFunc(xmlGenericErrorContext, "xmlRemoveNs : namespace in use\n");
      return(NULL);
    }

    // now check for namespace hold by attributes on the node.
    attr = node->properties;
    while (attr != NULL) {
      if (attr->ns == ns) {
        //pXmlGenericErrorFunc(xmlGenericErrorContext, "xmlRemoveNs : namespace in use\n");
        return(NULL);
      }
      attr = attr->next;
    }

    // Check if the namespace is declared in the node
    nsDef=node->nsDef;
    while(nsDef != NULL) {
      if (nsDef == ns) {
        declNode = node;
        break;
      }
      nsDef=nsDef->next;
    }

    // Browse the full subtree, deep first
    if (node->children != NULL) {
      // deep first
      node = node->children;
    } else if ((node != tree) && (node->next != NULL)) {
      // then siblings
      node = node->next;
    } else if (node != tree) {
      // go up to parents->next if needed
      while (node != tree) {
        if (node->parent != NULL)
          node = node->parent;
        if ((node != tree) && (node->next != NULL)) {
          node = node->next;
          break;
        }
        if (node->parent == NULL) {
          node = NULL;
          break;
        }
      }
      // exit condition
      if (node == tree)
        node = NULL;
    } else
      break;
  }

  // there is no such namespace declared here
  if (declNode == NULL) {
    //pXmlGenericErrorFunc(xmlGenericErrorContext, "xmlRemoveNs : no such namespace declared\n");
    return(NULL);
  }

  prev=NULL;
  nsDef=declNode->nsDef;
  while(nsDef != NULL) {
    if (nsDef == ns) {
      if (prev == NULL) declNode->nsDef=nsDef->next;
      else prev->next=nsDef->next;
      pXmlFreeNs(ns);
      break;
    }
    prev=nsDef;
    nsDef=nsDef->next;
  }

  return(declNode);
}
*/