/*
 * r3.h
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

#include "str_array.h"

#define node_edge_pattern(node,i) node->edges[i]->pattern
#define node_edge_pattern_len(node,i) node->edges[i]->pattern_len


struct _edge;
struct _node;
typedef struct _edge edge;
typedef struct _node node;

struct _node {
    edge ** edges;
    int      edge_len;
    int      edge_cap;


    /** compile-time variables here.... **/

    /* the combined regexp pattern string from pattern_tokens */
    char * combined_pattern;
    int    combined_pattern_len;
    pcre * pcre_pattern;
    pcre_extra * pcre_extra;
    int    ov_cnt;
    int *  ov;

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
    str_array * vars;
    char * path; // current path to dispatch
    int    path_len; // the length of the current path
    void * route_ptr; // route ptr
    int    request_method;  // current request method
    char * host; // the request host 
    int    host_len;

    char * remote_addr;
    int    remote_addr_len;
} match_entry;

typedef struct {
    char * path;
    int    path_len;

    int    request_method; // can be (GET || POST)

    char * host; // required host name
    int    host_len;

    char * remote_addr_pattern;
    int    remote_addr_pattern_len;

} route;


node * r3_tree_create(int cap);

node * r3_node_create();

void r3_tree_free(node * tree);

void r3_edge_free(edge * edge);

edge * r3_tree_add_child(node * n, char * pat , node *child);

edge * r3_node_find_edge(node * n, char * pat);

void r3_tree_append_edge(node *n, edge *child);

node * r3_tree_insert_path(node *tree, char *route, void * route_ptr);

node * r3_tree_insert_pathn(node *tree, char *route, int route_len, void * route_ptr);

void r3_tree_dump(node * n, int level);

int r3_tree_render_file(node * tree, char * format, char * filename);

int r3_tree_render_dot(node * tree);

edge * r3_node_find_edge_str(node * n, char * str, int str_len);


void r3_tree_compile(node *n);

void r3_tree_compile_patterns(node * n);

node * r3_tree_match(node * n, char * path, int path_len, match_entry * entry);

bool r3_node_has_slug_edges(node *n);

node * r3_tree_lookup(node * tree, char * path, int path_len);

edge * r3_edge_create(char * pattern, int pattern_len, node * child);

void r3_edge_branch(edge *e, int dl);

void r3_edge_free(edge * edge);



match_entry * match_entry_createl(char * path, int path_len);

#define match_entry_create(path) match_entry_createl(path,strlen(path))

void match_entry_free(match_entry * entry);


route * route_create(char * path);

route * route_createl(char * path, int path_len);

#endif /* !NODE_H */
