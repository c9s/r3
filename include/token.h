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

typedef struct _token_list {
  char **tokens;
  int    len;
  int    cap;
} token_list;

token_list * token_list_create(int cap);


bool token_list_is_full(token_list * l);

bool token_list_resize(token_list *l, int new_cap);

bool token_list_append(token_list * list, char * token);

void token_list_free(token_list *l);

#endif /* !TOKEN_H */
