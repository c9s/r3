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

#include "token.h"

struct _redge;
struct _rnode;
typedef struct _redge redge;
typedef struct _rnode rnode;




rnode * rnode_create(int cap);

void rnode_free(rnode * tree);

void redge_free(redge * edge);

redge * rnode_add_child(rnode * n, char * pat , rnode *child);

redge * rnode_find_edge(rnode * n, char * pat);

void rnode_append_edge(rnode *n, redge *child);

rnode * rnode_insert_tokens(rnode * tree, token_array * tokens);

rnode * rnode_insert_route(rnode *tree, char *route);

rnode * rnode_insert_routel(rnode *tree, char *route, int route_len);

void rnode_dump(rnode * n, int level);

void rnode_combine_patterns(rnode * n);

bool rnode_has_slug_edges(rnode *n);

rnode * rnode_lookup(rnode * tree, char * path, int path_len);

redge * redge_create(char * pattern, int pattern_len, rnode * child);

void redge_branch(redge *e, int dl);

void redge_free(redge * edge);

#endif /* !NODE_H */
