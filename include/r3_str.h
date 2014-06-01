/*
 * r3_str.h
 * Copyright (C) 2014 c9s <c9s@c9smba.local>
 *
 * Distributed under terms of the MIT license.
 */
#ifndef STR_H
#define STR_H

#include "r3.h"
#include "config.h"

int slug_count(const char * p, int len, char ** errstr);

char * slug_compile(const char * str, int len);

char * slug_find_pattern(const char *s1, int *len);

char * slug_find_placeholder(const char *s1, int *len);

char * inside_slug(const char * needle, int needle_len, char *offset, char **errstr);

char * ltrim_slash(char* str);

void str_repeat(char *s, const char *c, int len);

void print_indent(int level);

#ifndef HAVE_STRDUP
char *strdup(const char *s);
#endif

#ifndef HAVE_STRNDUP
char *strndup(const char *s, int n);
#endif


#endif /* !STR_H */

