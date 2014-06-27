/*
 * token.c
 * Copyright (C) 2014 c9s <yoanlin93@gmail.com>
 *
 * Distributed under terms of the MIT license.
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "r3.h"
#include "r3_str.h"
#include "str_array.h"
#include "zmalloc.h"

str_array * str_array_create(int cap) {
    str_array * list = (str_array*) zmalloc( sizeof(str_array) );
    if (!list)
        return NULL;
    list->len = 0;
    list->cap = cap;
    list->tokens = (char**) zmalloc( sizeof(char*) * cap);
    return list;
}

void str_array_free(str_array *l) {
    assert(l);
    for ( int i = 0; i < l->len ; i++ ) {
        if (l->tokens[ i ]) {
            zfree(l->tokens[i]);
        }
    }
    zfree(l);
}

bool str_array_is_full(const str_array * l) {
    return l->len >= l->cap;
}

bool str_array_resize(str_array * l, int new_cap) {
    l->tokens = zrealloc(l->tokens, sizeof(char**) * new_cap);
    l->cap = new_cap;
    return l->tokens != NULL;
}

bool str_array_append(str_array * l, char * token) {
    if ( str_array_is_full(l) ) {
        bool ret = str_array_resize(l, l->cap + 20);
        if (ret == FALSE ) {
            return FALSE;
        }
    }
    l->tokens[ l->len++ ] = token;
    return TRUE;
}

void str_array_dump(const str_array *l) {
    printf("[");
    for ( int i = 0; i < l->len ; i++ ) {
        printf("\"%s\"", l->tokens[i] );
        if ( i + 1 != l->len ) {
            printf(", ");
        }
    }
    printf("]\n");
}




