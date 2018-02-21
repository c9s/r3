/*
 * str_array.h
 * Copyright (C) 2014 c9s <yoanlin93@gmail.com>
 *
 * Distributed under terms of the MIT license.
 */

#ifndef STR_ARRAY_H
#define STR_ARRAY_H

#include "r3.h"
#include "memory.h"

typedef struct _str_array {
  R3_VECTOR(r3_iovec_t) slugs;
  R3_VECTOR(r3_iovec_t) tokens;
} str_array;

// str_array * str_array_create(int cap);

bool str_array_slugs_full(const str_array * l);

// bool str_array_tokens_full(const str_array * l);

// bool str_array_resize(str_array *l, int new_cap);

// bool str_array_append_slug(str_array * l, char * slug);

bool str_array_append(str_array * l, const char * token, unsigned int len);

void str_array_free(str_array *l);

void str_array_dump_slugs(const str_array *l);

void str_array_dump(const str_array *l);

str_array * split_route_pattern(char *pattern, int pattern_len);

#define str_array_fetch(t,i)  t->tokens[i]
#define str_array_len(t)  t->len
#define str_array_cap(t)  t->cap

#endif /* !STR_ARRAY_H */
