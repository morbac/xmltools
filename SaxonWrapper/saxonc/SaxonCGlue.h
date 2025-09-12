////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2022 - 2023 Saxonica Limited.
// This Source Code Form is subject to the terms of the Mozilla Public License,
// v. 2.0. If a copy of the MPL was not distributed with this file, You can
// obtain one at http://mozilla.org/MPL/2.0/. This Source Code Form is
// "Incompatible With Secondary Licenses", as defined by the Mozilla Public
// License, v. 2.0.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef SAXONCGLUE2_H
#define SAXONCGLUE2_H

#include "saxonc_export.h"
#include <saxonc/saxonc-core.h>

#include <stdint.h>
#include <stdio.h>
#if defined __linux__ || defined __APPLE__
#include <stdlib.h>
#include <string.h>
#else
#include <windows.h>
#endif

#ifdef __cplusplus
#define EXTERN_SAXONC extern "C" {
#define EXTERN_SAXONC_END }
#else
#define EXTERN_SAXONC
#define EXTERN_SAXONC_END

#endif

#ifndef __cplusplus
#ifndef _BOOL
#include <stdbool.h>
/*typedef unsigned char sxn_bool;
static const sxn_bool sxn_false = 0;
static const sxn_bool sxn_true = 1;*/

#else
// typedef bool sxn_bool;
static const bool false = 0;
static const bool true = 1;
#endif
#endif

#ifndef __cplusplus
#if defined(LICENSE)
#define sxn_license 1
#else
#define sxn_license 0
#endif
#endif

static const int SXN_EXCEPTION = -2;
static const int SXN_UNSET = -1;

/*!  \file  SaxonCGlue.h
 *   \brief This C header file contains a number of factory functions for
 * running SaxonC C/C++ APIs, including GraalVM create and destroy functions.
 */

//#define DEBUG

EXTERN_SAXONC

extern char *dllname;

extern char *resources_dir;

// Static variable used to track when jvm has been created. Used to prevent
// creation more than once.
extern int jvmCreated; // TODO this might not be required in graalvm - remove

//===============================================================================================//
/** <code>sxnc_environment</code>. This struct captures the GraalVM properties
 * associated with the compiled SaxonC library. <p/>
 */
typedef struct {
  graal_isolate_t *isolate; //!< Structure representing a GraalVM isolate
  graal_isolatethread_t *thread; //!< pointer to the current GraalVM isolate thread
  graal_isolatethread_t *mainthread; //!< pointer to the main GraalVM isolate thread
} sxnc_environment;

//===============================================================================================//

/** <code>sxnc_parameter</code>. This struct captures details of parameters used
 * for transformations, queries, and XPath expressions, as (string, value)
 * pairs. <p/>
 */
typedef struct {
  char *name; //!< The parameter name
  int64_t value; /*!< Internal use only. The value of the parameter points to
                    the Java object. */

} sxnc_parameter;

//===============================================================================================//

/** <code>sxnc_property</code>. This struct captures details of properties used
 * for transformations, queries, and XPath expressions, as (string, string)
 * pairs. <p/>
 */
typedef struct {
  char *name; //!< The property name
  char *value; /*!< Internal use only. The value of the parameter points to the
                  Java object. */
} sxnc_property;

/**! <code>sxnc_value</code>. This struct is used to capture the XdmValue and
 * its type information. <p/>
 */
typedef struct {
  int64_t xdmvalue; /*!< Internal use only. The value given as a Java object. */
} sxnc_value;

/**! <code>sxnc_processor</code>. This struct is used to capture the Java
 * processor objects created in C for XSLT, XQuery and XPath. <p/>
 */
typedef struct {
  int64_t value; //!< The ObjectHandle of the processor object
} sxnc_processor;

extern int64_t cpp;

typedef void *(*allocFn)(size_t length);

extern const char *failure;

/**
 * Get resources directory.
 */

char *_getResourceDirectory(void);

/**
 * Load dll using the default setting in SaxonC.
 * Recommended method to use to load library.
 */
SAXONC_EXPORT int create_graalvm_isolate(sxnc_environment *env);

/**
 * Attach to the current thread in the loaded GraalVM isolate
 */
SAXONC_EXPORT int attach_graalvm_thread(sxnc_environment *env);

/**
 * Closes down the GraalVM environment used by SaxonC
 */
SAXONC_EXPORT int detach_graalvm_thread(sxnc_environment *env);

/**
 * Create and return the ObjectHandle for a Saxon Processor. Here we pass in the Graal isolate
 * pointer which should have been attached to in advance.
 *
 * \param *thread [in] The Graal Isolate thread
 * \param license [in] Whether the processor should be licensed (EE/PE) or not (HE).
 */
SAXONC_EXPORT int64_t createSaxonProcessor2(graal_isolatethread_t *thread, int license);

/**
 * Create a pointer to the Saxon Processor. Here we pass in the processor
 * pointer which should have been allocated memory space in advance. Return
 * status true = success, false = failed to create Saxon Processor.
 */
SAXONC_EXPORT int c_createSaxonProcessor(sxnc_environment *environi,
                           sxnc_processor *processor, int license);

/**
 * Callback to check for exceptions. When called it returns the exception as a
 * string.
 */
SAXONC_EXPORT const char *checkForException(sxnc_environment *environi);

/**
 * Clean up and destroy GraalVM to release memory used.
 */
SAXONC_EXPORT void graal_tear_down(graal_isolatethread_t *thread);

/**
 * Get a parameter from the list.
 */
SAXONC_EXPORT int64_t getParameter(sxnc_parameter *parameters, int parLen, const char *name);

/**
 * Get a property from the list.
 */
SAXONC_EXPORT char *getProperty(sxnc_property *properties, int propLen, const char *name);

/**
 * Set a parameter.
 */
SAXONC_EXPORT void setParameter(sxnc_parameter **parameters, int *parLen, int *parCap,
                  const char *name, sxnc_value *value);

/**
 * Set a property.
 */
SAXONC_EXPORT void setProperty(sxnc_property **properties, int *propLen, int *propCap,
                 const char *name, const char *value);

/**
 * Clear parameters and properties.
 */
SAXONC_EXPORT void clearSettings(sxnc_parameter **parameters, int *parLen,
                   sxnc_property **properties, int *propLen);

EXTERN_SAXONC_END

#endif // SAXONCGLUE_H
