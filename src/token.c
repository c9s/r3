/*
 * token.c
 * Copyright (C) 2014 c9s <yoanlin93@gmail.com>
 *
 * Distributed under terms of the MIT license.
 */
#include "config.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "r3.h"
#include "r3_slug.h"
#include "str_array.h"
#include "zmalloc.h"
#include "memory.h"

void str_array_free(str_array *l) {
    assert(l);
    free(l->tokens.entries);
}

bool str_array_append(str_array * l, char * token, unsigned int len) {
    R3_VECTOR(r3_iovec_t) *tks = &l->tokens;
    r3_vector_reserve(NULL, tks, tks->size + 1);
    r3_iovec_t *temp = tks->entries + tks->size++;
    memset(temp, 0, sizeof(*temp));
    temp->base = token;
    temp->len = len;
    return true;
}

void str_array_dump_slugs(const str_array *l) {
    if (l->tokens.size) {
        printf("[");
        for ( int i = 0; i < l->tokens.size ; i++ ) {
            printf("\"%*.*s\"", l->slugs.entries[i].len,l->slugs.entries[i].len,l->slugs.entries[i].base );
            if ( i + 1 != l->tokens.size ) {
                printf(", ");
            }
        }
        printf("]\n");
    } else {
        printf("[]\n");
    }
}

void str_array_dump(const str_array *l) {
    printf("[");
    for ( int i = 0; i < l->tokens.size ; i++ ) {
        printf("\"%*.*s\"", l->tokens.entries[i].len,l->tokens.entries[i].len,l->tokens.entries[i].base );
        // printf("\"%s\"", l->tokens.entries[i] );
        if ( i + 1 != l->tokens.size ) {
            printf(", ");
        }
    }
    printf("]\n");
}




