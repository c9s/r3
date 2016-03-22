/*
 * r3.h
 * Copyright (C) 2014 c9s <yoanlin93@gmail.com>
 *
 * Distributed under terms of the MIT license.
 */
#ifndef R3_NODE_H
#define R3_NODE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pcre.h>

#ifdef HAVE_STDBOOL_H

#include <stdbool.h>

#else

#if !defined(bool) && !defined(__cplusplus)
typedef unsigned char bool;
#endif
#ifndef false 
#    define false 0
#endif
#ifndef true
#    define true 1
#endif

#endif

#include "str_array.h"
#include "r3_slug.h"
#include "memory.h"


#ifdef __cplusplus
extern "C" {
#endif

struct _edge;
struct _node;
struct _route;
typedef struct _edge R3Edge;
typedef struct _node R3Node;
typedef struct _R3Route R3Route;

struct _node  {
    R3_VECTOR(R3Edge) edges;
    R3_VECTOR(R3Route) routes;
    char * combined_pattern;
    pcre * pcre_pattern;
    pcre_extra * pcre_extra;

    // edges are mostly less than 255
    unsigned int compare_type; // compare_type: pcre, opcode, string
    unsigned int endpoint; // endpoint, should be zero for non-endpoint nodes
    unsigned int ov_cnt; // capture vector array size for pcre

    // the pointer of R3Route data
    void * data;

    // almost less than 255
} __attribute__((aligned(64)));

#define r3_node_edge_pattern(node,i) node->edges.entries[i].pattern.base
#define r3_node_edge_pattern_len(node,i) node->edges.entries[i].pattern.len

struct _edge {
    r3_iovec_t pattern; // 8 bytes
    R3Node * child; // 8 bytes
    // unsigned int pattern_len; // 4byte
    unsigned int opcode; // 4byte
    unsigned int has_slug; // 4byte
} __attribute__((aligned(64)));

struct _R3Route {
    r3_iovec_t path;
    R3_VECTOR(r3_iovec_t) slugs;
    int request_method; // can be (GET || POST)
    r3_iovec_t host; // required host name

    void * data;

    r3_iovec_t remote_addr_pattern;
} __attribute__((aligned(64)));

typedef struct _R3Entry match_entry;
struct _R3Entry {
    str_array vars;
    r3_iovec_t path; // current path to dispatch
    int request_method;  // current request method

    void * data; // R3Route ptr

    r3_iovec_t host; // the request host
    r3_iovec_t remote_addr;
} __attribute__((aligned(64)));


R3Node * r3_tree_create(int cap);

// R3Node * r3_node_create();

void r3_tree_free(R3Node * tree);

R3Edge * r3_node_connectl(R3Node * n, const char * pat, int len, int strdup, R3Node *child);

#define r3_node_connect(n, pat, child) r3_node_connectl(n, pat, strlen(pat), 0, child)

R3Edge * r3_node_find_edge(const R3Node * n, const char * pat, unsigned int pat_len);

R3Edge * r3_node_append_edge(R3Node *n);

R3Edge * r3_node_find_common_prefix(R3Node *n, const char *path, int path_len, int *prefix_len, char **errstr);

R3Node * r3_tree_insert_pathl(R3Node *tree, const char *path, int path_len, void * data);

#define r3_tree_insert_pathl(tree, path, path_len, data) r3_tree_insert_pathl_ex(tree, path, path_len, 0, 0, data, NULL)



R3Route * r3_tree_insert_routel(R3Node * tree, int method, const char *path, int path_len, void *data);

R3Route * r3_tree_insert_routel_ex(R3Node * tree, int method, const char *path, int path_len, void *data, char **errstr);

#define r3_tree_insert_routel(n, method, path, path_len, data) r3_tree_insert_routel_ex(n, method, path, path_len, data, NULL)

#define r3_tree_insert_path(n,p,d) r3_tree_insert_pathl_ex(n,p,strlen(p), 0, 0, d, NULL)

#define r3_tree_insert_route(n,method,path,data) r3_tree_insert_routel(n, method, path, strlen(path), data)


/**
 * The private API to insert a path
 */
R3Node * r3_tree_insert_pathl_ex(R3Node *tree, const char *path, unsigned int path_len, int method, unsigned int router, void * data, char **errstr);

void r3_tree_dump(const R3Node * n, int level);


R3Edge * r3_node_find_edge_str(const R3Node * n, const char * str, int str_len);


int r3_tree_compile(R3Node *n, char** errstr);

int r3_tree_compile_patterns(R3Node * n, char** errstr);

R3Node * r3_tree_matchl(const R3Node * n, const char * path, unsigned int path_len, match_entry * entry);

#define r3_tree_match(n,p,e)  r3_tree_matchl(n,p, strlen(p), e)

// R3Node * r3_tree_match_entry(R3Node * n, match_entry * entry);
#define r3_tree_match_entry(n, entry) r3_tree_matchl(n, entry->path.base, entry->path.len, entry)

bool r3_node_has_slug_edges(const R3Node *n);

// R3Edge * r3_edge_createl(const char * pattern, int pattern_len, R3Node * child);

void r3_edge_initl(R3Edge *e, const char * pattern, int pattern_len, R3Node * child);

R3Node * r3_edge_branch(R3Edge *e, int dl);

void r3_edge_free(R3Edge * edge);





R3Route * r3_route_create(const char * path);

// R3Route * r3_route_createl(const char * path, int path_len);


R3Route * r3_node_append_route(R3Node *tree, const char * path, int path_len, int method, void *data);

void r3_route_free(R3Route * route);

int r3_route_cmp(const R3Route *r1, const match_entry *r2);

R3Route * r3_tree_match_route(const R3Node *n, match_entry * entry);

#define r3_route_create(p) r3_route_createl(p, strlen(p))


#define METHOD_GET 2
#define METHOD_POST 2<<1
#define METHOD_PUT 2<<2
#define METHOD_DELETE 2<<3
#define METHOD_PATCH 2<<4
#define METHOD_HEAD 2<<5
#define METHOD_OPTIONS 2<<6



int r3_pattern_to_opcode(const char * pattern, unsigned int len);

enum { NODE_COMPARE_STR, NODE_COMPARE_PCRE, NODE_COMPARE_OPCODE };

enum { OP_EXPECT_MORE_DIGITS = 1, OP_EXPECT_MORE_WORDS, OP_EXPECT_NOSLASH, OP_EXPECT_NODASH, OP_EXPECT_MORE_ALPHA };



match_entry * match_entry_createl(const char * path, int path_len);

#define match_entry_create(path) match_entry_createl(path,strlen(path))

void match_entry_free(match_entry * entry);




#ifdef __cplusplus
}
#endif




#endif /* !R3_NODE_H */
