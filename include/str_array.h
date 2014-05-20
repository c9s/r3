/*
 * str_array.h
 * Copyright (C) 2014 c9s <c9s@c9smba.local>
 *
 * Distributed under terms of the MIT license.
 */

#ifndef TOKEN_H
#define TOKEN_H

typedef struct _str_array {
  char **tokens;
  int    len;
  int    cap;
} str_array;

str_array * str_array_create(int cap);

bool str_array_is_full(str_array * l);

bool str_array_resize(str_array *l, int new_cap);

bool str_array_append(str_array * list, char * token);

void str_array_free(str_array *l);

void str_array_dump(str_array *l);

str_array * split_route_pattern(char *pattern, int pattern_len);

#define str_array_fetch(t,i)  t->tokens[i]
#define str_array_len(t)  t->len
#define str_array_cap(t)  t->cap

#endif /* !TOKEN_H */
