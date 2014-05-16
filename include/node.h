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
#include <pcre.h>

#include "token.h"

struct _edge;
struct _node;
typedef struct _edge edge;
typedef struct _node node;

struct _node {
    edge ** edges;
    int      edge_len;
    int      edge_cap;

    /* the combined regexp pattern string from pattern_tokens */
    char * combined_pattern;
    int    combined_pattern_len;
    pcre * pcre_pattern;
    pcre_extra * pcre_extra;

    /**
     * the pointer of route structure
     */
    void * route_ptr;

    int endpoint;
};

struct _edge {
    char * pattern;
    int    pattern_len;
    bool   has_slug;
    node * child;
};

typedef struct {
    char ** vars;
    int     vars_len;
    char * path; // dispatched path
    void * route_ptr; // route ptr
} match_entry;


node * rtree_create(int cap);

node * node_create();

void rtree_free(node * tree);

void edge_free(edge * edge);

edge * rtree_add_child(node * n, char * pat , node *child);

edge * node_find_edge(node * n, char * pat);

void rtree_append_edge(node *n, edge *child);

node * rtree_insert_path(node *tree, char *route, void * route_ptr);

node * rtree_insert_pathn(node *tree, char *route, int route_len, void * route_ptr);

void rtree_dump(node * n, int level);

edge * node_find_edge_str(node * n, char * str, int str_len);


void rtree_compile(node *n);

void rtree_compile_patterns(node * n);

node * rtree_match(node * n, char * path, int path_len, match_entry * entry);

bool node_has_slug_edges(node *n);

node * rtree_lookup(node * tree, char * path, int path_len);

edge * edge_create(char * pattern, int pattern_len, node * child);

void edge_branch(edge *e, int dl);

void edge_free(edge * edge);




#endif /* !NODE_H */
