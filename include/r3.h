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
#include <stdbool.h>

#include "r3_define.h"

typedef struct _str_array {
  char **tokens;
  int    len;
  int    cap;
} str_array;

str_array * str_array_create(int cap);

bool str_array_is_full(const str_array * l);

bool str_array_resize(str_array *l, int new_cap);

bool str_array_append(str_array * list, char * token);

void str_array_free(str_array *l);

void str_array_dump(const str_array *l);

str_array * split_route_pattern(char *pattern, int pattern_len);

#define str_array_fetch(t,i)  t->tokens[i]
#define str_array_len(t)  t->len
#define str_array_cap(t)  t->cap


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
    // edge  ** edge_table;

    // almost less than 255
    unsigned char    edge_len;
    unsigned char    compare_type;
    unsigned char    endpoint;
    unsigned char    ov_cnt;

    // <-- here comes a char[3] struct padding for alignment since we have 5 char above.


    /** compile-time variables here.... **/

    /* the combined regexp pattern string from pattern_tokens */
    pcre * pcre_pattern;
    pcre_extra * pcre_extra;

    route ** routes;

    char * combined_pattern;

    // almost less than 255
    unsigned char      edge_cap;
    unsigned char      route_len;
    unsigned char      route_cap;

    /**
     * the pointer of route data
     */
    void * data;
};

struct _edge {
    char * pattern;
    node * child;
    unsigned short pattern_len; // 2 byte
    unsigned char  opcode; // 1 byte
    unsigned char  has_slug; // 1 bit
};

typedef struct {
    str_array * vars;
    const char * path; // current path to dispatch
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

edge * r3_node_connectl(node * n, const char * pat, int len, int strdup, node *child);

#define r3_node_connect(n, pat, child) r3_node_connectl(n, pat, strlen(pat), 0, child)

edge * r3_node_find_edge(const node * n, const char * pat);

void r3_node_append_edge(node *n, edge *child);


node * r3_tree_insert_pathl(node *tree, char *path, int path_len, void * data);

#define r3_tree_insert_path(n,p,d) r3_tree_insert_pathl_(n,p,strlen(p), NULL, d)
#define r3_tree_insert_route(n,r,d) r3_tree_insert_pathl_(n, r->path, r->path_len, r, d)

/**
 * The private API to insert a path
 */
node * r3_tree_insert_pathl_(node *tree, char *path, int path_len, route * route, void * data);

void r3_tree_dump(const node * n, int level);

int r3_tree_render_file(const node * tree, const char * format, char * filename);

int r3_tree_render_dot(node * tree);

edge * r3_node_find_edge_str(const node * n, const char * str, int str_len);


void r3_tree_compile(node *n);

void r3_tree_compile_patterns(node * n);

node * r3_tree_matchl(const node * n, const char * path, int path_len, const match_entry * entry);

#define r3_tree_match(n,p,e)  r3_tree_matchl(n,p, strlen(p), e)

// node * r3_tree_match_entry(node * n, match_entry * entry);
#define r3_tree_match_entry(n, entry) r3_tree_matchl(n, entry->path, entry->path_len, entry)

bool r3_node_has_slug_edges(const node *n);

edge * r3_edge_create(const char * pattern, int pattern_len, node * child);

node * r3_edge_branch(edge *e, int dl);

void r3_edge_free(edge * edge);





route * r3_route_create(const char * path);

route * r3_route_createl(const char * path, int path_len);

int r3_route_cmp(route *r1, const match_entry *r2);

void r3_node_append_route(node * n, route * route);

void r3_route_free(route * route);

route * r3_tree_match_route(const node *n, const match_entry * entry);

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
