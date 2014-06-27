/*
 * r3_gvc.h
 * Copyright (C) 2014 c9s <yoanlin93@gmail.com>
 *
 * Distributed under terms of the MIT license.
 */
#ifndef R3_GVC_H
#define R3_GVC_H

#include <stdio.h>
#include <gvc.h>
#include "r3.h"

void r3_tree_build_ag_nodes(Agraph_t * g, Agnode_t * ag_parent_node, const node * n, int * node_cnt);

int r3_tree_render(const node * tree, const char *layout, const char * format, FILE *fp);

int r3_tree_render_dot(const node * tree, const char *layout, FILE *fp);

int r3_tree_render_file(const node * tree, const char * format, const char * filename);


#endif /* !R3_GVC_H */
