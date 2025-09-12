////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2022 - 2025 Saxonica Limited.
// This Source Code Form is subject to the terms of the Mozilla Public License,
// v. 2.0. If a copy of the MPL was not distributed with this file, You can
// obtain one at http://mozilla.org/MPL/2.0/. This Source Code Form is
// "Incompatible With Secondary Licenses", as defined by the Mozilla Public
// License, v. 2.0.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef SAXON_XSLT30_H
#define SAXON_XSLT30_H

#include "saxonc_export.h"
#include "saxonc/SaxonProcessor.h"
//#include "XdmValue.h"
#include <string>

class SaxonProcessor;
class SaxonApiException;
class XsltExecutable;
class XdmValue;
class XdmItem;
class XdmNode;

/** An <code>Xslt30Processor</code> represents a factory to compile, load and
 * execute stylesheets. It is possible to cache the context and the stylesheet
 * in the <code>Xslt30Processor</code>.
 */
class SAXONC_EXPORT Xslt30Processor {

  friend class XsltExecutable;

public:
  /**
   * Default constructor.
   * Creates a Saxon-HE XSLT processor
   * @throws SaxonApiException
   */
  Xslt30Processor();

  /**
   * Constructor with the SaxonProcessor supplied.
   * @param proc - pointer to the SaxonProcessor object
   * @param cwd - the current working directory
   * @throws SaxonApiException
   */
  Xslt30Processor(SaxonProcessor *proc, std::string cwd = "");

  /**
   * Xslt30Processor copy constructor.
   * @param other - Xslt30Processor
   */
  Xslt30Processor(const Xslt30Processor &other);

  ~Xslt30Processor();

  /**
   * Get the SaxonProcessor object
   * @return Pointer to the SaxonProcessor object
   */
  SaxonProcessor *getSaxonProcessor() { return proc; }

  /**
   * Set the current working directory (cwd).
   * This method also applies to the static base URI for XSLT stylesheets when
   * supplied as lexical string. The cwd is used to set the base URI is part of
   * the static context, and is used to resolve any relative URIs appearing
   * within XSLT.
   * @param cwd - current working directory
   */
  void setcwd(const char *cwd);


 /**
  * Get the current working directory (cwd). Memory deallocation is handled internally.
  * The cwd is used to set the base URI is part of the static context, and is
  * used to resolve any relative URIs appearing within XSLT.
  * @return Current working directory
  */
 const char * getcwd();

  /**
   * Set the base output URI.
   * The base output URI is used for resolving relative URIs in the
   * <code>href</code> attribute of the <code>xsl:result-document</code>
   * instruction; it is accessible to XSLT stylesheet code using the XPath
   * current-output-uri() function
   *
   * @param baseURI - the base output URI
   */
  void setBaseOutputURI(const char *baseURI);

  /**
   * Say whether just-in-time compilation of template rules should be used.
   * <p><b>Recommendation:</b> disable this option unless you are confident that
   * the stylesheet you are compiling is error-free.</p>
   *
   * @param jit - true if just-in-time compilation is to be enabled. With this
   * option enabled, static analysis of a template rule is deferred until the
   * first time that the template is matched. This can improve performance when
   * many template rules are rarely used during the course of a particular
   * transformation; however, it means that static errors in the stylesheet will
   * not necessarily cause the compile(Source) method to throw an exception
   * (errors in code that is actually executed will still be notified to the
   * registered <code>ErrorListener</code> or <code>ErrorList</code>, but this
   * may happen after the <code>compile(Source)</code> method returns). This
   * option is enabled by default in Saxon-EE, and is not available in Saxon-HE
   * or Saxon-PE.
   */
  void setJustInTimeCompilation(bool jit);

 /**
  * Get the status of the just-in-time compilation flag.
  * <p><b>Recommendation:</b> disable this option unless you are confident that
  * the stylesheet you are compiling is error-free.</p>
  *
  * @return Status of the just-in-time compilation flag
  */
 bool getJustInTimeCompilation();

  /**
   * Set the target edition under which the stylesheet will be executed.
   * @param edition - the Saxon edition for the run-time environment. One of
   * "EE", "PE", "HE", or "JS", "JS2", or "JS3".
   */
  void setTargetEdition(const char *edition);

  /**
   * Set the XSLT (and XPath) language level to be supported by the processor.
   * Set the value to "4.0"
   * to enable support for experimental features defined in the XSLT 4.0
   * proposal (which is likely to change before it stabilizes).
   * @param version - the language level to be supported. The values "3.0" and
   * "4.0" are recognized
   */
  void setXsltLanguageVersion(const char *version);

  /**
   * Request fast compilation.
   *
   * Fast compilation will generally be achieved at the expense of run-time
   * performance and quality of diagnostics. Fast compilation is a good
   * trade-off if (a) the stylesheet is known to be correct, and (b) once
   * compiled, it is only executed once against a document of modest size.
   * <p>Fast compilation may result in static errors going unreported,
   * especially if they occur in code that is never executed.</p> <p><i>The
   * current implementation is equivalent to switching off all optimizations
   * other than just-in-time compilation of template rules. Setting this option,
   * however, indicates an intent rather than a mechanism, and the
   * implementation details may change in future to reflect the intent.</i></p>
   * @param fast - set to true to request fast compilation; set to false to
   * revert to the optimization options defined in the Configuration.
   */
  void setFastCompilation(bool fast);

  /**
   * Indicate that packages compiled by this processor are deployable to a different location.
   * Say whether any package produced by this compiler can be deployed to a
   * different location, with a different base URI.
   * @param relocatable - if true then static-base-uri() represents the deployed
   * location of the package, rather than its compile time location
   */
  void setRelocatable(bool relocatable);

  /**
   * Set the value of a stylesheet parameter.
   * Static (compile-time) parameters must be provided using
   * this method, prior to stylesheet compilation.
   * Non-static parameters may also be provided using this method if their
   * values will not vary from one transformation to another.
   *
   * @param name - the name of the stylesheet parameter, as a string. For
   * a namespaced name use clark notation i.e. "{uri}local"
   * @param value - the value of the stylesheet parameter, or nullptr to clear a
   * previously set value
   */
  void setParameter(const char *name, XdmValue *value);

  /**
   * Get the value of a stylesheet parameter
   * @param name - the name of the stylesheet parameter
   * @return The value of the parameter as an XdmValue
   */
  XdmValue *getParameter(const char *name);

  /**
   * Remove a parameter (name, value) pair from a stylesheet
   *
   * @param name - the name of the stylesheet parameter
   * @return Outcome of the removal
   */
  bool removeParameter(const char *name);

  /**
   * Get all parameters as a std::map.
   * Please note that the key name has been prefixed with 'param:', for example
   * 'param:name'
   * @return An std:map with key as string name mapped to XdmValue.
   *
   */
  std::map<std::string, XdmValue *> &getParameters();

  /**
   * Clear stylesheet parameter values set.
   * Default behaviour (false) is to leave XdmValues in memory.
   * Individual pointers to XdmValue objects have to be deleted in the calling
   * program.
   * @param deleteValues - if true then XdmValues are deleted
   */
  void clearParameters(bool deleteValues = false);


  /**
   * Utility method for working with SaxonC on Python - internal use only
   */
  char **createCharArray(int len) { return (new char *[len]); }

  /**
   * Utility method for Python API - internal use only.
   * This method deletes a XdmValue pointer array
   * @param arr - XdmValue pointer array
   * @param len - length of the array
   */
  void deleteXdmValueArray(XdmValue **arr, int len) {
    delete[] arr;
  }

  /**
   * Perform a one shot transformation.
   * The result is stored in the specified output file.
   *
   * @param sourcefile - the file name of the source document
   * @param stylesheetfile - the file name of the stylesheet document
   * @param outputfile - the file name where results will be stored
   * @throws SaxonApiException if the transformation fails
   */
  void transformFileToFile(const char *sourcefile, const char *stylesheetfile,
                           const char *outputfile);

  /**
   * Perform a one shot transformation.
   * The result is returned as a string.
   *
   * @param sourcefile - the file name of the source document
   * @param stylesheetfile - the file name of the stylesheet document
   * @return The result of the transformation as a string.
   * A zero length string is returned if the transformation result is an empty sequence.
   * The caller is responsible for memory deallocation using `operator delete`.
   * @throws SaxonApiException if the transformation fails
   */
  const char *transformFileToString(const char *sourcefile,
                                    const char *stylesheetfile);

  /**
   * Perform a one shot transformation. The result is returned as an XdmValue
   *
   * @param sourcefile - the file name of the source document
   * @param stylesheetfile - the file name of the stylesheet document
   * @return The result of the transformation as an XdmValue.
   * The caller is responsible for memory deallocation using `delete`.
   * @throws SaxonApiException if the transformation fails
   */
  XdmValue *transformFileToValue(const char *sourcefile,
                                 const char *stylesheetfile);

  /**
   * Import a library package.
   * Calling this method makes the supplied package available for reference
   * in the <code>xsl:use-package</code> declarations of subsequent compilations
   * performed using this <code>Xslt30Processor</code>.
   * @param packageFile - the file name of the package to be imported, which
   * should be supplied as an SEF. If relative, the file name of the SEF is
   * resolved against the cwd, which is set using <code>setcwd()</code>.
   */
  void importPackage(const char *packageFile);

  /**
   * Compile a stylesheet file.
   * Note: the term "compile" here indicates that the stylesheet is converted
   * into an executable form. The compilation uses a snapshot of the properties
   * of the <code>Xslt30Processor</code> at the time this method is invoked.
   * @param stylesheet - the file name of the stylesheet document
   * @return An XsltExecutable, which represents the compiled stylesheet. The
   * XsltExecutable is immutable and thread-safe; it may be used to run multiple
   * transformations, in series or concurrently.
   * The caller is responsible for memory deallocation using `delete`.
   * @throws SaxonApiException if the compilation of the stylesheet fails
   */
  XsltExecutable *compileFromFile(const char *stylesheet);

  /**
   * Compile a stylesheet supplied as a string.
   * Note: the term "compile" here indicates that the stylesheet is converted
   * into an executable form. The compilation uses a snapshot of the properties
   * of the <code>Xslt30Processor</code> at the time this method is invoked.
   * @param stylesheet - the stylesheet as a lexical string representation
   * @param encoding - the encoding of the stylesheet string. If not specified
   * then the platform default encoding is used.
   * @return An XsltExecutable, which represents the compiled stylesheet. The
   * XsltExecutable is immutable and thread-safe; it may be used to run multiple
   * transformations, in series or concurrently.
   * The caller is responsible for memory deallocation using `delete`.
   * @throws SaxonApiException if the compilation of the stylesheet fails
   */
  XsltExecutable *compileFromString(const char *stylesheet,
                                    const char *encoding = nullptr);

  /**
   * Compile a stylesheet as referenced in a specified XML document via the xml-stylesheet processing instruction.
   * Note: the term "compile" here indicates that the stylesheet is converted
   * into an executable form. The compilation uses a snapshot of the properties
   * of the <code>Xslt30Processor</code> at the time this method is invoked.
   * @param sourceFile - the file name of the XML document
   * @return An XsltExecutable, which represents the compiled stylesheet. The
   * XsltExecutable is immutable and thread-safe; it may be used to run multiple
   * transformations, in series or concurrently.
   * The caller is responsible for memory deallocation using `delete`.
   * @throws SaxonApiException if the compilation of the stylesheet fails
   */
  XsltExecutable *compileFromAssociatedFile(const char *sourceFile);

  /**
   * Compile a stylesheet received as a string and save to an exported file (SEF).
   * The compiled stylesheet is saved as SEF to file store.
   * @param stylesheet - the stylesheet as a lexical string representation
   * @param filename - the file to which the compiled package should be saved
   * @param encoding - the string encoding of the filename. Defaults to JVM default
   * @throws SaxonApiException if the compilation of the stylesheet fails
   */
  void compileFromStringAndSave(const char *stylesheet, const char *filename,
                                const char *encoding = nullptr);

  /**
   * Compile a stylesheet received as a file and save to an exported file (SEF).
   * The compiled stylesheet is saved as SEF to file store.
   * @param xslFilename - file name of the stylesheet
   * @param filename - the file to which the compiled package should be saved
   * @throws SaxonApiException if the compilation of the stylesheet fails
   */
  void compileFromFileAndSave(const char *xslFilename, const char *filename);

  /**
   * Compile a stylesheet received as an XdmNode.
   * The compiled stylesheet is cached and available for execution later.
   * @param node - the XdmNode object for the stylesheet
   * @param filename - the file to which the compiled package should be saved
   * @throws SaxonApiException if the compilation of the stylesheet fails
   */
  void compileFromXdmNodeAndSave(XdmNode *node, const char *filename);

  /**
   * Compile a stylesheet received as an XdmNode.
   * Note: the term "compile" here indicates that the stylesheet is converted
   * into an executable form. The compilation uses a snapshot of the properties
   * of the <code>Xslt30Processor</code> at the time this method is invoked.
   * @param node - the XdmNode object for the stylesheet
   * @return An XsltExecutable, which represents the compiled stylesheet. The
   * XsltExecutable is immutable and thread-safe; it may be used to run multiple
   * transformations, in series or concurrently.
   * The caller is responsible for memory deallocation using `delete`.
   * @throws SaxonApiException if the compilation of the stylesheet fails
   */
  XsltExecutable *compileFromXdmNode(XdmNode *node);

  /**
   * Deprecated. Checks for pending exceptions without creating a local reference to the
   * exception object.
   * @deprecated From 12.1 this method is no longer used, since exceptions are now thrown.
   * @return True when there is a pending exception; otherwise false
   */
  bool exceptionOccurred();

  /**
   * Deprecated. Get the SaxonApiException object created when we have an error.
   * After use of the processor if there is an error then a
   * SaxonApiException is created.
   * @deprecated From 12.1 this method is no longer used, since exceptions are now thrown.
   * @return The SaxonApiException object if there is an exception thrown, or nullptr otherwise
   */
  SaxonApiException *getException();

  /**
   * Deprecated. Clear any thrown exceptions
   * @deprecated From 12.1 this method is no longer used, since exceptions are now thrown.
   */
  void exceptionClear();

  /**
   * Deprecated. Get the first error message if there are any errors.
   * A transformation may have a number of errors reported against it.
   * @return The message of the exception
   * @deprecated From 12.1 this method is no longer used, since exceptions are now thrown.
   */
  const char *getErrorMessage();

  /**
   * Deprecated. Get the first error code if there are any errors
   * A transformation may have a number of errors reported against it.
   * @return The error code of the exception. The error codes are
   * related to the specific specification
   * @deprecated From 12.1 this method is no longer used, since exceptions are now thrown.
   */
  const char *getErrorCode();

private:
  void createException(const char *message = nullptr);

  /**
   * Set a configuration property specific to the XSLT processor in use.
   * Xslt30Processor: set serialization properties (names start with '!' i.e. name
   * "!method" -> "xml"), 'o':outfile name, 'it': initial template, 'im': initial
   * mode, 's': source as file name, 'm': switch on message listener for
   * xsl:message instructions
   * @param name - the name of the property
   * @param value - the value of the property
   */
  void setProperty(const char *name, const char *value);

 /**
  * Get all configuration properties specified on the processor as a std::map.
  * @return Map of (string, string) pairs
  */
 std::map<std::string, std::string> &getProperties();

  /**
   * Clear configuration property values set
   */
  void clearProperties();

  SaxonProcessor *proc; /*!< Pointer to the SaxonProcessor object which created this Xslt30Processor */
  int64_t cppXT;
  int64_t importPackageValue;
  std::string cwdXT;   /*!< Current working directory */
  bool jitCompilation; /*!< Just-in-time compilation flag*/
  std::map<std::string, XdmValue *>
      parameters; /*!< Map of parameters used for the transformation as (string,
                     value) pairs */
  std::map<std::string, std::string>
      properties; /*!< Map of properties used for the transformation as (string,
                     string) pairs */
  SaxonApiException *exception;
};

#endif /* SAXON_XSLT30_H */
