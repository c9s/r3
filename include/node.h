/*
 * node.h
 * Copyright (C) 2014 c9s <c9s@c9smba.local>
 *
 * Distributed under terms of the MIT license.
 */

#ifndef NODE_H
#define NODE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "token.h"

typedef struct _rnode rnode;

struct _rnode {
    rnode ** children;
    int      children_len;
    int      children_cap;

    /* the combined regexp pattern string from pattern_tokens */
    char* combined_pattern;

    token_array * edge_patterns;
};

rnode * rnode_create(int cap);

void rnode_free(rnode * tree);

bool rnode_add_edge(rnode * n, char * pat , rnode *child);

rnode * rnode_find_edge(rnode * n, char * pat);

void rnode_append_child(rnode *n, rnode *child);




void rnode_append_route(rnode * tree, token_array * token);


#endif /* !NODE_H */
