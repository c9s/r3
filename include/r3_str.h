/*
 * r3_str.h
 * Copyright (C) 2014 c9s <c9s@c9smba.local>
 *
 * Distributed under terms of the MIT license.
 */
#ifndef STR_H
#define STR_H

#include "r3_define.h"
#include "config.h"

int slug_count(char * p, int len);

char * slug_compile(char * str, int len);

bool contains_slug(char * str);

char * find_slug_pattern(char *s1, int *len);

char * find_slug_placeholder(char *s1, int *len);

char * inside_slug(char * needle, int needle_len, char *offset);

char * ltrim_slash(char* str);

char** str_split(char* a_str, const char a_delim);

void str_repeat(char *s, char *c, int len);

void print_indent(int level);

#ifndef HAVE_STRDUP
char *strdup(const char *s);
#endif

#ifndef HAVE_STRNDUP
char *strndup(const char *s, int n);
#endif


#endif /* !STR_H */

