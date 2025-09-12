////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2022 - 2025 Saxonica Limited.
// This Source Code Form is subject to the terms of the Mozilla Public License,
// v. 2.0. If a copy of the MPL was not distributed with this file, You can
// obtain one at http://mozilla.org/MPL/2.0/. This Source Code Form is
// "Incompatible With Secondary Licenses", as defined by the Mozilla Public
// License, v. 2.0.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef SAXON_XPATH_H
#define SAXON_XPATH_H

#include "saxonc_export.h"
#include "saxonc/SaxonProcessor.h"
//#include "XdmValue.h"
//#include "XdmItem.h"

#include <string>

class SaxonProcessor;
class SaxonApiException;
class XdmValue;
class XdmItem;

enum class SAXONC_EXPORT UnprefixedElementMatchingPolicy {

  /**
   * The standard W3C policy, whereby element names are implicitly qualified by
   * the default namespace for elements and types, as defined in the XPath
   * static context. In XSLT this can be set using the
   * <code>[xsl:]xpath-default-namespace</code> attribute, or programmatically
   * using {@link XsltCompiler#setDefaultElementNamespace(String)}
   */
  DEFAULT_NAMESPACE = 0,
  /**
   * Under this policy, unprefixed element names match on the local part only;
   * an element with this local name is matched regardless of its namespace
   * (that is, it can have any namespace, or none)
   */
  ANY_NAMESPACE = 1,
  /**
   * Under this policy, unprefixed element names match provided that (a) the
   * local part of the name matches, and (b) the namespace part of the name is
   * either equal to the default namespace for elements and types, or is absent.
   *
   * <p>This policy is provided primarily for use with HTML, where it can be
   * unpredictable whether HTML elements are in the XHTML namespace or in no
   * namespace. It is also useful with other vocabularies where instances are
   * sometimes in a namespace and sometimes not. The policy approximates to the
   * special rules defined in the HTML5 specification, which states that
   * unprefixed names are treated as matching names in the XHTML namespace if
   * the context item for a step is "a node in an HTML DOM", and as matching
   * no-namespace names otherwise; since in the XDM data model it is not
   * possible to make a distinction between different kinds of DOM, this policy
   * allows use of unprefixed names both when matching elements in the XHTML
   * namespace and when matching no-namespace elements</p>
   */
  DEFAULT_NAMESPACE_OR_NONE = 2
};

/** An <code>XPathProcessor</code> represents a factory to compile, load and
 * execute XPath expressions.
 */
class SAXONC_EXPORT XPathProcessor {
public:
 /**
  * Default constructor.
  *
  * Creates a Saxon-HE XPath processor.
  */
  XPathProcessor();

  ~XPathProcessor();

  /**
   * Constructor with the SaxonProcessor supplied.
   * @param proc - pointer to the SaxonProcessor object
   * @param cwd - the current working directory
   * @throws SaxonApiException
   */
  XPathProcessor(SaxonProcessor *proc, std::string cwd = "");

  /**
   * XPathProcessor copy constructor.
   * @param other - XPathProcessor
   */
  XPathProcessor(const XPathProcessor &other);

  /**
   * Set the static base URI for XPath expressions compiled using this XPathProcessor.
   *
   * The base URI is part of the static context, and is used to resolve any
   * relative URIs appearing within an XPath expression, for example a relative
   * URI passed as an argument to the doc() function. If no static base URI is
   * supplied, then the current working directory is used.
   * @param uriStr - the static base URI
   */
  void setBaseURI(const char *uriStr);

  /**
   * Get the static base URI for XPath expressions compiled using this XPathProcessor.
   * Note that memory deallocation is handled internally.
   * @return The static base URI as a string
   */
  const char *getBaseURI();

  /**
   * Compile and evaluate an XPath expression.
   * @param xpathStr - the XPath expression supplied as a character string
   * @param encoding - the encoding of the string. If not specified then the
   * platform default encoding is used.
   * @return An XdmValue representing the result of evaluating the expression or nullptr if
   * the expression returns an empty sequence.
   * The caller is responsible for memory deallocation using `delete`.
   * @throws SaxonApiException
   */
  XdmValue *evaluate(const char *xpathStr, const char *encoding = nullptr);

  /**
   * Compile and evaluate an XPath expression; the result is expected to be a single XdmItem or nullptr.
   * @param xpathStr - the XPath expression supplied as a character string
   * @param encoding - the encoding of the string. If not specified then the
   * platform default encoding is used.
   * @return An XdmItem representing the result of evaluating the expression
   * or nullptr if the expression returns an empty sequence.
   * If the expression returns a sequence of more than one item, any items after
   * the first are ignored.
   * The caller is responsible for memory deallocation using `delete`.
   * @throws SaxonApiException
   */
  XdmItem *evaluateSingle(const char *xpathStr, const char *encoding = nullptr);

  /** Set the context item for the expression
   * @param item - the initial context item
   */
  void setContextItem(XdmItem *item);

  /**
   * Set the current working directory
   * @param cwd - current working directory
   */
  void setcwd(const char *cwd);

 /**
  * Get the current working directory set on this XPathProcessor.
  * Memory deallocation is handled internally.
  * @return Current working directory
  */
 const char * getcwd();

  /** Set the context item from file
   */
  void setContextFile(const char *filename);

  /**
   * Evaluate the XPath expression, returning the effective boolean value of the result.
   * @param xpathStr - the XPath expression supplied as a character string
   * @param encoding - the encoding of the string. If not specified then the
   * platform default encoding is used.
   * @return The effective boolean value of the result
   * @throws SaxonApiException
   */
  bool effectiveBooleanValue(const char *xpathStr,
                             const char *encoding = nullptr);

  /**
   * Set the value of a parameter used in the XPath expression
   *
   * @param name - the name of the parameter, as a string. For a namespaced name use
   * clark notation i.e. "{uri}local"
   * @param value - the value of the parameter, or nullptr to clear a previously set
   * value
   * @param withParam - internal use only
   */

  void setParameter(const char *name, XdmValue *value, bool withParam=true);

  /**
   * Remove a parameter (name, value) pair
   *
   * @param name - the name of the parameter
   * @return Outcome of the removal
   */
  bool removeParameter(const char *name);

  /**
   * Set a configuration property specific to the XPath processor in use.
   * XPathProcessor: set serialization properties (names start with '!' i.e.
   * name "!method" -> "xml"), 'o':outfile name, 's': context item supplied as
   * file name
   * @param name - the name of the property
   * @param value - the value of the property
   */
  void setProperty(const char *name, const char *value);

 /**
  * Get the value of a configuration property specified on the XPath processor in use. Memory deallocation is handled internally.
  * @param name - the name of the property
  * @return Value of the property or nullptr if the property is not found. Memory deallocation is handled internally.
  */
 const char * getProperty(const char *name);

  /**
   * Declare a namespace binding as part of the static context for XPath expressions compiled using this XPathProcessor
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
   * Declare a variable as part of the static context for XPath expressions compiled using this XPathProcessor.
   *
   * It is an error for the XPath expression to refer to a variable unless it
   * has been declared. This method declares the existence of the variable, but
   * it does not bind any value to the variable; that is done later, when the
   * XPath expression is evaluated. The variable is allowed to have any type
   * (that is, the required type is <code>item()*</code>).
   *
   * @param name - the name of the parameter, as a string. For a namespaced name use
   * clark notation i.e. "{uri}local"
   */
  void declareVariable(const char *name);

  /**
   * Say whether an XPath 2.0, XPath 3.0, XPath 3.1 or XPath 4.0 processor is required.
   * Set the language version for the XPath compiler.
   *
   * @param version One of the values 1.0, 2.0, 3.0, 3.05, 3.1, 4.0.
   *              Setting the option to 1.0 requests an XPath 2.0 processor
   * running in 1.0 compatibility mode; this is equivalent to setting the
   * language version to 2.0 and backwards compatibility mode to true.
   *              Requesting "3.05" gives XPath 3.0 plus the extensions defined
   * in the XSLT 3.0 specification (map types and map constructors).
   */
  void setLanguageVersion(const char *version);

  /**
   * Say whether XPath 1.0 backwards compatibility mode is to be used.
   * In backwards compatibility
   * mode, more implicit type conversions are allowed in XPath expressions, for
   * example it is possible to compare a number with a string. The default is
   * false (backwards compatibility mode is off).
   *
   * @param option - true if XPath 1.0 backwards compatibility is to be enabled,
   * false if it is to be disabled.
   */
  void setBackwardsCompatible(bool option);

  /**
   * Say whether the compiler should maintain a cache of compiled expressions.
   * @param caching - if set to true, caching of compiled expressions is enabled.
   *                If set to false, any existing cache is cleared, and future
   * compiled expressions will not be cached until caching is re-enabled. The
   * cache is also cleared (but without disabling future caching) if any method
   * is called that changes the static context for compiling expressions, for
   * example declareVariable(QName) or declareNamespace(String, String).
   */

  void setCaching(bool caching);

  /**
   * Set the policy for matching unprefixed element names in XPath expressions.
   * Use the convertEnumPolicy method to create an UnprefixedElementMatchingPolicy value.
   * @param policy - the policy to be used, possible values: DEFAULT_NAMESPACE =
   * 0, ANY_NAMESPACE = 1 or DEFAULT_NAMESPACE_OR_NONE = 2
   *
   */
  void
  setUnprefixedElementMatchingPolicy(UnprefixedElementMatchingPolicy policy);

  /**
   * Convert an int into an UnprefixedElementMatchingPolicy.
   * To be used with the setUnprefixedElementMatchingPolicy method.
   *
   * @param n - the int representing the policy
   */
  UnprefixedElementMatchingPolicy convertEnumPolicy(int n) {
    return static_cast<UnprefixedElementMatchingPolicy>(n);
  }

  /**
   * Get the policy for matching unprefixed element names in XPath expressions
   * @return The policy being used
   */
  UnprefixedElementMatchingPolicy getUnprefixedElementMatchingPolicy();

  /**
   * Import a schema namespace.
   * That is, add the element and attribute declarations and type definitions
   * contained in a given namespace to the static context for the XPath
   * expression. <p>This method will not cause the schema to be loaded. This
   * method will not fail if the schema has not been loaded (but in that case
   * the set of declarations and definitions made available to the XPath
   * expression is empty). The schema document for the specified namespace may
   * be loaded before or after this method is called.</p> <p>This method does
   * not bind a prefix to the namespace. That must be done separately, using the
   * declareNamespace(String, String) method.</p>
   *
   * @param uri - the schema namespace to be imported. To import declarations in a
   * no-namespace schema, supply a zero-length string.
   */
  void importSchemaNamespace(const char *uri);

  /**
   * Get the value of a parameter
   * @param name - the name of the parameter
   * @param withParam - internal use only
   * @return The value of the parameter as an XdmValue
   */
  XdmValue *getParameter(const char *name, bool withParam=true);

  /**
   * Get all parameters as a std::map
   */
  std::map<std::string, XdmValue *> &getParameters();

  /**
   * Get all configuration properties specified on the processor as a std::map
   */
  std::map<std::string, std::string> &getProperties();

  /**
   * Clear parameter values set.
   * Default behaviour (false) is to leave XdmValues in memory.
   * Individual pointers to XdmValue objects have to be deleted in the calling
   * program.
   * @param deleteValues - if true then XdmValues are deleted
   */
  void clearParameters(bool deleteValues = false);

  /**
   * Clear property values set
   */
  void clearProperties();


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
   * An XPath expression may have a number of errors reported against it.
   * @return The message of the exception
   * @deprecated From 12.1 this method is no longer used, since exceptions are now thrown.
   */
  const char *getErrorMessage();

  /**
   * Deprecated. Get the first error code if there are any errors.
   * After the execution of the XPath expression there may be a number of
   * errors reported against it.
   * @return The error code of the exception
   * @deprecated From 12.1 this method is no longer used, since exceptions are now thrown.
   */
  const char *getErrorCode();

  /**
   * Deprecated. Get the SaxonApiException object created when we have an error.
   * After the execution of the processor if there is an error then a
   * SaxonApiException is created.
   * @return The SaxonApiException object if there is an exception thrown, or nullptr otherwise
   * @deprecated From 12.1 this method is no longer used, since exceptions are now thrown.
   */
  SaxonApiException *getException();

  /**
   * Get the SaxonProcessor which created this XPathProcessor.
   */
  SaxonProcessor* getSaxonProcessor() { return proc; }

private:
  void createException(const char *message = nullptr);


  SaxonProcessor *proc; /*!< Pointer to the SaxonProcessor object which created this XPathProcessor */
  std::string cwdXP;    /*!< Current working directory */
  char *xp_baseURI;
  // jclass  cppClass; /*!< Reference to the XPathProcessor Java class under JNI
  // */
  int64_t cppXP; /*!< The underlying XPathProcessor Java object  */
  std::map<std::string, XdmValue *>
      parameters; /*!< Map of parameters used for the transformation as (string,
                     value) pairs */
  std::map<std::string, std::string>
      properties; /*!< Map of properties used for the transformation as (string,
                     string) pairs */
  SaxonApiException
      *exception; /*!< Exception object created when there is an error */
  UnprefixedElementMatchingPolicy unprefixedElementPolicy;
};

#endif /* SAXON_XPATH_H */
