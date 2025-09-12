////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2022 - 2024 Saxonica Limited.
// This Source Code Form is subject to the terms of the Mozilla Public License,
// v. 2.0. If a copy of the MPL was not distributed with this file, You can
// obtain one at http://mozilla.org/MPL/2.0/. This Source Code Form is
// "Incompatible With Secondary Licenses", as defined by the Mozilla Public
// License, v. 2.0.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef DOCUMENT_BUILDER_H
#define DOCUMENT_BUILDER_H

#include "saxonc_export.h"
#include "saxonc/SaxonProcessor.h"
#include <string>

class SaxonProcessor;
class SaxonApiException;
class XdmNode;
class SchemaValidator;

/**
 * A document builder holds properties controlling how a Saxon document tree
 * should be built, and provides methods to invoke the tree construction.
 * <p>This class has no public constructor.  To construct a DocumentBuilder,
 * use the factory method SaxonProcessor.newDocumentBuilder().</p>
 * <p>All documents used in a single Saxon query, transformation, or validation
 * episode must be built with the same SaxonProcessor. However, there is no
 * requirement that they should use the same <code>DocumentBuilder</code>.</p>
 * <p>Sharing of a <code>DocumentBuilder</code> across multiple threads is not
 * recommended. However, in the current implementation sharing a
 * <code>DocumentBuilder</code> (once initialized) will only cause problems if a
 * <code>SchemaValidator</code> is used.</p>
 */
class SAXONC_EXPORT DocumentBuilder {

  friend class SaxonProcessor;

public:
  /**
   * Destructor method
   */
  ~DocumentBuilder();

  /**
   * Set whether line and column numbering is to be enabled for documents
   * constructed using this DocumentBuilder. This has the effect that the line
   * and column number in the original source document is maintained in the
   * constructed tree, for each element node (and only for elements). The line
   * and column number in question are generally the position at which the
   * closing "&gt;" of the element start tag appears. <p>By default, line and
   * column numbers are not maintained.</p> <p>Errors relating to document
   * parsing and validation will generally contain line numbers whether or not
   * this option is set, because such errors are detected during document
   * construction.</p> <p>Line numbering is not available for all kinds of
   * source: for example, it is not available when loading from an existing DOM
   * Document.</p>
   *
   * @param option - true if line numbers are to be maintained, false otherwise.
   */

  void setLineNumbering(bool option);

  /**
   * Ask whether line and column numbering is enabled for documents loaded using
   * this <code>DocumentBuilder</code>. <p>By default, line and column numbering
   * is disabled.</p> <p>Line numbering is not available for all kinds of
   * source: in particular, it is not available when loading from an existing
   * DOM Document.</p>
   * @return True if line numbering is enabled
   */

  bool isLineNumbering();

  /**
   * Set the SchemaValidator to be used. This determines whether schema
   * validation is applied to an input document and whether type annotations in
   * a supplied document are retained. If no SchemaValidator is supplied, then
   * schema validation does not take place. <p>This option requires the
   * schema-aware version of the Saxon product (Saxon-EE).</p> <p>Since a
   * <code>SchemaValidator</code> is serially reusable but not thread-safe,
   * using this method is not appropriate when the <code>DocumentBuilder</code>
   * is shared between threads.</p>
   *
   * @param validator - the SchemaValidator to be used
   */

  void setSchemaValidator(SchemaValidator *validator);

  /**
   * Get the SchemaValidator used to validate documents loaded using this
   * <code>DocumentBuilder</code>.
   *
   * @return The SchemaValidator if one has been set; otherwise null. The caller is responsible for memory deallocation.
   */
  SchemaValidator *getSchemaValidator();

  /**
   * Set whether DTD validation should be applied to documents loaded using this
   * <code>DocumentBuilder</code>.
   * <p>By default, no DTD validation takes place.</p>
   *
   * @param option - true if DTD validation is to be applied to the document
   */

  void setDTDValidation(bool option);

  /**
   * Ask whether DTD validation is to be applied to documents loaded using this
   * <code>DocumentBuilder</code>.
   *
   * @return True if DTD validation is to be applied
   */

  bool isDTDValidation();

  /**
   * Set the base URI of a document loaded using this
   * <code>DocumentBuilder</code>. <p>This is used for resolving any relative
   * URIs appearing within the document, for example in references to DTDs and
   * external entities.</p> <p>This information is required when the document is
   * loaded from a source that does not provide an intrinsic URI, notably when
   * loading from a Stream or a DOMSource. The value is ignored when loading
   * from a source that does have an intrinsic base URI.</p>
   *
   * @param uri - the base URI of documents loaded using this
   * <code>DocumentBuilder</code>. This must be an absolute URI.
   */

  void setBaseUri(const char *uri);

  /**
   * Get the base URI of documents loaded using this DocumentBuilder when no
   * other URI is available.
   *
   * @return The base URI to be used, or null if no value has been set. Memory deallocation is handled internally.
   */

  const char *getBaseUri();

  /**
   * Load an XML document, to create a tree representation of the document in
   * memory.
   *
   * @param content - the XML document as a serialized string
   * @param encoding - the encoding of the source document string. If not specified then
   * the platform default encoding is used.
   * @return The document node at the root of the tree of the resulting
   * in-memory document, as an XdmNode. The caller is responsible for memory deallocation.
   * @throws SaxonApiException if there is a failure in the parsing of the XML
   * document
   */

  XdmNode *parseXmlFromString(const char *content,
                              const char *encoding = nullptr);

  /**
   * Build a document from a supplied XML file.
   *
   * @param filename - the file name for the source document
   * @return The XdmNode representing the root of the document tree
   * @throws SaxonApiException if there is a failure in the parsing of the XML
   * document
   */

  XdmNode *parseXmlFromFile(const char *filename);

  /**
   * Build a document from a supplied URI source.
   *
   * @param source - URI for the XML source document
   *
   * @return The document node at the root of the tree of the resulting
   * in-memory document, as an XdmNode.
   * @throws SaxonApiException if there is a failure in the parsing of the XML
   * document
   */
  XdmNode *parseXmlFromUri(const char *source);

  /**
   * Deprecated. Checks for pending exceptions without creating a local reference to the
   * exception object
   * @deprecated From 12.1 this method is no longer used, since exceptions are now thrown.
   * @return True when there is a pending exception; otherwise false
   */
  bool exceptionOccurred();

 /**
  * Deprecated. Clear any thrown exceptions.
  * @deprecated From 12.1 this method is no longer used, since exceptions are now thrown.
  */
  void exceptionClear();

  /**
   * Deprecated. Get the first error message if there are any errors.
   * @deprecated From 12.1 this method is no longer used, since exceptions are now thrown.
   * @return The message of the exception
   */
  const char *getErrorMessage();

  /**
   * Deprecated. Get the first error code if there are any errors.
   * @deprecated From 12.1 this method is no longer used, since exceptions are now thrown.
   * @return The error code of the exception
   */
  const char *getErrorCode();

  /**
   * Deprecated. Get the SaxonApiException object created when we have an error.
   * @deprecated From 12.1 this method is no longer used, since exceptions are now thrown.
   * @return The SaxonApiException object if there is an exception thrown, or nullptr otherwise
   */
  SaxonApiException *getException();

private:
  /**
   * Create a DocumentBuilder. This is a protected constructor. Users should
   * construct a DocumentBuilder by calling the factory method
   * SaxonProcessor.newDocumentBuilder().
   */

  DocumentBuilder();

  /**
   * Constructor with the SaxonProcessor supplied.
   * @param p - supplied pointer to the SaxonProcessor object
   * @param docBuilderObject - reference to the underlying Java DocumentBuilder object
   * @param cwd - the current working directory
   */
  DocumentBuilder(SaxonProcessor *p, int64_t docBuilderObject, std::string cwd);

  /**
   * DocumentBuilder copy constructor.
   * @param other - DocumentBuilder
   */
  DocumentBuilder(const DocumentBuilder &other);

  void createException(const char *message = nullptr);

  SchemaValidator *schemaValidator;
  SaxonProcessor *proc;
  SaxonApiException *exception;
  int64_t docBuilderObject;
  // jclass  docBuilderClass, procClass;
  // jclass saxonCAPIClass;
  std::string baseURI;
  std::string cwdDB; /*!< current working directory */
  bool lineNumbering, dtdVal;
};

#endif /* DOCUMENT_BUILDER_H */
