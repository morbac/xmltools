#ifndef SAXONCPROC_H
#define SAXONCPROC_H

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2022 - 2023 Saxonica Limited.
// This Source Code Form is subject to the terms of the Mozilla Public License,
// v. 2.0. If a copy of the MPL was not distributed with this file, You can
// obtain one at http://mozilla.org/MPL/2.0/. This Source Code Form is
// "Incompatible With Secondary Licenses", as defined by the Mozilla Public
// License, v. 2.0.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "saxonc_export.h"
#include "saxonc/SaxonCGlue.h"

/*!    \file   SaxonCProcessor.h
    \brief  <code>SaxonCProcessor.h</code> provides the C API for XSLT and
   XQuery processing. This file contains a set of functions to compile and
   execute stylesheets and queries.

*/

/** <code>sxnc_xquery</code>. This struct is used to capture the Java processor
 * objects created in C for XQuery. <p/>
 */
typedef struct {
  int64_t xqueryProc; //!< ObjectHandle reference to the underlying XQueryProcessor
} sxnc_xquery;

/** <code>sxnc_xslt</code>. This struct is used to capture the Java
 * processor objects created in C for XSLT. <p/>
 */
typedef struct {
  int64_t xsltProc; //!< ObjectHandle reference to the underlying XSLTProcessor
} sxnc_xslt;

/** <code>sxnc_xpath</code>. This struct is used to capture the Java
 * processor objects created in C for XPath. <p/>
 */
typedef struct {
  int64_t xpathProc; //!< ObjectHandle reference to the underlying XPathProcessor
} sxnc_xpath;

EXTERN_SAXONC

/**
 * Get the Saxon version
 */
SAXONC_EXPORT const char *version(sxnc_environment *environi, sxnc_processor *proc);

/**
 * Get the Saxon version and edition
 */
SAXONC_EXPORT const char *getProductVariantAndVersion(sxnc_environment *environi,
                                        sxnc_processor *proc);

/**
 * Initialise the SaxonC Processor, along with the parameters and the properties
 * pointers. This utility function is used to allocate memory on the heap,
 * therefore is required to be called before execution of stylesheets or
 * queries.
 *
 * @param environi - GraalVM environment held as the struct sxnc_environment.
 * This function calls malloc on the pointer.
 * @param proc - Pointer to the Saxon Processor which is represented as the
 * struct sxnc_processor and is used for creating the XSLT and XQuery
 * processors. This function calls malloc on the pointer.
 * @param param - Pointers to the sxnc_parameter structs used for parameters.
 * This function calls calloc on the pointers.
 * @param prop - Pointers to the sxnc_property structs used for properties. This
 * function calls calloc on the pointers.
 * @param cap - The length of the parameters array
 * @param propCap - The length of the properties array
 */
SAXONC_EXPORT void initSaxonc(sxnc_environment **environi, sxnc_processor **proc,
                sxnc_parameter **param, sxnc_property **prop, int cap,
                int propCap);

/**
 * Free associated memory on the heap created by the SaxonC Processors,
 * properties and parameters.
 *
 * @param environi - GraalVM environment held as the struct sxnc_environment
 * @param proc - Pointer to the Saxon Processor which is represented as the
 * struct sxnc_processor
 * @param param - Pointers to the sxnc_parameter structs used for parameters
 * @param prop - Pointers to the sxnc_property structs used for properties
 */
SAXONC_EXPORT void freeSaxonc(sxnc_environment **environi, sxnc_processor **proc,
                sxnc_parameter **param, sxnc_property **prop);

/**
 * Set a configuration property specific to the processor in use.
 *
 * @param environi - GraalVM environment
 * @param proc - Pointer to the Saxon Processor which is represented as the
 * struct sxnc_processor
 * @param name - the name of the property
 * @param value - the value for the property
 */

SAXONC_EXPORT void setConfigurationProperty(sxnc_environment *environi, sxnc_processor *proc,
                                            char *name, char *value);

/**
 * Set a catalog file to be used in Saxon.
 *
 * @param environi - GraalVM environment
 * @param proc - Pointer to the Saxon Processor which is represented as the
 * struct sxnc_processor
 * @param cwd - Current working directory
 * @param catalogFileName - file name to the catalog
 * @deprecated - Use the setCatalogFiles() method
 */

SAXONC_EXPORT void setCatalog(sxnc_environment *environi, sxnc_processor *proc,
                                            char *cwd, char *catalogFileName);


/**
 * Set multiple catalog files to be used in Saxon.
 *
 * @param environi - GraalVM environment
 * @param proc - Pointer to the Saxon Processor which is represented as the
 * struct sxnc_processor
 * @param cwd - Current working directory
 * @param catalogFileNames - file names to the catalog as an array of pointers
 * @param length - Number of cataFileNames to be added
 */

SAXONC_EXPORT void setCatalogFiles(sxnc_environment *environi, sxnc_processor *proc,
                                            char *cwd, char **catalogFileName, int length);

/**
 * Transform an XSLT stylesheet with the result saved to file.
 * The compile and execution is done in the same call of this method.
 *
 * @param environi - GraalVM environment
 * @param proc - Pointer to the Saxon Processor used for creating the XSLT
 * executable
 * @param cwd - Current working directory - used for base URI for input and
 * output files
 * @param source - The file name of the source document
 * @param stylesheet - The file name of the stylesheet document
 * @param outputfile - The file name of where the result will be stored
 * @param parameters - Array of parameters for the transform, as mappings
 * (string, value)
 * @param properties - Array of properties for the transform, as mappings
 * (string, string). Available XSLT Processor properties: serialization
 * properties names start with '!' (e.g.
 * '!method' -> 'xml'), 'o': output file name, 'it': initial template, 'im':
 * initial mode, 's': source file name, 'm': switch on message listener for
 * xsl:message instructions
 * @param parLen - The length of the parameters array
 * @param propLen - The length of the properties array
 */
SAXONC_EXPORT void xsltSaveResultToFile(sxnc_environment *environi, sxnc_processor *proc,
                          char *cwd, char *source, char *stylesheet,
                          char *outputfile, sxnc_parameter *parameters,
                          sxnc_property *properties, int parLen, int propLen);

/**
 * Transform an XSLT stylesheet with the result returned as a string value (char
 * pointer array). The compile and execution is done in the same call of this
 * method.
 *
 * @param environi - GraalVM environment
 * @param proc - Pointer to the Saxon Processor used for creating the XSLT
 * executable
 * @param cwd - Current working directory - used for base URI for input and
 * output files
 * @param source - The file name of the source document
 * @param stylesheet - The file name of the stylesheet document
 * @param parameters - Array of parameters for the transform, as mappings
 * (string, value)
 * @param properties - Array of properties for the transform, as mappings
 * (string, string). Available XSLT Processor properties: serialization
 * properties names start with '!' (e.g.
 * '!method' -> 'xml'), 'o': output file name, 'it': initial template, 'im':
 * initial mode, 's': source file name, 'm': switch on message listener for
 * xsl:message instructions
 * @param parLen - The length of the parameters array
 * @param propLen - The length of the properties array
 * @return char pointer array - The result of the transformation
 */
SAXONC_EXPORT const char *xsltApplyStylesheet(sxnc_environment *environi,
                                sxnc_processor *proc, char *cwd, char *source,
                                char *stylesheet, sxnc_parameter *parameters,
                                sxnc_property *properties, int parLen,
                                int propLen);
/**
 * Execute a query with the result saved to file.
 *
 * @param environi - GraalVM environment
 * @param proc - Pointer to the Saxon Processor used for creating the XQuery
 * executable
 * @param cwd - Current working directory - used for base URI for input and
 * output files
 * @param outputfile - The file name of where the result will be stored
 * @param parameters - Array of parameters for the query, as mappings (string,
 * value)
 * @param properties - Array of properties for the query, as mappings (string,
 * string). Available XQuery Processor properties: serialization properties
 * names start with '!' (e.g.
 * '!method' -> 'xml'), 'o': output file name, 's': source file name, 'q': query
 * file name, 'qs': string form of the query, 'base': base URI of the query,
 * 'dtd': set DTD validation 'on' or 'off'
 * @param parLen - The length of the parameters array
 * @param propLen - The length of the properties array
 */
SAXONC_EXPORT void executeQueryToFile(sxnc_environment *environi, sxnc_processor *proc,
                        char *cwd, char *outputfile, sxnc_parameter *parameters,
                        sxnc_property *properties, int parLen, int propLen);

/**
 * Execute a query with the result returned as a string value (char pointer
 * array).
 *
 * @param environi - GraalVM environment
 * @param proc - Pointer to the Saxon Processor used for creating the XQuery
 * executable
 * @param cwd - Current working directory - used for base URI for input and
 * output files
 * @param parameters - Array of parameters for the query, as mappings (string,
 * value)
 * @param properties - Array of properties for the query, as mappings (string,
 * string). Available XQuery Processor properties: serialization properties
 * names start with '!' (e.g.
 * '!method' -> 'xml'), 'o': output file name, 's': source file name, 'q': query
 * file name, 'qs': string form of the query, 'base': base URI of the query,
 * 'dtd': set DTD validation 'on' or 'off'
 * @param parLen - The length of the parameters array
 * @param propLen - The length of the properties array
 */
SAXONC_EXPORT const char *executeQueryToString(sxnc_environment *environi,
                                 sxnc_processor *proc, char *cwd,
                                 sxnc_parameter *parameters,
                                 sxnc_property *properties, int parLen,
                                 int propLen);

/**
 * Get a pointer to the current error message as a string
 *
 * @param environi - GraalVM environment
 */
SAXONC_EXPORT const char *c_getErrorMessage(sxnc_environment *environi);

EXTERN_SAXONC_END

#endif
