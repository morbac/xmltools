#if !defined(__LOAD_XML_LIBRARY__)
#define __LOAD_XML_LIBRARY__

#include <libxml/xmlmemory.h>
#include <libxml/debugXML.h>
#include <libxml/HTMLtree.h>
#include <libxml/xmlIO.h>
#include <libxml/DOCBparser.h>
#include <libxml/xinclude.h>
#include <libxml/catalog.h>
#include <libxml/globals.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>
#include <libxml/xmlschemas.h>

#include <libxslt/xslt.h>
#include <libxslt/xsltInternals.h>
#include <libxslt/transform.h>
#include <libxslt/xsltutils.h>

extern xsltStylesheetPtr (*pXsltParseStylesheetDoc)(xmlDocPtr doc);
extern xmlDocPtr (*pXmlParseMemory)(const char * buffer, int size);
extern xmlDocPtr (*pXmlReadMemory)(const char *buffer, int size, const char *URL, const char *encoding, int options);

extern void (*pXmlFree)(void *mem);
extern void (*pXmlFreeDoc)(xmlDocPtr cur);
extern xmlDocPtr (*pXmlParseFile)(const char * filename);
extern int (*pXmlSaveFormatFile)(const char * filename, xmlDocPtr cur, int format);
extern void (*pXmlDocDumpFormatMemory)(xmlDocPtr cur, xmlChar ** mem, int * size, int format);
extern void (*pXmlInitParser)(void);
extern void (*pXmlCleanupParser)(void);
extern int (*pXmlSubstituteEntitiesDefault)(int val);

extern xmlNodePtr (*pXmlDocGetRootElement)(xmlDocPtr doc);

extern xmlErrorPtr (*pXmlGetLastError)(void);
extern void (*pXmlResetLastError)(void);
extern void (*pXmlResetError)(xmlErrorPtr);
extern xmlChar * (*pXmlGetProp)(xmlNodePtr node, const xmlChar *name);
extern xmlDtdPtr (*pXmlParseDTD)(const xmlChar *ExternalID, const xmlChar *SystemID);
extern xmlSchemaPtr (*pXmlSchemaParse)(xmlSchemaParserCtxtPtr ctxt);
extern xmlSchemaParserCtxtPtr (*pXmlSchemaNewParserCtxt)(const char *URL);
extern void (*pXmlSchemaFreeParserCtxt)(xmlSchemaParserCtxtPtr ctxt);
extern xmlSchemaValidCtxtPtr (*pXmlSchemaNewValidCtxt)(xmlSchemaPtr schema);
extern void (*pXmlSchemaFree)(xmlSchemaPtr schema);
extern void (*pXmlSchemaSetValidErrors)(xmlSchemaValidCtxtPtr ctxt, xmlSchemaValidityErrorFunc err, xmlSchemaValidityWarningFunc warn, void *ctx);
extern void (*pXmlSchemaFreeValidCtxt)(xmlSchemaValidCtxtPtr ctxt);
extern xmlValidCtxtPtr (*pXmlNewValidCtxt)(void);
extern void (*pXmlFreeValidCtxt)(xmlValidCtxtPtr);
extern void (*pXmlFreeDtd)(xmlDtdPtr cur);
extern xmlXPathContextPtr (*pXmlXPathNewContext)(xmlDocPtr doc);
extern void (*pXmlXPathFreeContext)(xmlXPathContextPtr ctxt);
extern xmlXPathObjectPtr (*pXmlXPathEvalExpression)(const xmlChar *str, xmlXPathContextPtr ctxt);
extern void (*pXmlXPathFreeObject)(xmlXPathObjectPtr obj);
extern xmlChar * (*pXmlStrdup)(const xmlChar *cur);
extern const xmlChar * (*pXmlStrchr)(const xmlChar *str, xmlChar val);
extern int (*pXmlXPathRegisterNs)(xmlXPathContextPtr ctxt, const xmlChar *prefix, const xmlChar *ns_uri);
extern int (*pXmlValidateDtd)(xmlValidCtxtPtr ctxt, xmlDocPtr doc, xmlDtdPtr dtd);
extern int (*pXmlSchemaValidateDoc)(xmlSchemaValidCtxtPtr ctxt, xmlDocPtr instance);
extern xmlGlobalStatePtr (*pXmlGetGlobalState)(void);
extern int (*pXmlKeepBlanksDefault)(int val);
extern int (*pXmlThrDefIndentTreeOutput)(int v);
extern const char * (*pXmlThrDefTreeIndentString)(const char * v);

//-------------------------------------------------------------------------------------------------

extern xsltStylesheetPtr (*pXsltParseStylesheetFile)(const xmlChar * filename);
extern int (*pXsltSaveResultToString)(xmlChar ** doc_txt_ptr, int * doc_txt_len, xmlDocPtr result, xsltStylesheetPtr style);
extern void (*pXsltFreeStylesheet)(xsltStylesheetPtr sheet);
extern xmlDocPtr (*pXsltApplyStylesheet)(xsltStylesheetPtr style, xmlDocPtr doc, const char ** params);
extern void (*pXsltCleanupGlobals)(void);
extern void (*pXsltSetGenericErrorFunc)(void * ctx, xmlGenericErrorFunc handler);
extern xsltTransformContextPtr(*pXsltNewTransformContext)(xsltStylesheetPtr style, xmlDocPtr doc);

int loadLibXML(wchar_t* nppMainPath, wchar_t* appDataPath);

#endif __LOAD_XML_LIBRARY__