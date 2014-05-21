/*
 * edge.c
 * Copyright (C) 2014 c9s <c9s@c9smba.local>
 *
 * Distributed under terms of the MIT license.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

// Jemalloc memory management
// #include <jemalloc/jemalloc.h>

// PCRE
#include <pcre.h>

// Judy array
// #include <Judy.h>
#include <config.h>

#include "r3.h"
#include "r3_str.h"
#include "str_array.h"
#include "zmalloc.h"

edge * r3_edge_create(char * pattern, int pattern_len, node * child) {
    edge * e = (edge*) zmalloc( sizeof(edge) );
    e->pattern = pattern;
    e->pattern_len = pattern_len;
    e->child = child;
    return e;
}



/**
 * branch the edge pattern at "dl" offset,
 * insert a dummy child between the edges.
 *
 *
 * A -> [prefix..suffix] -> B
 * A -> [prefix] -> B -> [suffix] -> New Child (Copy Data, Edges from B)
 *
 */
node * r3_edge_branch(edge *e, int dl) {
    node *new_child;
    edge *e1;
    char * s1 = e->pattern + dl;
    int s1_len = 0;

    edge **tmp_edges = e->child->edges;
    int   tmp_edge_len = e->child->edge_len;

    // the suffix edge of the leaf
    new_child = r3_tree_create(3);
    s1_len = e->pattern_len - dl;
    e1 = r3_edge_create(zstrndup(s1, s1_len), s1_len, new_child);

    // Migrate the child edges to the new edge we just created.
    for ( int i = 0 ; i < tmp_edge_len ; i++ ) {
        r3_node_append_edge(new_child, tmp_edges[i]);
        e->child->edges[i] = NULL;
    }
    e->child->edge_len = 0;
    new_child->endpoint = e->child->endpoint;
    e->child->endpoint = 0; // reset endpoint

    r3_node_append_edge(e->child, e1);
    new_child->data = e->child->data; // copy data pointer
    e->child->data = NULL;

    // truncate the original edge pattern
    char *op = e->pattern;
    e->pattern = zstrndup(e->pattern, dl);
    e->pattern_len = dl;

    return new_child;
}

void r3_edge_free(edge * e) {
    if (e->pattern) {
        zfree(e->pattern);
    }
    if ( e->child ) {
        r3_tree_free(e->child);
    }
    // free itself
    zfree(e);
}

