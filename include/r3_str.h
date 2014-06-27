/*
 * r3_str.h
 * Copyright (C) 2014 c9s <yoanlin93@gmail.com>
 *
 * Distributed under terms of the MIT license.
 */
#ifndef STR_H
#define STR_H

#include "r3.h"
#include "config.h"

char * r3_slug_compile(const char * str, int len);

char * r3_slug_find_pattern(const char *s1, int *len);

char * r3_slug_find_placeholder(const char *s1, int *len);

char * r3_inside_slug(const char * needle, int needle_len, char *offset, char **errstr);

void str_repeat(char *s, const char *c, int len);

void print_indent(int level);


#if _GNU_SOURCE || POSIX_C_SOURCE >= 200809L || _XOPEN_SOURCE >= 700 || __DARWIN_C_LEVEL >= 200809L
#ifndef HAVE_STRNDUP
#define HAVE_STRNDUP
#endif
#endif

#if _SVID_SOURCE || _BSD_SOURCE \
    || _XOPEN_SOURCE >= 500 \
    || _XOPEN_SOURCE && _XOPEN_SOURCE_EXTENDED \
    || /* Since glibc 2.12: */ _POSIX_C_SOURCE >= 200809L \
    || __DARWIN_C_LEVEL >= 200809L
#ifndef HAVE_STRDUP
#define HAVE_STRDUP
#endif
#endif

#ifndef HAVE_STRDUP
char *strdup(const char *s);
#endif

#ifndef HAVE_STRNDUP
char *strndup(const char *s, int n);
#endif


#endif /* !STR_H */

