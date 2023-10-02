/*
 * str_array.h
 * Copyright (C) 2014 c9s <yoanlin93@gmail.com>
 *
 * Distributed under terms of the MIT license.
 */

#ifndef STR_ARRAY_H
#define STR_ARRAY_H

#include "memory.h"

#if __STDC_VERSION__ <= 201710L
#ifdef HAVE_STDBOOL_H
#  include <stdbool.h>
#elif !defined(bool) && !defined(__cplusplus)
typedef unsigned char bool;
#  define bool bool /* For redefinition guards */
#  define false 0
#  define true 1
#endif
#endif

typedef struct _str_array {
  R3_VECTOR(r3_iovec_t) slugs;
  R3_VECTOR(r3_iovec_t) tokens;
} str_array;

bool str_array_append(str_array * l, const char * token, unsigned int len);

void str_array_free(str_array *l);

void str_array_dump_slugs(const str_array *l);

void str_array_dump(const str_array *l);

#define str_array_fetch(t,i)  t->tokens.entries[i]
#define str_array_len(t)  t->tokens.size
#define str_array_cap(t)  t->tokens.capacity

#endif /* !STR_ARRAY_H */
