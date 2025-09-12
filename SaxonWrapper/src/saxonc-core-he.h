#ifndef __SAXONC_CORE_HE_H
#define __SAXONC_CORE_HE_H

#include "graal_isolate.h"


#if defined(__cplusplus)
extern "C" {
#endif

long long int createDocumentBuilder(graal_isolatethread_t*, void *);

long long int j_db_setBaseURI(graal_isolatethread_t*, void *, char*);

long long int j_db_setLineNumbering(graal_isolatethread_t*, void *, int);

long long int j_db_setDTDValidation(graal_isolatethread_t*, void *, int);

long long int j_create_mapData(graal_isolatethread_t*);

long long int j_create_mapDataWithCapacity(graal_isolatethread_t*, int);

long long int j_addMapPair(graal_isolatethread_t*, void *, void *, void *);

long long int j_makeXdmMap(graal_isolatethread_t*, void *);

long long int createProcessorDataWithArray(graal_isolatethread_t*, void *);

long long int createProcessorData(graal_isolatethread_t*);

long long int createProcessorDataWithCapacity(graal_isolatethread_t*, int);

void addProcessorDataPair(graal_isolatethread_t*, void *, char*, void *);

void addProcessorPropertyPair(graal_isolatethread_t*, void *, char*, char*);

void addProcessorValue(graal_isolatethread_t*, void *, void *);

char* j_getProcessorDataProperty(graal_isolatethread_t*, void *, void *, int);

void addProcessorProperty(graal_isolatethread_t*, void *, char*);

void clearProcessorData(graal_isolatethread_t*, void *);

long long int createSaxonProcessor(graal_isolatethread_t*, int);

void j_gc(graal_isolatethread_t*);

char j_isSchemaAware(graal_isolatethread_t*, void *);

char j_isLicensed(graal_isolatethread_t*, void *);

char* j_c_getSaxonEdition(graal_isolatethread_t*, void *);

long long int j_run_transform(graal_isolatethread_t*, void *);

long long int j_run_gizmo(graal_isolatethread_t*, void *);

long long int j_run_query(graal_isolatethread_t*, void *);

long long int j_copy_handles(graal_isolatethread_t*, void *);

void j_handles_destroy(graal_isolatethread_t*, void *);

void j_printObjectHandlesPoolInfo(graal_isolatethread_t*);

void j_createHeapDump(graal_isolatethread_t*, int);

long long* j_getLongArray(graal_isolatethread_t*, void *, void *);

long long int j_createStringObject(graal_isolatethread_t*, char*);

long long int createSaxonProcessorWithConfigurationFile(graal_isolatethread_t*, char*);

char* j_c_getProductVersion(graal_isolatethread_t*, void *);

char* j_cpp_getProductVersion(graal_isolatethread_t*, void *, void *);

long long int j_setCatalogFiles(graal_isolatethread_t*, void *, char*, void *);

long long int j_applyPropertyToConfiguration(graal_isolatethread_t*, void *, char*, char*);

long long int j_applyPropertiesToConfiguration(graal_isolatethread_t*, void *, void *);

void setDebugMode(graal_isolatethread_t*, int);

long long int parseJsonFile(graal_isolatethread_t*, void *, char*, char*);

long long int parseJsonString(graal_isolatethread_t*, void *, char*, char*);

long long int parseXmlFile(graal_isolatethread_t*, void *, char*, char*);

long long int parseXmlFileWithValidator(graal_isolatethread_t*, char*, void *, void *, char*);

long long int parseXmlString(graal_isolatethread_t*, void *, char*, char*);

long long int parseXmlStringWithValidator(graal_isolatethread_t*, void *, char*, void *, char*);

long long int createXdmAtomicItem(graal_isolatethread_t*, char*, char*);

long long int j_main_parseXmlString(graal_isolatethread_t*, char*, void *, void *, void *, char*, char*);

char* getTypeName(graal_isolatethread_t*, char*);

char* j_getStringValue(graal_isolatethread_t*, void *);

char* j_cpp_getStringValue(graal_isolatethread_t*, void *, void *);

char* j_cpp_encodeString(graal_isolatethread_t*, char*, char*, void *);

long long int j_main_parseXmlFile(graal_isolatethread_t*, char*, void *, void *, void *, char*);

char* j_getErrorMessage(graal_isolatethread_t*, void *);

char* j_getErrorMessageWithErrorCode(graal_isolatethread_t*, void *);

char* j_getErrorMessageInJavaMem(graal_isolatethread_t*);

char* j_getCombinedStaticErrorMessages(graal_isolatethread_t*, void *);

int j_getLineNumber(graal_isolatethread_t*);

char* j_getErrorCode(graal_isolatethread_t*, void *);

char* j_getSystemId(graal_isolatethread_t*, void *);

void j_clearException(graal_isolatethread_t*);

char j_checkForException(graal_isolatethread_t*);

int j_staticErrorCount(graal_isolatethread_t*);

long long int createSaxonCMessageListener(graal_isolatethread_t*, void *, char*, char*);

long long* j_getXslMessages(graal_isolatethread_t*, void *, void *);

void j_clearXslMessages(graal_isolatethread_t*, void *);

long long int createSaxonCResultDocumentHandler(graal_isolatethread_t*, int);

long long* j_getResultDocuments(graal_isolatethread_t*, void *, void *);

long long int j_getResultDocumentURIs(graal_isolatethread_t*, void *);

long long int createSchemaValidator(graal_isolatethread_t*);

long long int createSchemaValidatorWithProcessor(graal_isolatethread_t*, void *);

long long int j_getSchemaManager(graal_isolatethread_t*, void *);

long long int j_getValidationReport(graal_isolatethread_t*, void *);

long long int j_registerSchema(graal_isolatethread_t*, void *, char*, char*, void *);

long long int j_registerSchemaString(graal_isolatethread_t*, void *, char*, char*, void *);

long long int j_registerSchemaNode(graal_isolatethread_t*, void *, void *, void *);

long long int j_exportSchema(graal_isolatethread_t*, void *, char*, char*);

long long int j_validate(graal_isolatethread_t*, void *, void *, char*, char*, char*, void *);

long long int j_validateToNode(graal_isolatethread_t*, void *, void *, char*, char*, void *);

long long int createXPathProcessor(graal_isolatethread_t*);

long long int createXPathProcessorWithProcessor(graal_isolatethread_t*, void *);

long long int j_xp_declareNamespace(graal_isolatethread_t*, void *, char*, char*);

long long int j_xp_declareVariable(graal_isolatethread_t*, void *, char*);

long long int j_xp_importSchemaNamespace(graal_isolatethread_t*, void *, char*);

void j_xp_setAllowUndeclaredVariables(graal_isolatethread_t*, void *, int);

void j_xp_setBackwardsCompatible(graal_isolatethread_t*, void *, int);

long long int j_xp_setBaseURI(graal_isolatethread_t*, void *, char*);

char* j_xp_getBaseURI(graal_isolatethread_t*, void *, void *);

long long int j_evaluate(graal_isolatethread_t*, void *, char*, char*, char*, void *);

long long int j_evaluateSingle(graal_isolatethread_t*, void *, char*, char*, char*, void *);

int j_effectiveBooleanValue(graal_isolatethread_t*, void *, char*, char*, char*, void *);

long long int j_createXQueryEngine(graal_isolatethread_t*);

long long int j_createXQueryEngineWithProcessor(graal_isolatethread_t*, void *);

char* j_executeQueryToString(graal_isolatethread_t*, void *, void *, char*, char*, char*, void *);

char* j_executeQueryToString_c(graal_isolatethread_t*, void *, char*, void *);

long long int j_executeQueryToValue(graal_isolatethread_t*, void *, char*, char*, char*, void *);

long long int j_executeQueryToFile(graal_isolatethread_t*, void *, char*, char*, char*, char*, void *);

char* getBaseURIForXdmNode(graal_isolatethread_t*, void *, void *);

long long int getParentForXdmNode(graal_isolatethread_t*, void *);

char* getStringValueForXdmValue(graal_isolatethread_t*, void *, void *, char*);

char* getStringValueForXdmItem(graal_isolatethread_t*, void *, void *, char*);

char* xdmItemToString(graal_isolatethread_t*, void *, void *, char*);

char* j_EQNameToClarkName(graal_isolatethread_t*, void *, char*);

char* j_clarkNameToEQName(graal_isolatethread_t*, void *, char*);

char* xdmNodeToString(graal_isolatethread_t*, void *, void *, char*);

char* xdmMapToString(graal_isolatethread_t*, void *, void *, char*);

char j_xdmNodeEquals(graal_isolatethread_t*, void *, void *);

char* j_getPrimitiveTypeName(graal_isolatethread_t*, void *, void *);

char* j_getFunctionName(graal_isolatethread_t*, void *, void *);

int j_xdmFunctionItem_getArity(graal_isolatethread_t*, void *);

long long int j_xdmFunctionItem_call(graal_isolatethread_t*, void *, void *, void *);

long long int j_getSystemFunction(graal_isolatethread_t*, void *, char*, int);

int j_xdmArray_arrayLength(graal_isolatethread_t*, void *);

int j_xdmMap_size(graal_isolatethread_t*, void *);

long long int j_xdmArray_get(graal_isolatethread_t*, void *, int);

long long int j_xdmArray_put(graal_isolatethread_t*, void *, int, void *);

long long int j_xdmMap_remove(graal_isolatethread_t*, void *, void *);

long long int j_xdmArray_addMember(graal_isolatethread_t*, void *, void *);

long long int j_xdmArray_concat(graal_isolatethread_t*, void *, void *);

char* j_xdmArray_toString(graal_isolatethread_t*, void *, void *);

long long* j_convertXdmArrayToArrayObject(graal_isolatethread_t*, void *, void *);

long long int j_makeXdmArray(graal_isolatethread_t*, void *);

long long int j_makeXdmArrayFromShort(graal_isolatethread_t*, short*, int);

long long int j_makeXdmArrayFromStrings(graal_isolatethread_t*, char**, int);

long long int j_makeXdmArrayFromInt(graal_isolatethread_t*, int*, int);

long long int j_makeXdmArrayFromLong(graal_isolatethread_t*, long long*, int);

long long int j_makeXdmArrayFromBool(graal_isolatethread_t*, int*, int);

long long* j_xdmMap_values(graal_isolatethread_t*, void *, void *);

char j_xdmMap_containsKey(graal_isolatethread_t*, void *, void *);

char j_xdmMap_isEmpty(graal_isolatethread_t*, void *);

long long int j_xdmMap_put(graal_isolatethread_t*, void *, void *, void *);

long long int j_xdmMap_get(graal_isolatethread_t*, void *, void *);

long long int j_xdmMap_get_with_key_as_string(graal_isolatethread_t*, void *, char*);

long long int j_xdmMap_get_with_key_as_int(graal_isolatethread_t*, void *, int);

long long int j_xdmMap_get_with_key_as_double(graal_isolatethread_t*, void *, double);

long long int j_xdmMap_get_with_key_as_long(graal_isolatethread_t*, void *, long long int);

long long* j_xdmMap_keys(graal_isolatethread_t*, void *, void *);

int j_getNodeKind(graal_isolatethread_t*, void *);

char* j_getLocalName(graal_isolatethread_t*, void *, void *);

char* j_getNodeName(graal_isolatethread_t*, void *, void *);

char* xdmValueArrayToString(graal_isolatethread_t*, void *, void *, char*);

char* j_getAttributeValue(graal_isolatethread_t*, void *, char*, void *);

long long* j_getAttributeNodes(graal_isolatethread_t*, void *, void *);

long long* j_getChildren(graal_isolatethread_t*, void *, void *);

int j_getXdmObjectType(graal_isolatethread_t*, void *);

long long int j_getChild(graal_isolatethread_t*, void *, int);

int j_getChildCount(graal_isolatethread_t*, void *);

int j_getAttributeCount(graal_isolatethread_t*, void *);

long long* j_axisIterator(graal_isolatethread_t*, void *, void *, int);

long long int j_makeStringValue(graal_isolatethread_t*, char*, char*);

long long int j_makeIntegerValue(graal_isolatethread_t*, int);

long long int j_makeDoubleValue(graal_isolatethread_t*, double);

long long int j_makeFloatValue(graal_isolatethread_t*, float);

long long int j_makeLongValue(graal_isolatethread_t*, long long int);

long long int j_makeBooleanValue(graal_isolatethread_t*, int);

long long int j_makeQNameValue(graal_isolatethread_t*, char*);

long long int j_makeAtomicValue(graal_isolatethread_t*, char*, char*);

long long* makeArrayFromXdmValue2(graal_isolatethread_t*, void *, void *);

long long int j_getTypedValue(graal_isolatethread_t*, void *);

int j_getLineNumber1(graal_isolatethread_t*, void *);

int j_getColumnNumber1(graal_isolatethread_t*, void *);

long long int j_getXdmEmptySequence(graal_isolatethread_t*);

char j_getBooleanValue(graal_isolatethread_t*, void *);

double j_getDoubleValue(graal_isolatethread_t*, void *);

long long int j_getLongValue(graal_isolatethread_t*, void *);

int j_getHashCode(graal_isolatethread_t*, void *);

long long int createXslt30Processor(graal_isolatethread_t*);

long long int createXslt30WithProcessor(graal_isolatethread_t*, void *);

long long int j_compileFromFileAndSave(graal_isolatethread_t*, void *, char*, char*, char*, void *);

long long int j_compileFromFileAndImport(graal_isolatethread_t*, void *, char*, char*, char*, void *);

long long int j_save(graal_isolatethread_t*, char*, void *, char*);

long long int j_compileFromStringAndSave(graal_isolatethread_t*, void *, char*, char*, char*, void *);

long long int j_compileFromXdmNodeAndSave(graal_isolatethread_t*, void *, void *, char*, char*, void *);

long long int j_compileFromAssociatedFile(graal_isolatethread_t*, void *, char*, char*, int, void *);

long long int j_compileFromFile(graal_isolatethread_t*, void *, char*, char*, int, void *);

long long int j_compileFromString(graal_isolatethread_t*, void *, char*, char*, int, void *);

long long int j_compileFromXdmNode(graal_isolatethread_t*, void *, char*, void *, int, void *);

long long int j_applyTemplatesReturningValue(graal_isolatethread_t*, char*, void *, void *, void *);

long long int j_applyTemplatesReturningFile(graal_isolatethread_t*, char*, void *, void *, char*, void *);

char* j_applyTemplatesReturningString(graal_isolatethread_t*, void *, char*, void *, void *, void *);

long long int j_callFunctionReturningFile(graal_isolatethread_t*, char*, void *, char*, char*, void *, void *);

long long int j_callFunctionReturningValue(graal_isolatethread_t*, char*, void *, char*, void *, void *);

char* j_callFunctionReturningString(graal_isolatethread_t*, void *, char*, void *, char*, void *, void *);

long long int j_callTemplateReturningFile(graal_isolatethread_t*, char*, void *, char*, char*, void *);

long long int j_callTemplateReturningValue(graal_isolatethread_t*, char*, void *, char*, void *);

char* j_callTemplateReturningString(graal_isolatethread_t*, void *, char*, void *, char*, void *);

long long int j_transformToFile(graal_isolatethread_t*, char*, void *, void *, char*, char*, char*, void *);

long long int j_transformToValue(graal_isolatethread_t*, char*, void *, void *, char*, char*, void *);

char* j_transformToString(graal_isolatethread_t*, void *, char*, void *, void *, char*, char*, void *);

long long int c_xsltSaveResultToFile(graal_isolatethread_t*, void *, char*, char*, char*, char*, void *);

char* c_xsltApplyStylesheet(graal_isolatethread_t*, void *, char*, char*, char*, void *);

#if defined(__cplusplus)
}
#endif
#endif
