////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2022 - 2023 Saxonica Limited.
// This Source Code Form is subject to the terms of the Mozilla Public License,
// v. 2.0. If a copy of the MPL was not distributed with this file, You can
// obtain one at http://mozilla.org/MPL/2.0/. This Source Code Form is
// "Incompatible With Secondary Licenses", as defined by the Mozilla Public
// License, v. 2.0.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef SAXON_PROCESSOR_H
#define SAXON_PROCESSOR_H

#if defined __linux__ || defined __APPLE__

#include <dlfcn.h>
#include <stdlib.h>
#include <string>

#define HANDLE void *
#define LoadLibrary(x) dlopen(x, RTLD_LAZY)
#define GetProcAddress(x, y) dlsym(x, y)
#else
#include <windows.h>
#endif

//#define DEBUG //remove
//#define MEM_DEBUG //remove
#define CVERSION_API_NO 1210

#include <iostream>
#include <map>
#include <sstream>
#include <stdexcept> // std::logic_error
#include <string>
#include <vector>

#include "saxonc_export.h"
#include "saxonc/DocumentBuilder.h"
#include "saxonc/SaxonApiException.h"
#include "saxonc/SaxonCGlue.h"
#include "saxonc/SaxonCXPath.h"
#include "saxonc/SchemaValidator.h"
#include "saxonc/XPathProcessor.h"
#include "saxonc/XQueryProcessor.h"
#include "saxonc/Xslt30Processor.h"
#include "saxonc/XsltExecutable.h"


//#define MEM_DEBUG
#if defined MEM_DEBUG

#include <algorithm>
#include <cstdlib>
#include <new>

static std::vector<void *> myAlloc;

void *newImpl(std::size_t sz, char const *file, int line);

void *operator new(std::size_t sz, char const *file, int line);

void *operator new[](std::size_t sz, char const *file, int line);

void operator delete(void *ptr) noexcept;

void operator delete(void *, std::size_t) noexcept;

#endif

class Xslt30Processor;

class XQueryProcessor;

class XPathProcessor;

class SchemaValidator;

class XdmValue;

class XdmNode;

class XdmItem;

class XdmAtomicValue;

class XdmFunctionItem;

class XdmArray;

class XdmMap;

class XsltExecutable;

class DocumentBuilder;

class SaxonApiException;

typedef enum eXdmType {
  XDM_VALUE = 0,
  XDM_ATOMIC_VALUE = 1,
  XDM_NODE = 2,
  XDM_ARRAY = 3,
  XDM_MAP = 4,
  XDM_FUNCTION_ITEM = 5,
  XDM_EMPTY = 6,
  XDM_ITEM = 7,
} XDM_TYPE;

//==========================================

/**
 * The <code>SaxonProcessor</code> class acts as a factory for generating XQuery, XPath, Schema and XSLT compilers.
 * The <code>SaxonProcessor</code> class not only generates XQuery, XPath,
 * Schema and XSLT Processors, but is also used to create XDM values from primitive
 * types.
 */
class SAXONC_EXPORT SaxonProcessor {

  friend class DocumentBuilder;

  friend class Xslt30Processor;

  friend class XsltExecutable;

  friend class XQueryProcessor;

  friend class SchemaValidator;

  friend class XPathProcessor;

  friend class XdmValue;

  friend class XdmItem;

  friend class XdmAtomicValue;

  friend class XdmFunctionItem;

  friend class XdmNode;

  friend class XdmMap;

  friend class XdmArray;

public:
  /**
   * Default constructor.
   * Creates a Saxon processor.
   * @throws SaxonApiException
   */

  SaxonProcessor();

  /**
   * Constructor based upon a Saxon configuration file.
   * Creates a Saxon processor.
   * @throws SaxonApiException
   */

  SaxonProcessor(const char *configFile);

  /**
   * Constructor.
   * Creates a Saxon processor.
   * @param l - flag that a license is to be used. Default is false.
   * @throws SaxonApiException
   */
  SaxonProcessor(bool l);

  /**
   * The copy assignment= operator.
   * Creates a copy of the Saxon processor.
   * @param other - SaxonProcessor object
   */
  SaxonProcessor &operator=(const SaxonProcessor &other);

  /**
   * SaxonProcessor copy constructor.
   * @param other - SaxonProcessor
   */
  SaxonProcessor(const SaxonProcessor &other);

  /**
   * Destructor method: at the end of the program call the release() method to clear the JVM.
   */
  ~SaxonProcessor();

  /**
   * Deprecated. Get any error message thrown by the processor.
   * @return String value of the error message. JNI exceptions thrown by this
   * processor are handled internally and can be retrieved by this method.
   * @deprecated From 12.1 this method is no longer used, since exceptions are now thrown.
   */
  const char *getErrorMessage();

  /**
   * Create a DocumentBuilder.
   * A DocumentBuilder is used to load source XML documents.
   *
   * @return Pointer to a newly created DocumentBuilder. The caller is responsible for deallocation of memory using `delete`.
   */
  DocumentBuilder *newDocumentBuilder();

  /**
   * Create an Xslt30Processor.
   * An Xslt30Processor is used to compile XSLT 3.0 stylesheets.
   *
   * @return Pointer to a newly created Xslt30Processor. The caller is responsible for deallocation of memory using `delete`.
   */
  Xslt30Processor *newXslt30Processor();

  /**
   * Create an XQueryProcessor.
   * An XQueryProcessor is used to compile XQuery queries.
   *
   * @return Pointer to a newly created XQueryProcessor. The caller is responsible for deallocation of memory using `delete`.
   */
  XQueryProcessor *newXQueryProcessor();

  /**
   * Create an XPathProcessor.
   * An XPathProcessor is used to compile XPath expressions.
   *
   * @return Pointer to a newly created XPathProcessor. The caller is responsible for deallocation of memory using `delete`.
   */
  XPathProcessor *newXPathProcessor();

  /**
   * Create a SchemaValidator.
   * A SchemaValidator can be used to validate instance documents against the schema held by this SchemaValidator.
   *
   * @return Pointer to a new SchemaValidator. The caller is responsible for deallocation of memory using `delete`.
   */
  SchemaValidator *newSchemaValidator();


  void setExtensionLibrary(const char * libraryName);

  /**
   * Factory method to create an <code>xs:string</code> atomic value as a new XdmAtomicValue.
   * @param str - the <code>xs:string</code> value as a string. NULL is taken as equivalent to "".
   * @param encoding - the encoding of the string. If not specified then the
   * platform default encoding is used.
   * @return Pointer to a new XdmAtomicValue. The caller is responsible for deallocation of memory using `delete`.
   */
  XdmAtomicValue *makeStringValue(std::string str,
                                  const char *encoding = nullptr);

  /**
   * Factory method to create an <code>xs:string</code> atomic value as a new XdmAtomicValue.
   * @param str - the <code>xs:string</code> value as a char pointer array. nullptr is taken as equivalent to "".
   * @param encoding - the encoding of the string. If not specified then the
   * platform default encoding is used.
   * @return Pointer to a new XdmAtomicValue. The caller is responsible for deallocation of memory using `delete`.
   */
  XdmAtomicValue *makeStringValue(const char *str,
                                  const char *encoding = nullptr);

  /**
   * Factory method to create an <code>xs:integer</code> atomic value as a new XdmAtomicValue.
   * Internally represented by either a Java Int64Value or a BigIntegerValue depending
   * on the value supplied.
   *
   * @param i - the <code>xs:integer</code> value as an int
   * @return Pointer to a new XdmAtomicValue. The caller is responsible for deallocation of memory using `delete`.
   */
  XdmAtomicValue *makeIntegerValue(int i);

  /**
   * Factory method to create an <code>xs:double</code> atomic value as a new XdmAtomicValue.
   * @param d - the <code>xs:double</code> value as a double
   * @return Pointer to a new XdmAtomicValue. The caller is responsible for deallocation of memory using `delete`.
   */
  XdmAtomicValue *makeDoubleValue(double d);

  /**
   * Factory method to create an <code>xs:float</code> atomic value as a new XdmAtomicValue.
   * @param f - the <code>xs:float</code> value as a float
   * @return Pointer to a new XdmAtomicValue. The caller is responsible for deallocation of memory using `delete`.
   */
  XdmAtomicValue *makeFloatValue(float f);

  /**
   * Factory method to create an <code>xs:long</code> atomic value as a new XdmAtomicValue.
   * Internally represented by either a Java Int64Value or a BigIntegerValue depending
   * on the value supplied.
   *
   * @param l - the <code>xs:integer</code> value as a long
   * @return Pointer to a new XdmAtomicValue. The caller is responsible for deallocation of memory using `delete`.
   */
  XdmAtomicValue *makeLongValue(long l);

  /**
   * Factory method to create an <code>xs:boolean</code> atomic value as a new XdmAtomicValue.
   * @param b - the <code>xs:boolean</code> value as a boolean, true or false
   * @return Pointer to a new XdmAtomicValue. The caller is responsible for deallocation of memory using `delete`.
   */
  XdmAtomicValue *makeBooleanValue(bool b);

  /**
   * Create an <code>xs:QName</code> atomic value, from the string representation in clark notation.
   * @param str - the QName value given in a string form in clark notation: {uri}local
   * @return Pointer to a new XdmAtomicValue. The caller is responsible for deallocation of memory using `delete`.
   */
  XdmAtomicValue *makeQNameValue(const char *str);

  /**
   * Create an XDM atomic value from its lexical representation and the name of the required built-in atomic type.
   * @param type - the local name of a type in the XML Schema namespace.
   * @param value - the atomic value given in a string form.
   * In the case of a QName the value supplied must be in clark notation: {uri}local
   * @return Pointer to a new XdmAtomicValue. The caller is responsible for deallocation of memory using `delete`.
   */
  XdmAtomicValue *makeAtomicValue(const char *type, const char *value);

  /**
   * Make an XdmArray whose members are xs:string values.
   * @param input - the input array of strings
   * @param length - the number of items in the array
   * @return Pointer to an XdmArray whose members are xs:string values corresponding
   * one-to-one with the input. The caller is responsible for deallocation of memory using `delete`.
   */
  XdmArray *makeArray(char **input, int length);

  /**
   * Make an XdmArray whose members are xs:short values.
   * @param input  - the input array of short values
   * @param length - the number of items in the array
   * @return Pointer to an XdmArray whose members are xs:short values corresponding
   * one-to-one with the input. The caller is responsible for deallocation of memory using `delete`.
   */
  XdmArray *makeArray(short *input, int length);

  /**
   * Make an XdmArray whose members are xs:integer values.
   * @param input  - the input array of int values
   * @param length - the number of items in the array
   * @return Pointer to an XdmArray whose members are xs:integer values corresponding
   * one-to-one with the input. The caller is responsible for deallocation of memory using `delete`.
   */
  XdmArray *makeArray(int *input, int length);

  /**
   * Make an XdmArray whose members are xs:long values.
   * @param input  - the input array of long values
   * @param length - the number of items in the array
   * @return Pointer to an XdmArray whose members are xs:boolean values corresponding
   * one-to-one with the input. The caller is responsible for deallocation of memory using `delete`.
   */
  XdmArray *makeArray(long long *input, int length);

  /**
   * Make an XdmArray whose members are xs:boolean values.
   * @param input  - the input array of boolean values
   * @param length - the number of items in the array
   * @return Pointer to an XdmArray whose members are xs:boolean values corresponding
   * one-to-one with the input. The caller is responsible for deallocation of memory using `delete`.
   */
  XdmArray *makeArray(bool *input, int length);

  /**
   * Make an XdmArray by supplying an array of XdmValue pointers.
   * @param values - the input array of XdmValue pointers
   * @param length - the number of items in the array
   * @return Pointer to an XdmArray whose members are XdmValue values corresponding
   * one-to-one with the input. The caller is responsible for deallocation of memory using `delete`.
   */
  XdmArray *makeArray(XdmValue **values, int length);


  static void * makeInternalArray(void **inputs, int length);

  /**
   * Make an XdmMap by supplying a map from the standard template library, with keys in XDM form.
   * @param dataMap - the input map as an std::map, which consists of keys as XdmAtomicValue and
   * values as XdmValue.
   * @return Pointer to an XdmMap whose members are key, value pairs given as
   * (XdmAtomicValue, XdmValue). The caller is responsible for deallocation of memory using `delete`.
   */
  XdmMap *makeMap(std::map<XdmAtomicValue *, XdmValue *> dataMap);

  /**
   * Make an XdmMap by supplying a map from the standard template library, with keys in primitive form.
   * @param dataMap - the input map as an std::map, which consists of keys as std::string and
   * values as XdmValue. Keys are converted to XdmAtomicValue objects
   * @return Pointer to an XdmMap whose members are key, value pairs given as
   * (XdmAtomicValue, XdmValue). The caller is responsible for deallocation of memory using `delete`.
   */
  static XdmMap *makeMap2(std::map<std::string, XdmValue *> dataMap);

  /**
   * Make an XdmMap from arrays of keys and values in XDM form.
   * @param keys - the keys are given as a pointer array of XdmAtomicValue
   * @param values - the values are given as a pointer array of XdmValue
   * @param len - the number of items in the arrays
   * @return Pointer to an XdmMap whose members are key, value pairs given as
   * (XdmAtomicValue, XdmValue). The caller is responsible for deallocation of memory using `delete`.
   */
  XdmMap *makeMap3(XdmAtomicValue **keys, XdmValue **values, int len);

  /**
   * Convert a string representing a QName value in clark notation to a string representing the QName in EQName notation.
   * Returns the expanded name, as a string using the notation defined by the EQName
   * production in XPath 3.0. If the name is in a namespace, the resulting
   * string takes the form <code>Q{uri}local</code>. Otherwise, the value is the
   * local part of the name.
   * @param name - the QName in Clark notation: <code>{uri}local</code> if the
   *                name is in a namespace, or simply <code>local</code> if not.
   * @return The expanded name, as a string in EQName notation.
   * The caller is responsible for memory deallocation using `operator delete`.
   */
  const char *clarkNameToEQName(const char *name);

  /**
   * Convert a string representing a QName value in EQName notation to a string representing the QName in clark notation.
   * @param name - the QName in EQName notation: <code>Q{uri}local</code> if the
   *               name is in a namespace. For a name in no namespace, either of
   * the forms <code>Q{}local</code> or simply <code>local</code> are accepted.
   * @return The QName in clark notation. The caller is responsible for deallocation of memory using `operator delete`.
   */
  const char *EQNameToClarkName(const char *name);

  /**
   * Get the string representation of an XdmItem.
   * @param item - the XdmItem
   * @return String value as char pointer array. The caller is responsible for deallocation of memory using `operator delete`.
   */
  const char *getStringValue(XdmItem *item);

  /**
   * Parse a lexical representation of a source XML document and return it as an XdmNode.
   * @param source - the source document as a lexical string
   * @param encoding - the encoding used to decode the source string. If not
   * specified then platform default encoding is used.
   * @param validator - can be used to supply a SchemaValidator to validate the
   * document. Default is null.
   * @return Pointer to an XdmNode. The caller is responsible for deallocation of memory using `delete`.
   * @throws SaxonApiException if there is a failure in the parsing of the XML
   * document
   */
  XdmNode *parseXmlFromString(const char *source,
                              const char *encoding = nullptr,
                              SchemaValidator *validator = nullptr);

  /**
   * Parse a source document file and return it as an XdmNode.
   * @param source - the filename of the source document
   * @param validator - can be used to supply a SchemaValidator to validate the
   * document. Default is null.
   * @return Pointer to an XdmNode. The caller is responsible for deallocation of memory using `delete`.
   * @throws SaxonApiException  if there ia a failure in the parsing of the XML
   * file
   */
  XdmNode *parseXmlFromFile(const char *source,
                            SchemaValidator *validator = nullptr);

  /**
   * Parse a source document available by URI and return it as an XdmNode.
   * @param source - the URI of the source document
   * @param validator - can be used to supply a SchemaValidator to validate the
   * document. Default is null.
   * @return Pointer to an XdmNode. The caller is responsible for deallocation of memory using `delete`.
   * @throws SaxonApiException if there ia failure in the parsing of the XML
   * file
   */
  XdmNode *parseXmlFromUri(const char *source,
                           SchemaValidator *validator = nullptr);

  /**
   * Parse a lexical representation of a source JSON document and return it as an XdmValue.
   * @param source - the JSON document as a lexical string
   * @param encoding - the encoding of the source argument. Argument can be
   * omitted and NULL accepted to use the default platform encoding.
   * @return Pointer to an XdmValue. The caller is responsible for deallocation of memory using `delete`.
   * @throws SaxonApiException if there is a failure in the parsing of the JSON
   */
  XdmValue *parseJsonFromString(const char *source,
                                const char *encoding = NULL);

  /**
   * Parse a source JSON file and return it as an XdmValue.
   * @param source - the filename of the JSON document. This is a full path filename or a
   * URI
   * @return Pointer to an XdmValue. The caller is responsible for deallocation of memory using `delete`.
   * @throws SaxonApiException if there is a failure in the parsing of the JSON
   * file
   */
  XdmValue *parseJsonFromFile(const char *source);

  /**
   * Get the kind of node - internal use only.
   * @param obj - the Java object representation of the XdmNode
   * @return The kind of node, for example ELEMENT or ATTRIBUTE as an integer
   */
  int getNodeKind(int64_t obj);

  /**
   * Test whether this processor is schema-aware.
   * @return True if this processor is licensed for schema processing,
   * false otherwise
   */
  bool isSchemaAwareProcessor();

  /**
   * Deprecated. Checks for thrown exceptions.
   * @deprecated From 12.1 this method is no longer used, since exceptions are now thrown.
   * @return True when there is a pending exception; false otherwise
   */
  bool exceptionOccurred();

 /**
  * Deprecated. Clears any exception that is currently being thrown.
  * If no exception is currently being thrown, this routine has no effect.
  *
  * @deprecated From 12.1 this method is no longer used, since exceptions are now thrown.
  */
  void exceptionClear();

  /**
   * Utility method for working with SaxonC on Python - internal use only.
   */
  XdmValue **createXdmValueArray(int len) { return (new XdmValue *[len]); }

  /**
   * Utility method for Python API - internal use only.
   * This method deletes a XdmValue pointer array.
   * @param arr - XdmValue pointer array
   * @param len - length of the array
   */
  void deleteXdmValueArray(XdmValue **arr, int len);

  /**
   * Utility method for working with SaxonC on Python - internal use only.
   */
  XdmAtomicValue **createXdmAtomicValueArray(int len) {
    return (new XdmAtomicValue *[len]);
  }

  /**
   * Utility method for Python API - internal use only.
   * This method deletes a XdmAtomicValue pointer array.
   * @param arr - XdmAtomicValue pointer array
   * @param len - length of the array
   */
  void deleteXdmAtomicValueArray(XdmAtomicValue **arr, int len);

 /**
  * Deprecated. Checks for pending exceptions and creates a SaxonApiException object.
  * @deprecated From 12.1 this method is no longer used, since exceptions are now thrown.
  */
  static SaxonApiException *checkForExceptionCPP(
      /*JNIEnv *env, jclass callingClass, jobject callingObject*/);


 /**
 *
*/
 static  const char * encodeString(const char * stringValue, const char * toCharSetName);

  /**
   * Clean up and destroy Java VM to release memory used; method to be called at the end of the program
   */
  static void release();

  /**
   * Attaches a current thread to the Java VM
   */
  static void attachCurrentThread();

  /**
   * Detach JVM from the current thread
   */
  static void detachCurrentThread();

  /**
   * Set the current working directory
   * @param cwd - current working directory
   */
  void setcwd(const char *cwd);

  /**
   * Get the current working directory
   * @return Current working directory
   */
  const char *getcwd();

  /**
   * Get the saxon resources directory
   * @return String value of the resources directory. The caller is responsible for deallocation of memory using `delete`.
   */
  const char *getResourcesDirectory();

  /**
   * Set the saxon resources directory
   * @param dir - resources directory
   */
  void setResourcesDirectory(const char *dir);

  /**
   * Deprecated. Set a catalog file to be used in Saxon.
   * @param catalogFile - file name to the catalog
   * @throws SaxonApiException if there is a failure to set the catalog file
   * @deprecated - Use the setCatalogFiles() method
   */
  void setCatalog(const char *catalogFile);

  /**
   * Set catalog files to be used in Saxon
   * @param catalogFiles - array of the catalog file names
   * @param length - number of catalog files in the array argument
   * @throws SaxonApiException if there is a failure to set the catalog files
   */
  void setCatalogFiles(const char **catalogFiles, int length);

  /**
   * Set a configuration property specific to the processor in use.
   * Properties specified here are common across all the processors.
   * Example 'l':enable line number has the value 'on' or 'off'
   *
   * @param name - the name of the property
   * @param value - the value for the property
   */
  void setConfigurationProperty(const char *name, const char *value);

  /**
   * Clear configuration properties specific to the processor in use.
   */
  void clearConfigurationProperties();

 /**
  * Test whether a license key has been found and accepted.
  * @return True if this processor is licensed, false otherwise
  */
 bool isLicensed();

 /**
 * Get the short name of the licensed Saxon product edition, for example "EE". This represents the kind of
 * configuration that has been created, rather than the software that has been installed; which depends on
 * the license key supplied, as well as the software edition installed. For example it is possible to
 * instantiate an "HE" configuration even when using the "PE" or "EE" software.
 * @return The Saxon edition code: "ee", "pe", or "he". Memory deallocation is handled internally.
 */
 const char * getSaxonEdition();

  /**
   * Get the Saxon version
   * @return The Saxon version. Memory deallocation is handled internally.
   */
  const char *version();


  static int jvmCreatedCPP; /*!< Flag to indicate JVM created - used in memory
                               management */
  static sxnc_environment
      *sxn_environ; /*!< Environment to capture the JNI, JVM and handler to the
                       cross compiled SaxonC library. */
  std::string cwd;  /*!< Current working directory */

  /**
  * Utility method required for the python and PHP extensions to delete a string created in the C++ code-base
  * @param data - the string data to be deleted using the operator delete
  */
  static void deleteString(const char *data) {
    if (data != nullptr) {
      operator delete((char *)data);
      data = nullptr;
    } 
  }

  /**
   * Internal method for diagnostics
   */
  void createHeapDump(bool live);


  /*static JavaVM *jvm;*/

protected:
  // jclass xdmAtomicClass; /*!< The XdmAtomicValue instance */
  // jclass versionClass; /*!< The Version instance */
  // jclass procClass; /*!< The Saxon Processor instance */
  // jclass saxonCAPIClass; /*!< The SaxonCAPI instance */
  std::string cwdV; /*!< Current working directory */
  // std::string resources_dir; /*!< Saxon resources directory */
  std::string versionStr; /*!< The Saxon version string */
  std::map<std::string, XdmValue *>
      parameters; /*!< Map of parameters used for the transformation as (string,
                     value) pairs */
  //std::map<std::string, std::string>
  // configProperties; /*!< Map of properties used for the transformation as
  //                         (string, string) pairs */
  bool licensei; /*!< If true, this indicates that the Processor created
                    needs a license file (i.e. Saxon-EE), otherwise a Saxon-HE
                    Processor is created  */
  int64_t procRef; /*!< ObjectHandle reference to the underlying processor */

  // JNINativeMethod *nativeMethods; /*!< native methods variable used in
  // extension methods */ std::vector<JNINativeMethod> nativeMethodVect; /*!<
  // Vector of native methods defined by user */
  SaxonApiException
      *exception; /*!< SaxonApiException object to capture exceptions thrown
                     from the underlying Java code via JNI */

private:
  // void createException(const char *message = nullptr);

  void initialize(bool l);

  void applyConfigurationProperties();

  // SaxonC method for internal use
  static int64_t
  createParameterJArray(std::map<std::string, XdmValue *> parameters,
                        std::map<std::string, std::string> properties,
                        int additions = 0);

  static int64_t
  createParameterJArray2(std::map<std::string, XdmValue *> parameters);

  static int64_t createJArray(XdmValue **values, int length);

  static XdmValue *makeXdmValueFromRef(int64_t valueRef);

  static XdmItem *makeXdmItemFromRef(int64_t valueRef);

  static const char *checkException();

 /**
 * Remove ObjectHandle from the pool. Free for the GC.
 */
  static void destroyHandle(int64_t handleRef);

   /**
   * Get info on objects allocated and still in memory
   */
  static void getInfo();
};

//===============================================================================================

#endif /* SAXON_PROCESSOR_H */
