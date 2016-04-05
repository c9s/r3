/*
 * match_entry.c
 * Copyright (C) 2014 c9s <yoanlin93@gmail.com>
 *
 * Distributed under terms of the MIT license.
 */
#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pcre.h>
#include <assert.h>
#include <stdbool.h>

#include "r3.h"
#include "zmalloc.h"

match_entry * match_entry_createl(const char * path, int path_len) {
    match_entry * entry = r3_mem_alloc( sizeof(match_entry) );
    memset(entry, 0, sizeof(*entry));
    r3_vector_reserve(NULL, &entry->vars.tokens, 3);
    entry->path.base = path;
    entry->path.len = path_len;
    return entry;
}

void match_entry_free(match_entry * entry) {
    assert(entry);
    free(entry->vars.tokens.entries);
    free(entry);
}
