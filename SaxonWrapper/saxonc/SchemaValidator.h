////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2022 - 2025 Saxonica Limited.
// This Source Code Form is subject to the terms of the Mozilla Public License,
// v. 2.0. If a copy of the MPL was not distributed with this file, You can
// obtain one at http://mozilla.org/MPL/2.0/. This Source Code Form is
// "Incompatible With Secondary Licenses", as defined by the Mozilla Public
// License, v. 2.0.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef SAXON_SCHEMA_H
#define SAXON_SCHEMA_H

#include "saxonc_export.h"
#include "saxonc/SaxonProcessor.h"

#include <string>

class SaxonProcessor;
class XdmValue;
class XdmNode;
class XdmItem;

/*! A <code>SchemaValidator</code> represents a factory for validating instance
 * documents against a schema.
 */
class SAXONC_EXPORT SchemaValidator {
public:
  /**
   * Default constructor.
   * Creates a Schema Validator
   * @throws SaxonApiException
   */
  SchemaValidator();

  /**
   * Constructor with the SaxonProcessor supplied.
   * The supplied processor should have license flag set to true for the Schema
   * Validator to operate.
   * @param proc - pointer to the SaxonProcessor object
   * @param cwd - the current working directory
   * @throws SaxonApiException
   */
  SchemaValidator(SaxonProcessor *proc, std::string cwd = "");

  /**
   * SchemaValidator copy constructor.
   * @param other - SchemaValidator
   */
  SchemaValidator(const SchemaValidator &other);

  /**
   * Destructor method: at the end of the program call the release() method to clear the JVM.
   */
  ~SchemaValidator();

  /**
   * The copy assignment= operator.
   * Create a copy of the SchemaValidator.
   * @param other - SchemaValidator object
   */
  SchemaValidator &operator=(const SchemaValidator &other);

  /**
   * Set the current working directory for the validator.
   * @param cwd  - Supplied working directory which replaces any set cwd. Ignore
   * if cwd is NULL.
   */
  void setcwd(const char *cwd);

 /**
  * Get the current working directory set on this validator.
  * Memory deallocation is handled internally.
  * @return Current working directory
  */
 const char * getcwd();

  /**
   * Register the schema from file name.
   *
   * The schema components derived from this schema document are added to
   * the cache of schema components maintained by this SchemaValidator.
   * @param xsd - File name of the schema relative to the cwd or full path if
   * cwd is null. The document may be either a schema document in source XSD
   * format, or a compiled schema in Saxon-defined SCM format (as produced using
   * the method exportSchema)
   * @throws SaxonApiException
   */
  void registerSchemaFromFile(const char *xsd);

  /**
   * Register the schema supplied as a string.
   * @param schemaStr - The schema document supplied as a string
   * @param systemID - The system ID of the document supplied as a string
   * @throws SaxonApiException
   */
  void registerSchemaFromString(const char *schemaStr,
                                const char *systemID = nullptr);

  /**
   * Register the schema supplied as an XDM Node Object
   * @param node - The schema document supplied as an XdmNode object
   * @throws SaxonApiException
   */
  void registerSchemaFromNode(XdmNode *node);

  /**
   * Export a precompiled Schema Component Model.
   * The Component model containing all the components (except built-in components) that have
   * been loaded by using the register methods.
   * @param fileName - The file name where the exported schema will be stored
   * @throws SaxonApiException
   */
  void exportSchema(const char *fileName);

  /**
   * Set the name of the output file that will be used by the validator.
   * @param outputFile the output file name for later use
   */
  void setOutputFile(const char *outputFile);

 /**
  * Get the output file that will be used by the validator.
  * @return const char * - string of the output file. Memory deallocation is handled internally.
  */
 const char *getOutputFile();

  /**
   * Validate an instance document by a registered schema.
   * @param sourceFile Name of the file to be validated. Allow nullptr when
   * source document is supplied with other method
   * @throws SaxonApiException
   */
  void validate(const char *sourceFile = nullptr);

  /**
   * Validate an instance document supplied as a Source object.
   * @param sourceFile The name of the file to be validated. Default is nullptr
   * @return XdmNode - the validated document returned to the calling program.
   * The caller is responsible for deallocation using `delete`.
   * @throws SaxonApiException
   */
  XdmNode *validateToNode(const char *sourceFile = nullptr);

  /**
   * Set the source node for validation.
   * @param source - The source document supplied as an XdmNode, which will be
   * used to validate against the schema using the validate methods.
   * The caller is responsible for deallocation of memory associated with the source node.
   */
  void setSourceNode(XdmNode *source);

  /**
   * Get the validation report.
   * The 'report-node' option must have been set to true in the properties
   * to use this feature: e.g. using setProperty("report-node", true) on the class object.
   * @return XdmNode - Pointer to XdmNode. Return nullptr if validation
   * reporting feature has not been enabled. The caller is responsible for memory deallocation.
   * @throws SaxonApiException
   */
  XdmNode *getValidationReport();

  /**
   * Set the value of a parameter used in the validator.
   * @param name - the name of the parameter, as a string. For a namespaced name use
   * clark notation i.e. "{uri}local"
   * @param value - the value of the parameter, or nullptr to clear a previously set
   * value
   */
  void setParameter(const char *name, XdmValue *value, bool withParam=true);

  /**
   * Remove a parameter (name, value) pair.
   * @param name - the name of the parameter
   * @return Outcome of the removal
   */
  bool removeParameter(const char *name);

  /**
   * Set a configuration property specific to the validator in use.
   *
   * @param name - the name of the property
   * @param value - the value of the property
   */
  void setProperty(const char *name, const char *value);

  /**
   * Clear parameter values set.
   * Default behaviour (false) is to leave XdmValues in memory.
   * Individual pointers to XdmValue objects have to be deleted in the calling
   * program.
   *
   *  @param deleteValues - if true then XdmValues are deleted
   */
  void clearParameters(bool deleteValues = false);

  /**
   * Clear configuration property values set.
   */
  void clearProperties();

  /**
   * Get the value of a parameter
   * @param name - the name of the parameter
   * @param withParam - internal use only
   * @return The value of the parameter as an XdmValue. Memory deallocation is handled internally.
   */
  XdmValue *getParameter(const char *name, bool withParam=true);

  /**
   * Get all parameters as a std::map.
   */
  std::map<std::string, XdmValue *> &getParameters();

  /**
   * Get all configuration properties specified on the processor as a std::map.
   */
  std::map<std::string, std::string> &getProperties();

  /**
   * Checks for pending exceptions without creating a local reference to the exception object.
   * @return True when there is a pending exception; otherwise false
   */
  bool exceptionOccurred();

  /**
   * Deprecated. Check for thrown exceptions.
   * @return The main exception message if any has been thrown otherwise
   * return nullptr
   * @deprecated From 12.1 this method is no longer used, since exceptions are now thrown.
   */
  const char *checkException();

  /**
   * Deprecated. Clear any thrown exceptions.
   * @deprecated From 12.1 this method is no longer used, since exceptions are now thrown.
   */
  void exceptionClear();


  /**
   * Deprecated. Get the error message if there are any validation errors.
   * A validation may have a number of errors reported against it.
   * @return The message of the exception
   * @deprecated From 12.1 this method is no longer used, since exceptions are now thrown.
   */
  const char *getErrorMessage();

  /**
   * Deprecated. Get error code if an error is reported.
   * Validation error are reported as exceptions. All errors can be retrieved.
   * @return char* - The error code of the exception.
   * @deprecated From 12.1 this method is no longer used, since exceptions are now thrown.
   */
  const char *getErrorCode();

  /**
   * Set the validation mode - which may be either strict or lax.
   * The default is strict; this method may be called
   * to indicate that lax validation is required. With strict validation,
   * validation fails if no element declaration can be located for the outermost
   * element. With lax validation, the absence of an element declaration results
   * in the content being considered valid.
   * @param l -  true if validation is to be lax, false if it is to be strict
   */
  void setLax(bool l);

  /**
   * Get the underlying Java object of the C++ schema validator.
   */
  int64_t getUnderlyingValidator() { return cppV; }

  /**
   * Get the SaxonProcessor which created this SchemaValidator.
   */
  SaxonProcessor* getSaxonProcessor() { return proc; }

private:
  bool lax; /*!< Flag to indicate lax mode for the Schema validation */
  SaxonProcessor
      *proc; /*!< Pointer to the SaxonProcessor object which created this SchemaValidator */
  // jclass  cppClass; /*!< JNI object where the SchemaValidator method will be
  // invoked */
  int64_t cppV;     /*!< The underlying SchemaValidator Java object */
  std::string cwdV; /*!< Current working directory */
  std::string
      outputFile; /*!< The output file name for the exported schema validator */
  std::map<std::string, XdmValue *>
      parameters; /*!< Map of parameters used for the transformation as (string,
                     value) pairs */
  std::map<std::string, std::string>
      properties; /*!< Map of properties used for the transformation as (string,
                     string) pairs */
  SaxonApiException *exception; /*!< Exceptions reported against the
                                   SchemaValidator are stored here */
};

#endif /* SAXON_SCHEMA_H */
