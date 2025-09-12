#ifndef SAXONCXPATH_H
#define SAXONCXPATH_H

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2022 - 2023 Saxonica Limited.
// This Source Code Form is subject to the terms of the Mozilla Public License,
// v. 2.0. If a copy of the MPL was not distributed with this file, You can
// obtain one at http://mozilla.org/MPL/2.0/. This Source Code Form is
// "Incompatible With Secondary Licenses", as defined by the Mozilla Public
// License, v. 2.0.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "saxonc_export.h"
#include "saxonc/SaxonCProcessor.h"

//===============================================================================================//

EXTERN_SAXONC

/*!    \file   SaxonCXPath.h
    \brief  <code>SaxonCXPath.h</code> provides the C API for XPath processing.
   This file contains a set of functions to compile and execute XPath
   expressions.

*/

/**
 * Create a boxed Boolean value
 * @param environi - GraalVM environment held as the struct sxnc_environment.
 * @param val - boolean value
 */

SAXONC_EXPORT int64_t booleanValue(sxnc_environment *environi, bool val);

/**
 * Create a boxed Integer value
 * @param environi - GraalVM environment held as the struct sxnc_environment.
 * @param i - int value
 */

SAXONC_EXPORT int64_t integerValue(sxnc_environment *environi, int i);

/**
 * Create a boxed Double value
 * @param environi - GraalVM environment held as the struct sxnc_environment.
 * @param d - double value
 */

SAXONC_EXPORT int64_t doubleValue(sxnc_environment *environi, double d);

/**
 * Create a boxed Float value
 * @param environi - GraalVM environment held as the struct sxnc_environment.
 * @param f - float value
 */

SAXONC_EXPORT int64_t floatValue(sxnc_environment *environi, float f);

/**
 * Create a boxed Long value
 * @param environi - GraalVM environment held as the struct sxnc_environment.
 * @param l - long value
 */

SAXONC_EXPORT int64_t longValue(sxnc_environment *environi, long l);

/**
 * Create a boxed String value
 * @param environi - GraalVM environment held as the struct sxnc_environment.
 * @param str - char array value
 */

SAXONC_EXPORT int64_t getJavaStringValue(sxnc_environment *environi, const char *str);

/**
 * A Constructor for internal use. Create an underlying representation for the
 * XdmValue based on the target type. Conversion is applied if possible.
 * @param environi - GraalVM environment held as the struct sxnc_environment.
 * @param type - the target type of the value
 * @param str - value to convert
 */

SAXONC_EXPORT int64_t xdmValueAsObj(sxnc_environment *environi, const char *type,
                      const char *str);

/**
 * A Constructor. Create an XdmValue based on the target type. Conversion is
 * applied if possible.
 * @param environi - GraalVM environment held as the struct sxnc_environment.
 * @param type - the target type of the value
 * @param str - value to convert
 */

SAXONC_EXPORT sxnc_value *xdmValue(sxnc_environment *environi, const char *type,
                     const char *str);

/**
 * Initialise the Saxon Processor and XPath processor.
 * This utility function is used to allocate memory on the heap, therefore is
 * required to be called before execution of XPath.
 *
 * @param environi - GraalVM environment held as the struct sxnc_environment.
 * This function calls malloc on the pointer.
 * @param processor - Pointer to the Saxon Processor which is represented as the
 * struct sxnc_processor and is used for creating the XPath processor. This
 * function calls malloc on the pointer.
 * @param xpath - Pointer to the sxnc_xpath struct which is used for the
 * execution of XPath expressions. This function calls malloc on the pointer
 */
SAXONC_EXPORT int c_createXPathProcessor(sxnc_environment *environi,
                           sxnc_processor *processor, sxnc_xpath *xpath);

/**
 * Compile and evaluate an XPath expression, supplied as a character string,
 * with properties and parameters required by the XPath expression.
 * @param environi - GraalVM environment
 * @param proc - Pointer to the XPath Processor used for creating the XPath
 *executable
 * @param cwd - Current working directory
 * @param xpathStr - A string containing the source text of the XPath expression
 * @param encoding -  the encoding of the string. Accept null. If not specified
 *then the platform default encoding is used.
 * @param parameters - Array of parameters for the XPath expression, as mappings
 *(string, value)
 * @param properties - Array of properties for the XPath expression, as mappings
 *(string, string). This could contain the context node, source as string or
 *file name, etc.
 * @param parLen - The length of the parameters array
 * @param propLen - The length of the properties array
 * @return sxnc_value - The sxnc_value struct encapsulates the returned XdmValue
 *object or NULL if the expression returns an empty sequence.
 **/

SAXONC_EXPORT sxnc_value *evaluate(sxnc_environment *environi, sxnc_xpath *proc, char *cwd,
                     char *xpathStr, char *encoding, sxnc_parameter *parameters,
                     sxnc_property *properties, int parLen, int propLen);

/**
 * Compile and evaluate an XPath expression for which the result is expected to
 *be a single XdmItem or NULL. The XPath expression is supplied as a character
 *string, with properties and parameters required by the XPath expression.
 * @param environi - GraalVM environment
 * @param proc - Pointer to the XPath Processor used for creating the XPath
 *executable
 * @param cwd - Current working directory
 * @param xpathStr - A string containing the source text of the XPath expression
 * @param encoding -  the encoding of the string. Accept null. If not specified
 *then the platform default encoding is used.
 * @param parameters - Array of parameters for the XPath expression, as mappings
 *(string, value)
 * @param properties - Array of properties for the XPath expression, as mappings
 *(string, string). This could contain the context node, source as string or
 *file name, etc.
 * @param parLen - The length of the parameters array
 * @param propLen - The length of the properties array
 * @return sxnc_value - The sxnc_value struct encapsulates the returned XdmItem
 *object or NULL if the expression returns an empty sequence.
 **/
SAXONC_EXPORT sxnc_value *evaluateSingle(sxnc_environment *environi, sxnc_xpath *proc,
                           char *cwd, char *xpathStr, char *encoding,
                           sxnc_parameter *parameters,
                           sxnc_property *properties, int parLen, int propLen);

/**
 * Evaluate an XPath expression, returning the effective boolean value of the
 *result.
 *
 * @param environi - GraalVM environment
 * @param proc - Pointer to the XPath Processor used for creating the XPath
 *executable
 * @param cwd - Current working directory
 * @param xpathStr - A string containing the source text of the XPath expression
 * @param encoding -  the encoding of the string. Accept null. If not specified
 *then the platform default encoding is used.
 * @param parameters - Array of parameters for the XPath expression, as mappings
 *(string, value)
 * @param properties - Array of properties for the XPath expression, as mappings
 *(string, string). This could contain the context node, source as string or
 *file name, etc.
 * @param parLen - The length of the parameters array
 * @param propLen - The length of the properties array
 *
 **/

SAXONC_EXPORT bool effectiveBooleanValue(sxnc_environment *environi, sxnc_xpath *proc,
                           char *cwd, char *xpathStr, char *encoding,
                           sxnc_parameter *parameters,
                           sxnc_property *properties, int parLen, int propLen);

/**
 * Determine whether the item is an atomic value or a node
 * @param environi - GraalVM environment held as the struct sxnc_environment.
 * @param value - XDM value
 *
 * @return true if the item is an atomic value, false if it is a node
 */

SAXONC_EXPORT bool isAtomicValue(sxnc_environment *environi, sxnc_value value);

/**
 * Get the number of items in the sequence
 * @param environi - GraalVM environment held as the struct sxnc_environment.
 * @param val - XDM value
 *
 * @return the number of items in the value - always one
 */

SAXONC_EXPORT int size(sxnc_environment *environi, sxnc_value val);

// /**
// * Get the n'th item in the value, counting from zero.
// *
// * @param n - the item that is required, counting the first item in the
// sequence
// * as item zero
// * @return the n'th item in the sequence making up the value, counting from
// zero
// */
// sxnc_value *itemAt(sxnc_environment *environi, sxnc_value, int i);

// /**
// * Convert the Get the n'th item in the value, counting from zero.
// *
// * @param n the item that is required, counting the first item in the sequence
// * as item zero
// * @return the n'th item in the sequence making up the value, counting from
// zero
// */
// int64_t getvalue(sxnc_environment *environi, sxnc_value);

/**
 * Get the string value of the item. For a node, this gets the string value
 * of the node. For an atomic value, it has the same effect as casting the value
 * to a string. In all cases the result is the same as applying the XPath
 * string() function.
 * @param environi - GraalVM environment held as the struct sxnc_environment.
 * @param value - Value to convert to string
 **/

SAXONC_EXPORT const char *getStringValue(sxnc_environment *environi, sxnc_value value);

/**
 * Get the integer value of the item.
 * @param environi - GraalVM environment held as the struct sxnc_environment.
 * @param value - Value to convert to integer
 * @param failure_value - If the value is not an integer then we can specify the
 *value to return. Default is zero.
 *
 **/

SAXONC_EXPORT int getIntegerValue(sxnc_environment *environi, sxnc_value value,
                    int failure_value);

/**
 * Get the boolean value of the item.
 * @param environi - GraalVM environment held as the struct sxnc_environment.
 * @param value - Value to convert to boolean
 *
 **/
SAXONC_EXPORT bool getBooleanValue(sxnc_environment *environi, sxnc_value value);

/**
 * Get the long value of the item.
 * @param environi - GraalVM environment held as the struct sxnc_environment.
 * @param value - Value to convert to long
 * @param failureVal - If the Value is not a long then we can specify the value
 * to return. Default is zero.
 *
 **/
SAXONC_EXPORT long getLongValue(sxnc_environment *environi, sxnc_value value,
                  long failureVal);

/**
 * Get the float value of the item.
 * @param environi - GraalVM environment held as the struct sxnc_environment.
 * @param value - Value to convert to float
 * @param failureVal - If the Value is not a float then we can specify the value
 * to return. Default is zero.
 *
 **/
SAXONC_EXPORT float getFloatValue(sxnc_environment *environi, sxnc_value value,
                    float failureVal);

/**
 * Get the double value of the item.
 * @param environi - GraalVM environment held as the struct sxnc_environment.
 * @param value - Value to convert to double
 * @param failureVal - If the Value is not a double then we can specify the
 *value to return. Default is zero.
 *
 **/
SAXONC_EXPORT double getDoubleValue(sxnc_environment *environi, sxnc_value value,
                      double failureVal);

EXTERN_SAXONC_END

#endif
