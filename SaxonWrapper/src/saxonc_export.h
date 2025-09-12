
#ifndef SAXONC_EXPORT_H
#define SAXONC_EXPORT_H

#ifdef SAXONC_STATIC_DEFINE
#  define SAXONC_EXPORT
#  define SAXONC_NO_EXPORT
#else
#  ifndef SAXONC_EXPORT
#    ifdef saxonc_he_EXPORTS
        /* We are building this library */
#      define SAXONC_EXPORT __declspec(dllexport)
#    else
        /* We are using this library */
#      define SAXONC_EXPORT __declspec(dllimport)
#    endif
#  endif

#  ifndef SAXONC_NO_EXPORT
#    define SAXONC_NO_EXPORT 
#  endif
#endif

#ifndef SAXONC_DEPRECATED
#  define SAXONC_DEPRECATED __declspec(deprecated)
#endif

#ifndef SAXONC_DEPRECATED_EXPORT
#  define SAXONC_DEPRECATED_EXPORT SAXONC_EXPORT SAXONC_DEPRECATED
#endif

#ifndef SAXONC_DEPRECATED_NO_EXPORT
#  define SAXONC_DEPRECATED_NO_EXPORT SAXONC_NO_EXPORT SAXONC_DEPRECATED
#endif

/* NOLINTNEXTLINE(readability-avoid-unconditional-preprocessor-if) */
#if 0 /* DEFINE_NO_DEPRECATED */
#  ifndef SAXONC_NO_DEPRECATED
#    define SAXONC_NO_DEPRECATED
#  endif
#endif

#endif /* SAXONC_EXPORT_H */
