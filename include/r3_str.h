/*
 * r3_str.h
 * Copyright (C) 2014 c9s <c9s@c9smba.local>
 *
 * Distributed under terms of the MIT license.
 */
#ifndef STR_H
#define STR_H

#include "r3.h"
#include "sds.h"
#include "config.h"

int slug_count(char * p, int len);

char * slug_compile(char * str, int len);

bool contains_slug(char * str);

char * slug_find_pattern(char *s1, int *len);

char * slug_find_placeholder(char *s1, int *len);

char * inside_slug(char * needle, int needle_len, char *offset);

char * ltrim_slash(char* str);

void str_repeat(char *s, char *c, int len);

void print_indent(int level);

sds concat_indent(sds s, int level);

#ifndef HAVE_STRDUP
char *strdup(const char *s);
#endif

#ifndef HAVE_STRNDUP
char *strndup(const char *s, int n);
#endif


#endif /* !STR_H */

