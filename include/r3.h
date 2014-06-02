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
#include <pcre.h>
#include <stdbool.h>
#include "config.h"
#include "r3_define.h"
#include "str_array.h"
#include "match_entry.h"

#ifdef ENABLE_JSON
#include <json-c/json.h>
#endif


#ifdef __cplusplus
extern "C" {
#endif

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

#define node_edge_pattern(node,i) node->edges[i]->pattern
#define node_edge_pattern_len(node,i) node->edges[i]->pattern_len

struct _edge {
    char * pattern;
    node * child;
    unsigned short pattern_len; // 2 byte
    unsigned char  opcode; // 1 byte
    unsigned char  has_slug; // 1 bit
};

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

edge * r3_node_connectl(node * n, const char * pat, int len, int strdup, node *child);

#define r3_node_connect(n, pat, child) r3_node_connectl(n, pat, strlen(pat), 0, child)

edge * r3_node_find_edge(const node * n, const char * pat, int pat_len);

void r3_node_append_edge(node *n, edge *child);


edge * r3_node_find_common_prefix(node *n, char *path, int path_len, int *prefix_len);

node * r3_tree_insert_pathl(node *tree, const char *path, int path_len, void * data);

route * r3_tree_insert_routel(node *tree, int method, const char *path, int path_len, void *data);

#define r3_tree_insert_path(n,p,d) r3_tree_insert_pathl_(n,p,strlen(p), NULL, d, NULL)

#define r3_tree_insert_route(n,method,path,data) r3_tree_insert_routel(n, method, path, strlen(path), data)


/**
 * The private API to insert a path
 */
node * r3_tree_insert_pathl_(node *tree, const char *path, int path_len, route * route, void * data, char ** errstr);

void r3_tree_dump(const node * n, int level);


edge * r3_node_find_edge_str(const node * n, const char * str, int str_len);


int r3_tree_compile(node *n, char** errstr);

int r3_tree_compile_patterns(node * n, char** errstr);

node * r3_tree_matchl(const node * n, const char * path, int path_len, match_entry * entry);

#define r3_tree_match(n,p,e)  r3_tree_matchl(n,p, strlen(p), e)

// node * r3_tree_match_entry(node * n, match_entry * entry);
#define r3_tree_match_entry(n, entry) r3_tree_matchl(n, entry->path, entry->path_len, entry)

bool r3_node_has_slug_edges(const node *n);

edge * r3_edge_createl(const char * pattern, int pattern_len, node * child);

node * r3_edge_branch(edge *e, int dl);

void r3_edge_free(edge * edge);





route * r3_route_create(const char * path);

route * r3_route_createl(const char * path, int path_len);

int r3_route_cmp(const route *r1, const match_entry *r2);

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



int r3_pattern_to_opcode(const char * pattern, int pattern_len);

enum { NODE_COMPARE_STR, NODE_COMPARE_PCRE, NODE_COMPARE_OPCODE };

enum { OP_EXPECT_MORE_DIGITS = 1, OP_EXPECT_MORE_WORDS, OP_EXPECT_NOSLASH, OP_EXPECT_NODASH, OP_EXPECT_MORE_ALPHA };

#ifdef ENABLE_JSON
json_object * r3_edge_to_json_object(const edge * e);
json_object * r3_node_to_json_object(const node * n);
json_object * r3_route_to_json_object(const route * r);

const char * r3_node_to_json_string_ext(const node * n, int options);
const char * r3_node_to_json_pretty_string(const node * n);
const char * r3_node_to_json_string(const node * n);
#endif

#ifdef __cplusplus
}
#endif

#endif /* !R3_NODE_H */
