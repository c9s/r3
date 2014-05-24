/*
 * r3.h
 * Copyright (C) 2014 c9s <c9s@c9smba.local>
 *
 * Distributed under terms of the MIT license.
 */
#ifndef R3_NODE_H
#define R3_NODE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <pcre.h>

#include "r3_define.h"
#include "str_array.h"


#define node_edge_pattern(node,i) node->edges[i]->pattern
#define node_edge_pattern_len(node,i) node->edges[i]->pattern_len


struct _edge;
struct _node;
struct _route;
typedef struct _edge edge;
typedef struct _node node;
typedef struct _route route;

struct _node {
    edge  ** edges;
    route ** routes;

    // almost less than 255
    unsigned char      edge_len;
    unsigned char      edge_cap;

    // almost less than 255
    unsigned char      route_len;
    unsigned char      route_cap;

    unsigned char    compare_type;
    unsigned char    endpoint;

    /** compile-time variables here.... **/

    /* the combined regexp pattern string from pattern_tokens */
    pcre * pcre_pattern;
    pcre_extra * pcre_extra;


    char * combined_pattern;

    /**
     * the pointer of route data
     */
    void * data;
};

struct _edge {
    char * pattern;
    node * child;
    unsigned char opcode;
    unsigned short pattern_len;
    bool     has_slug:1;
};

typedef struct {
    str_array * vars;
    char * path; // current path to dispatch
    int    path_len; // the length of the current path
    int    request_method;  // current request method

    void * data; // route ptr

    char * host; // the request host 
    int    host_len;

    char * remote_addr;
    int    remote_addr_len;
} match_entry;

struct _route {
    char * path;
    int    path_len;

    int    request_method; // can be (GET || POST)

    char * host; // required host name
    int    host_len;

    void * data;

    char * remote_addr_pattern;
    int    remote_addr_pattern_len;
};


node * r3_tree_create(int cap);

node * r3_node_create();

void r3_tree_free(node * tree);

void r3_edge_free(edge * edge);

edge * r3_node_connectl(node * n, char * pat, int len, int strdup, node *child);

#define r3_node_connect(n, pat, child) r3_node_connectl(n, pat, strlen(pat), 0, child)

edge * r3_node_find_edge(node * n, char * pat);

void r3_node_append_edge(node *n, edge *child);


node * r3_tree_insert_pathl(node *tree, char *path, int path_len, void * data);

#define r3_tree_insert_path(n,p,d) r3_tree_insert_pathl_(n,p,strlen(p), NULL, d)
#define r3_tree_insert_route(n,r,d) r3_tree_insert_pathl_(n, r->path, r->path_len, r, d)

/**
 * The private API to insert a path
 */
node * r3_tree_insert_pathl_(node *tree, char *path, int path_len, route * route, void * data);

void r3_tree_dump(node * n, int level);

int r3_tree_render_file(node * tree, char * format, char * filename);

int r3_tree_render_dot(node * tree);

edge * r3_node_find_edge_str(const node * n, char * str, int str_len);


void r3_tree_compile(node *n);

void r3_tree_compile_patterns(node * n);

node * r3_tree_matchl(const node * n, char * path, int path_len, match_entry * entry);

#define r3_tree_match(n,p,e)  r3_tree_matchl(n,p, strlen(p), e)

// node * r3_tree_match_entry(node * n, match_entry * entry);
#define r3_tree_match_entry(n, entry) r3_tree_matchl(n, entry->path, entry->path_len, entry)

bool r3_node_has_slug_edges(node *n);

edge * r3_edge_create(char * pattern, int pattern_len, node * child);

node * r3_edge_branch(edge *e, int dl);

void r3_edge_free(edge * edge);



match_entry * match_entry_createl(char * path, int path_len);

#define match_entry_create(path) match_entry_createl(path,strlen(path))

void match_entry_free(match_entry * entry);


route * r3_route_create(char * path);

route * r3_route_createl(char * path, int path_len);

int r3_route_cmp(route *r1, match_entry *r2);

void r3_node_append_route(node * n, route * route);

void r3_route_free(route * route);

route * r3_tree_match_route(const node *n, match_entry * entry);

#define METHOD_GET 2
#define METHOD_POST 2<<1
#define METHOD_PUT 2<<2
#define METHOD_DELETE 2<<3
#define METHOD_PATCH 2<<4
#define METHOD_HEAD 2<<5
#define METHOD_OPTIONS 2<<6



int r3_pattern_to_opcode(char * pattern, int pattern_len);

enum { NODE_COMPARE_STR, NODE_COMPARE_PCRE, NODE_COMPARE_OPCODE };

enum { OP_EXPECT_MORE_DIGITS = 1, OP_EXPECT_MORE_WORDS, OP_EXPECT_NOSLASH, OP_EXPECT_NODASH, OP_EXPECT_MORE_ALPHA };

#endif /* !R3_NODE_H */
