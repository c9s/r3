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
#include "r3.h"
#include "r3_str.h"
#include "slug.h"
#include "zmalloc.h"


#define CHECK_PTR(ptr) if (ptr == NULL) return NULL;



void r3_edge_initl(edge *e, const char * pattern, int pattern_len, node * child)
{
    e->pattern = (char*) pattern;
    e->pattern_len = pattern_len;
    e->opcode = 0;
    e->child = child;
    e->has_slug = r3_path_contains_slug_char(e->pattern);
}

edge * r3_edge_createl(const char * pattern, int pattern_len, node * child)
{
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
 * r3_edge_branch splits the edge and append the rest part as the child of the
 * first level child
 *
 * branch the edge pattern at "dl" offset,
 * and insert a dummy child between the edges.
 *
 * A -> [EDGE: abcdefg] -> B -> [EDGE:branch1], [EDGE:branch2]
 * A -> [EDGE: abcd] -> B1 -> [efg] -> B2 (new child with copied data from B)
 *
 */
node * r3_edge_branch(edge *e, int dl) {
    node * new_child;
    edge * new_edge;

    // the rest string
    char * s1 = e->pattern + dl;
    int s1_len = e->pattern_len - dl;

    // the suffix edge of the leaf
    new_child = r3_tree_create(3);
    new_edge = r3_edge_createl(zstrndup(s1, s1_len), s1_len, new_child);

    // Move child node to the new edge
    new_edge->child = e->child;
    e->child = new_child;

    r3_node_append_edge(new_child, new_edge);

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

