//
// Created by Matthew Patterson on 23/01/2025.
//
#include "saxonc_export.h"

#ifndef SAXONC_VERSION_H
#define SAXONC_VERSION_H

#ifdef __cplusplus
#define EXTERN_SAXONC extern "C" {
#define EXTERN_SAXONC_END }
#else
#define EXTERN_SAXONC
#define EXTERN_SAXONC_END
#endif

EXTERN_SAXONC

SAXONC_EXPORT char*    get_saxonc_version();
SAXONC_EXPORT unsigned get_saxonc_version_major();
SAXONC_EXPORT unsigned get_saxonc_version_minor();
SAXONC_EXPORT unsigned get_saxonc_version_patch();
SAXONC_EXPORT unsigned get_saxonc_version_tweak();

EXTERN_SAXONC_END

#endif //SAXONC_VERSION_H
