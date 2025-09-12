////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2022 - 2025 Saxonica Limited.
// This Source Code Form is subject to the terms of the Mozilla Public License,
// v. 2.0. If a copy of the MPL was not distributed with this file, You can
// obtain one at http://mozilla.org/MPL/2.0/. This Source Code Form is
// "Incompatible With Secondary Licenses", as defined by the Mozilla Public
// License, v. 2.0.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef SAXON_XQUERY_H
#define SAXON_XQUERY_H

#include "saxonc_export.h"
#include "saxonc/SaxonProcessor.h"
#include <string>

class SaxonProcessor;
class SaxonApiException;
class XdmValue;
class XdmItem;

/** An <code>XQueryProcessor</code> represents a factory to compile, load and
 * execute queries.
 */
class SAXONC_EXPORT XQueryProcessor {
public:
  /**
  * Default constructor.
  * Creates a Saxon-HE XQuery processor
  */
  XQueryProcessor();

  /**
   * Constructor with the SaxonProcessor supplied.
   * @param p - pointer to the SaxonProcessor object
   * @param cwd - the current working directory. Default is the empty string
   */
  XQueryProcessor(SaxonProcessor *p, std::string cwd = "");

  /**
   * XQueryProcessor copy constructor.
   * @param other - XQueryProcessor
   */
  XQueryProcessor(const XQueryProcessor &other);

  /**
   * Create a clone of this XQueryProcessor object with the same internal state,
   * which can be used in separate threads.
   * @return A new copy of this XQueryProcessor object
   */
  XQueryProcessor *clone();

  ~XQueryProcessor();

  /**
   * Set the initial context item for the query as an XdmItem
   * @param value - the initial context item, or nullptr if there is to be no
   * initial context item
   */
  void setContextItem(XdmItem *value);

  /**
   * Set the output file where the query result is sent
   * @param outfile - the name of the file where results will be stored
   */
  void setOutputFile(const char *outfile);

  /**
   * Set the context item for the query as a source document
   * @param filename - the name of the source document
   */
  void setContextItemFromFile(const char *filename);

  /**
   * Set the value of a parameter used in the query
   *
   * @param name - the name of the parameter, as a string. For a namespaced name use
   * clark notation i.e. "{uri}local"
   * @param value - value of the query parameter, or nullptr to clear a previously set
   * value
   * @param withParam - internal use only
   */
  void setParameter(const char *name, XdmValue *value, bool withParam=true);

  /**
   * Set the XQuery language version for the XQuery compiler
   * @param version - "3.1" or "4.0" in the current Saxon release.
   */
  void setLanguageVersion(const char *version);

  /**
   * Say whether the query should be compiled and evaluated to use streaming.
   * This affects subsequent calls on the compile() methods. This option
   * requires Saxon-EE.
   * @param option if true, the compiler will attempt to compile a query to be
   * capable of executing in streaming mode. If the query cannot be streamed, a
   * compile-time exception is reported. In streaming mode, the source document
   * is supplied as a stream, and no tree is built in memory. The default is
   * false. When setStreaming(true) is specified, this has the additional
   * side effect of setting the required context item type to "document-node()"
   */
  void setStreaming(bool option);

  /**
   * Ask whether the streaming option has been set. That is, whether
   * subsequent calls on compile() will compile queries to be capable
   * of executing in streaming mode.
   *
   * @return True if the streaming option has been set.
   */
  bool isStreaming();

  /**
   * Remove a parameter (name, value) pair
   * @param name - the name of the parameter
   * @return Outcome of the removal
   */
  bool removeParameter(const char *name);

  /**
   * Set a configuration property specific to the XQuery processor in use.
   * XQueryProcessor: set serialization properties (names start with '!' i.e.
   * name "!method" -> "xml"), 'o':outfile name, 's': source as file name 'q':
   * query file name, 'q': current by name, 'qs': string form of the query,
   * 'base': set the base URI of the query, 'dtd': set DTD validation 'on' or
   * 'off'
   * @param name - the name of the property
   * @param value - the value of the property
   */
  void setProperty(const char *name, const char *value);

   /**
  * Get the value of a configuration property specified on the XQuery processor in use.
  * @param name - the name of the property
  * @return Value of the property or nullptr if the property is not found. Memory deallocation is handled internally.
  */
  const char * getProperty(const char *name);

  /**
   * Clear parameter values set.
   * Default behaviour (false) is to leave XdmValues in memory.
   * Individual pointers to XdmValue objects have to be deleted in the calling
   * program.
   * @param deleteValues - if true then XdmValues are deleted
   */
  void clearParameters(bool deleteValues = false);

  /**
   * Clear configuration property values set
   */
  void clearProperties();

  /**
   * Say whether the query is allowed to be updating.
   * XQuery update syntax will be rejected
   * during query compilation unless this flag is set. XQuery Update is
   * supported only under Saxon-EE.
   * @param updating - true if the query is allowed to use the XQuery Update
   * facility (requires Saxon-EE). If set to false, the query must not be an
   * updating query. If set to true, it may be either an updating or a
   * non-updating query.
   */
  void setUpdating(bool updating);

  /**
   * Execute a query, and save the result to file.
   * @param infilename - the file name of the source document
   * @param ofilename - the file name where results will be stored
   * @param query - the query supplied as a string
   * @param encoding - the encoding of the query string. If not specified then
   * the platform default encoding is used.
   * @throws SaxonApiException
   */
  void executeQueryToFile(const char *infilename, const char *ofilename,
                          const char *query, const char *encoding = nullptr);

  /**
   * Execute a query, returning the result as an XdmValue.
   * @param infilename - the file name of the source document
   * @param query - the query supplied as a string
   * @param encoding - the encoding of the query string. If not specified then
   * the platform default encoding is used.
   * @return The result of the query as an XdmValue. The caller is responsible for memory deallocation.
   * @throws SaxonApiException
   */
  XdmValue *executeQueryToValue(const char *infilename, const char *query,
                                const char *encoding = nullptr);

  /**
   * Execute a query, returning the result as a string.
   * @param infilename - the file name of the source document
   * @param query - the query supplied as a string
   * @param encoding - the encoding of the query string. If not specified then
   * the platform default encoding is used.
   * @return The result of the query serialized to a string. A zero length string is returned if
   * the query result is an empty sequence.
   * The caller is responsible for memory deallocation using `operator delete`.
   * @throws SaxonApiException
   */
  const char *executeQueryToString(const char *infilename, const char *query,
                                   const char *encoding = nullptr);

  /**
   * Execute a query as already configured for this XQueryProcessor, returning the result as an XdmValue.
   * All configuration properties must set in advance - for instance supplying the query and the context item.
   * @return The result of the query as an XdmValue. The caller is responsible for memory deallocation.
   * @throws SaxonApiException
   *
   */
  XdmValue *runQueryToValue();

  /**
   * Execute a query as already configured for this XQueryProcessor, returning the result as a string.
   * All configuration properties must set in advance - for instance supplying the query and the context item.
   * @return The result of the query serialized to a string. A zero length string is returned if
   * the query result is an empty sequence.
   * The caller is responsible for memory deallocation using `operator delete`.
   * @throws SaxonApiException
   *
   */
  const char *runQueryToString();

  /**
   * Execute a query as already configured for this XQueryProcessor, and save the result to file.
   * All configuration properties must set in advance - for instance supplying the query and the context item,
   * and the file name for the result output.
   * @throws SaxonApiException
   *
   */
  void runQueryToFile();

  /**
   * Declare a namespace binding as part of the static context for queries
   * compiled using this XQueryCompiler. This binding may be overridden by a
   * binding that appears in the query prolog. The namespace binding will form
   * part of the static context of the query, but it will not be copied into
   * result trees unless the prefix is actually used in an element or attribute
   * name.
   *
   * @param prefix - the namespace prefix. If the value is a zero-length string,
   * this method sets the default namespace for elements and types.
   * @param uri - the namespace URI. It is possible to specify a zero-length
   * string to "undeclare" a namespace; in this case the prefix will not be
   * available for use, except in the case where the prefix is also a zero
   * length string, in which case the absence of a prefix implies that the name
   * is in no namespace.
   * @throws SaxonApiException if either the prefix or uri is nullptr
   */
  void declareNamespace(const char *prefix, const char *uri);

  /**
   * Get the value of a parameter
   * @param name - the name of the parameter
   * @param withParam - internal use only
   * @return The value of the parameter as an XdmValue
   */
  XdmValue *getParameter(const char *name, bool withParam=true);

  /**
   * Get all parameters as a std::map
   * @return An std:map with key as string name mapped to XdmValue.
   */
  std::map<std::string, XdmValue *> &getParameters();

  /**
   * Get all configuration properties specified on the processor as a std::map
   * @return Map of (string, string) pairs
   */
  std::map<std::string, std::string> &getProperties();

  /**
   * Compile a query file.
   * The supplied query is cached for later execution.
   * @param filename - the file name of the query document
   */
  void setQueryFile(const char *filename);

  /**
   * Compile a query supplied as a string.
   * The supplied query is cached for later execution.
   * @param content - the query supplied as a character string
   */
  void setQueryContent(const char *content);

  /**
   * Set the static base URI for the query
   * @param baseURI - the static base URI; or nullptr to indicate that no base URI
   * is available
   */
  void setQueryBaseURI(const char *baseURI);

    /**
   * Get the static base URI for the query
   * @return  the static base URI
   */
  const char * getQueryBaseURI();

  /**
   * Set the current working directory
   * @param cwd - current working directory
   */
  void setcwd(const char *cwd);

 /**
  * Get the current working directory set on this XQueryProcessor.
  * Memory deallocation is handled internally.
  * @return Current working directory
  */
 const char * getcwd();

  /**
   * Check for thrown exceptions.
   * @return The main exception message if any has been thrown otherwise
   * return nullptr
   */
  const char *checkException();

  /**
   * Deprecated. Checks for pending exceptions without creating a local reference to the
   * exception object
   * @return True when there is a pending exception; otherwise false
   * @deprecated From 12.1 this method is no longer used, since exceptions are now thrown.
   */
  bool exceptionOccurred();

  /**
   * Deprecated. Clear any thrown exceptions
   * @deprecated From 12.1 this method is no longer used, since exceptions are now thrown.
   */
  void exceptionClear();

  /**
   * Deprecated. Get the first error message if there are any errors.
   * A query may have a number of errors reported against it.
   * @return The message of the exception
   * @deprecated From 12.1 this method is no longer used, since exceptions are now thrown.
   */
  const char *getErrorMessage();

  /**
   * Deprecated. Get the first error code if there are any errors.
   * After the execution of the query there may be a number of errors reported
   * against it.
   * @return The error code of the exception
   * @deprecated From 12.1 this method is no longer used, since exceptions are now thrown.
   */
  const char *getErrorCode();

  /**
   * Deprecated. Get the SaxonApiException object created when we have an error.
   * After the execution of the query if there is an error then a
   * SaxonApiException is created.
   * @return The SaxonApiException object if there is an exception thrown, or nullptr otherwise
   * @deprecated From 12.1 this method is no longer used, since exceptions are now thrown.
   */
  SaxonApiException *getException();

  /**
   * Get the SaxonProcessor which created this XQueryProcessor.
   */
  SaxonProcessor* getSaxonProcessor() { return proc; }

private:
  void createException(const char *message = nullptr);
  bool streaming;
  std::string cwdXQ; /*!< Current working directory */
  SaxonProcessor *proc; /*!< Pointer to the SaxonProcessor object which created this XQueryProcessor */

  int64_t cppXQ;
  std::map<std::string, XdmValue *>
      parameters; /*!< Map of parameters used for the query as (string, value)
                     pairs */
  std::map<std::string, std::string>
      properties; /*!< Map of properties used for the query as (string, string)
                     pairs */
  SaxonApiException *exception;
};

#endif /* SAXON_XQUERY_H */
