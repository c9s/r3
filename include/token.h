/*
 * token.h
 * Copyright (C) 2014 c9s <c9s@c9smba.local>
 *
 * Distributed under terms of the MIT license.
 */

#ifndef TOKEN_H
#define TOKEN_H

#include "define.h"

typedef struct _token_array {
  char **tokens;
  int    len;
  int    cap;
} token_array;

token_array * token_array_create(int cap);


bool token_array_is_full(token_array * l);

bool token_array_resize(token_array *l, int new_cap);

bool token_array_append(token_array * list, char * token);

void token_array_free(token_array *l);

void token_array_dump(token_array *l);

token_array * split_route_pattern(char *pattern, int pattern_len);

#define token_array_fetch(t,i)  t->tokens[i]
#define token_array_len(t)  t->len
#define token_array_cap(t)  t->cap

#endif /* !TOKEN_H */
