#ifndef R3_STR_INTERN_H
#define R3_STR_INTERN_H

#ifdef __cplusplus
extern "C" {
#endif


#ifndef HAVE_STRNDUP
char *strndup(const char *s, int n);
#endif


#ifndef HAVE_ASPRINTF

#include <stdarg.h>

/**
 * Sets `char **' pointer to be a buffer
 * large enough to hold the formatted string
 * accepting a `va_list' args of variadic
 * arguments.
 */

int vasprintf (char **, const char *, va_list);

/**
 * Sets `char **' pointer to be a buffer
 * large enough to hold the formatted
 * string accepting `n' arguments of
 * variadic arguments.
 */

int asprintf (char **, const char *, ...);
#endif

void print_indent(int level);

#ifdef __cplusplus
}
#endif

#endif
