/*
 * token.c
 * Copyright (C) 2014 c9s <c9s@c9smba.local>
 *
 * Distributed under terms of the MIT license.
 */
#include <stdlib.h>
#include "token.h"


token_array * token_array_create(int cap) {
    token_array * list = (token_array*) malloc( sizeof(token_array) );
    list->len = 0;
    list->cap = cap;
    list->tokens = (char**) malloc( sizeof(char*) * cap);
    return list;
}

bool token_array_is_full(token_array * l) {
    return l->len >= l->cap;
}

bool token_array_resize(token_array *l, int new_cap) {
    l->tokens = realloc(l->tokens, sizeof(char**) * new_cap);
    l->cap = new_cap;
    return l->tokens != NULL;
}

bool token_array_append(token_array * l, char * token) {
    if ( token_array_is_full(l) ) {
        bool ret = token_array_resize(l, l->cap + 20);
        if (ret == FALSE ) {
            return FALSE;
        }
    }
    l->tokens[ ++(l->len) ] = token;
    return TRUE;
}

void token_array_free(token_array *l) {
    for ( int i = 0; i < l->len ; i++ ) {
        char * t = l->tokens[ i ];
        free(t);
    }
    free(l);
}


