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
#include <jemalloc/jemalloc.h>

// PCRE
#include <pcre.h>

// Judy array
// #include <Judy.h>
#include <config.h>

#include "r3_define.h"
#include "r3_str.h"
#include "r3.h"
#include "str_array.h"



/**
 * branch the edge pattern at "dl" offset
 *
 */
void r3_edge_branch(edge *e, int dl) {
    node *c1; // child 1, child 2
    edge *e1; // edge 1, edge 2
    char * s1 = e->pattern + dl;
    int s1_len = 0;

    edge **tmp_edges = e->child->edges;
    int   tmp_r3_edge_len = e->child->edge_len;

    // the suffix edge of the leaf
    c1 = r3_tree_create(3);
    s1_len = e->pattern_len - dl;
    e1 = r3_edge_create(my_strndup(s1, s1_len), s1_len, c1);
    // printf("edge left: %s\n", e1->pattern);

    // Migrate the child edges to the new edge we just created.
    for ( int i = 0 ; i < tmp_r3_edge_len ; i++ ) {
        r3_tree_append_edge(c1, tmp_edges[i]);
        e->child->edges[i] = NULL;
    }
    e->child->edge_len = 0;
    e->child->endpoint--;

    info("branched pattern: %s\n", e1->pattern);

    r3_tree_append_edge(e->child, e1);
    c1->endpoint++;
}


edge * r3_edge_create(char * pattern, int pattern_len, node * child) {
    edge * e = (edge*) malloc( sizeof(edge) );
    e->pattern = pattern;
    e->pattern_len = pattern_len;
    e->child = child;
    return e;
}

void r3_edge_free(edge * e) {
    if (e->pattern) {
        free(e->pattern);
    }
    if ( e->child ) {
        r3_tree_free(e->child);
    }
}

