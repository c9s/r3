/*
 * token.h
 * Copyright (C) 2014 c9s <c9s@c9smba.local>
 *
 * Distributed under terms of the MIT license.
 */

#ifndef TOKEN_H
#define TOKEN_H

typedef unsigned char bool;
#define FALSE 0
#define TRUE 1

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

#endif /* !TOKEN_H */
