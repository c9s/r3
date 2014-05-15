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
    l->tokens[ l->len++ ] = token;
    return TRUE;
}

void token_array_dump(token_array *l) {
    printf("[");
    for ( int i = 0; i < l->len ; i++ ) {
        printf("\"%s\"", l->tokens[i] );
        if ( i + 1 != l->len ) {
            printf(", ");
        }
    }
    printf("]\n");
}

void token_array_free(token_array *l) {
    for ( int i = 0; i < l->len ; i++ ) {
        char * t = l->tokens[ i ];
        free(t);
    }
    free(l);
}



/**
 * This function is used to split route path into a string array, not for performance.
 * hence this function should be safe.
 *
 * Split "/path/foo/{id}" into [ "/path" , "/foo" , "/{id}" ]
 * Split "/path/bar/{id}" into [ "/path" , "/foo" , "/{id}" ]
 * Split "/blog/post/{id}" into [ "/blog" , "/post" , "/{id}" ]
 * Split "/blog/{id}" into [ "/blog" , "/{id}" ]
 * Split "/blog" into [ "/blog" ]
 * Split "/b" into [ "/b" ]
 * Split "/{id}" into [ "/{id}" ]
 *
 * @param char* pattern
 * @param int   pattern_len
 *
 * @return char**
 */
token_array * split_route_pattern(char *pattern, int pattern_len) {
    char *s1, *p = pattern;

    token_array * token_array = token_array_create( 20 );

    s1 = p;
    p++;
    while (*p && (p - pattern) < pattern_len ) {

        // a slug
        if ( *p == '{' ) {
            // find closing '}'
            while (*p != '}') {
                p++;
                assert(p - pattern < pattern_len ); // throw exception
            }
            p++; // contains the '}'
            // printf("==> %s\n", strndup(s1, p-s1) );
            token_array_append(token_array, strndup(s1, p-s1) );
            s1 = p;
            continue;
        }
        else if ( *p == '/' ) {
            // printf("==> %s\n", strndup(s1, p-s1) );
            token_array_append(token_array, strndup(s1, p-s1) );
            s1 = p;
        }
        p++;
    }

    if ( p-s1 > 0 ) {
        token_array_append(token_array, strndup(s1, p-s1) );
    }
    return token_array;
}
