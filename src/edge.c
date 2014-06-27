/*
 * edge.c
 * Copyright (C) 2014 c9s <yoanlin93@gmail.com>
 *
 * Distributed under terms of the MIT license.
 */
#include "config.h"
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
#include "r3.h"
#include "r3_str.h"
#include "slug.h"
#include "zmalloc.h"


#define CHECK_PTR(ptr) if (ptr == NULL) return NULL;


edge * r3_edge_createl(const char * pattern, int pattern_len, node * child) {
    edge * e = (edge*) zmalloc( sizeof(edge) );

    CHECK_PTR(e);

    e->pattern = (char*) pattern;
    e->pattern_len = pattern_len;
    e->opcode = 0;
    e->child = child;
    e->has_slug = r3_path_contains_slug_char(e->pattern);
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

    // the suffix edge of the leaf
    new_child = r3_tree_create(3);
    s1_len = e->pattern_len - dl;
    e1 = r3_edge_createl(zstrndup(s1, s1_len), s1_len, new_child);

    // Migrate the child edges to the new edge we just created.
    for ( int i = 0 ; i < e->child->edge_len ; i++ ) {
        r3_node_append_edge(new_child, e->child->edges[i]);
        e->child->edges[i] = NULL;
    }
    e->child->edge_len = 0;


    // Migrate the child routes
    for ( int i = 0 ; i < e->child->route_len ; i++ ) {
        r3_node_append_route(new_child, e->child->routes[i]);
        e->child->routes[i] = NULL;
    }
    e->child->route_len = 0;

    // Migrate the endpoint
    new_child->endpoint = e->child->endpoint;
    e->child->endpoint = 0; // reset endpoint

    // Migrate the data
    new_child->data = e->child->data; // copy data pointer
    e->child->data = NULL;

    r3_node_append_edge(e->child, e1);

    // truncate the original edge pattern
    char *oldpattern = e->pattern;
    e->pattern = zstrndup(e->pattern, dl);
    e->pattern_len = dl;
    zfree(oldpattern);

    return new_child;
}

void r3_edge_free(edge * e) {
    zfree(e->pattern);
    if ( e->child ) {
        r3_tree_free(e->child);
    }
    // free itself
    zfree(e);
}

