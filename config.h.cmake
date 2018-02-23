#cmakedefine HAVE_DLFCN_H @HAVE_STDLIB_H@
#cmakedefine HAVE_GETTIMEOFDAY @HAVE_GETTIMEOFDAY@
#cmakedefine HAVE_INTTYPES_H @HAVE_INTTYPES_H@
#cmakedefine HAVE_JEMALLOC_JEMALLOC_H @HAVE_JEMALLOC_JEMALLOC_H@
#cmakedefine HAVE_MEMORY_H @HAVE_MEMORY_H@
#cmakedefine HAVE_MEMSET @HAVE_MEMSET@
#cmakedefine HAVE_STDBOOL_H @HAVE_STDBOOL_H@
#cmakedefine HAVE_STDINT_H @HAVE_STDINT_H@
#cmakedefine HAVE_STDLIB_H @HAVE_STDLIB_H@
#cmakedefine HAVE_STRCHR @HAVE_STRCHR@
#cmakedefine HAVE_STRDUP @HAVE_STRDUP@
#cmakedefine HAVE_STRINGS_H @HAVE_STRINGS_H@
#cmakedefine HAVE_STRING_H @HAVE_STRING_H@
#cmakedefine HAVE_STRNDUP @HAVE_STRNDUP@
#cmakedefine HAVE_STRSTR @HAVE_STRSTR@
#cmakedefine HAVE_SYS_STAT_H @HAVE_SYS_STAT_H@
#cmakedefine HAVE_SYS_TIME_H @HAVE_SYS_TIME_H@
#cmakedefine HAVE_SYS_TYPES_H @HAVE_SYS_TYPES_H@
#cmakedefine HAVE_UNISTD_H @HAVE_UNISTD_H@
#cmakedefine HAVE__BOOL @HAVE__BOOL@

/* Name of package */
#define PACKAGE "@PROJECT_NAME@"

/* Define to the address where bug reports for this package should be sent. */
#define PACKAGE_BUGREPORT ""

/* Define to the full name of this package. */
#define PACKAGE_NAME "@PROJECT_NAME@"

/* Define to the full name and version of this package. */
#define PACKAGE_STRING "@PROJECT_NAME@ @PROJECT_VERSION@"

/* Define to the one symbol short name of this package. */
#define PACKAGE_TARNAME "@PROJECT_NAME@"

/* Define to the home page for this package. */
#define PACKAGE_URL ""

/* Define to the version of this package. */
#define PACKAGE_VERSION "@PROJECT_VERSION@"

/* Define to 1 if you have the ANSI C header files. */
#define STDC_HEADERS 1

/* Define to 1 if you have the PATH_MAX macro. */
/* #undef USE_JEMALLOC */

/* Enable extensions on AIX 3, Interix.  */
#ifndef _ALL_SOURCE
# define _ALL_SOURCE 1
#endif
/* Enable GNU extensions on systems that have them.  */
#ifndef _GNU_SOURCE
# define _GNU_SOURCE 1
#endif
/* Enable threading extensions on Solaris.  */
#ifndef _POSIX_PTHREAD_SEMANTICS
# define _POSIX_PTHREAD_SEMANTICS 1
#endif
/* Enable extensions on HP NonStop.  */
#ifndef _TANDEM_SOURCE
# define _TANDEM_SOURCE 1
#endif
/* Enable general extensions on Solaris.  */
#ifndef __EXTENSIONS__
# define __EXTENSIONS__ 1
#endif


/* Version number of package */
#define VERSION "@PROJECT_VERSION@"

/* Define to 1 if on MINIX. */
/* #undef _MINIX */

/* Define to 2 if the system does not provide POSIX.1 features except with
   this defined. */
/* #undef _POSIX_1_SOURCE */

/* Define to 1 if you need to in order for `stat' and other things to work. */
/* #undef _POSIX_SOURCE */

/* Define to `__inline__' or `__inline' if that's what the C compiler
   calls it, or to nothing if 'inline' is not supported under any name.  */
#ifndef __cplusplus
/* #undef inline */
#endif

/* Define to `unsigned int' if <sys/types.h> does not define. */
/* #undef size_t */
