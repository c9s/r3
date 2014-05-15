/*
 * token.c
 * Copyright (C) 2014 c9s <c9s@c9smba.local>
 *
 * Distributed under terms of the MIT license.
 */
#include <stdlib.h>
#include "token.h"


token_list * token_list_create(int cap) {
    token_list * list = (token_list*) malloc( sizeof(token_list) );
    list->len = 0;
    list->cap = cap;
    list->tokens = (char**) malloc( sizeof(char*) * cap);
    return list;
}

bool token_list_is_full(token_list * l) {
    return l->len >= l->cap;
}

bool token_list_resize(token_list *l, int new_cap) {
    l->tokens = realloc(l->tokens, sizeof(char**) * new_cap);
    l->cap = new_cap;
    return l->tokens != NULL;
}

bool token_list_append(token_list * l, char * token) {
    if ( token_list_is_full(l) ) {
        bool ret = token_list_resize(l, l->cap + 20);
        if (ret == FALSE ) {
            return FALSE;
        }
    }
    l->tokens[ ++(l->len) ] = token;
    return TRUE;
}

void token_list_free(token_list *l) {
    for ( int i = 0; i < l->len ; i++ ) {
        char * t = l->tokens[ i ];
        free(t);
    }
    free(l);
}



