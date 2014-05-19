/*
 * token.c
 * Copyright (C) 2014 c9s <c9s@c9smba.local>
 *
 * Distributed under terms of the MIT license.
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "str_array.h"
#include "r3_str.h"


str_array * str_array_create(int cap)
{
    str_array * list = (str_array*) malloc( sizeof(str_array) );
    list->len = 0;
    list->cap = cap;
    list->tokens = (char**) malloc( sizeof(char*) * cap);
    return list;
}

void str_array_free(str_array *l)
{
    for ( int i = 0; i < l->len ; i++ ) {
        char * t = l->tokens[ i ];
        free(t);
    }
    free(l);
}

bool str_array_is_full(str_array * l)
{
    return l->len >= l->cap;
}

bool str_array_resize(str_array *l, int new_cap)
{
    l->tokens = realloc(l->tokens, sizeof(char**) * new_cap);
    l->cap = new_cap;
    return l->tokens != NULL;
}

bool str_array_append(str_array * l, char * token)
{
    if ( str_array_is_full(l) ) {
        bool ret = str_array_resize(l, l->cap + 20);
        if (ret == FALSE ) {
            return FALSE;
        }
    }
    l->tokens[ l->len++ ] = token;
    return TRUE;
}

void str_array_dump(str_array *l)
{
    printf("[");
    for ( int i = 0; i < l->len ; i++ ) {
        printf("\"%s\"", l->tokens[i] );
        if ( i + 1 != l->len ) {
            printf(", ");
        }
    }
    printf("]\n");
}




