/*
 * gvz.c
 * Copyright (C) 2014 c9s <c9s@c9smba.local>
 *
 * Distributed under terms of the MIT license.
 */

#include <gvc.h>
#include <stdio.h>
#include <stdlib.h>
#include "r3.h"
#include "r3_gvc.h"
#include "zmalloc.h"


static char * node_id_str(int id) {
    char * name = zmalloc(sizeof(char) * 20);
    sprintf(name, "#%d", id);
    return name;
}

void r3_tree_build_ag_nodes(Agraph_t * g, Agnode_t * ag_parent_node, node * n, int node_cnt) {
    edge * e;
    Agnode_t *agn_child;
    Agedge_t *agn_edge;

    for ( int i = 0 ; i < n->edge_len ; i++ ) {
        e = n->edges[i];

        node_cnt++;
        agn_child = agnode(g, node_id_str(node_cnt) , 1);
        agn_edge = agedge(g, ag_parent_node, agn_child, 0, 1);
        agsafeset(agn_edge, "label", e->pattern, "");
        if (e->child && e->child->endpoint) {
            agsafeset(agn_child, "shape", "doublecircle", "");
        }
        r3_tree_build_ag_nodes(g, agn_child, e->child, node_cnt);
    }
}




/**
 * Render a tree to tree graph image via graphviz (dot)
 */
int r3_tree_render_dot(node * tree)
{
    Agraph_t *g;
    /* set up a graphviz context - but only once even for multiple graphs */
    static GVC_t *gvc;

    if (!gvc) {
        gvc = gvContext();
    }
    /* Create a simple digraph */
    g = agopen("g", Agdirected, 0);

    // create self node
    Agnode_t *ag_root = agnode(g, "{root}", 1);
    r3_tree_build_ag_nodes(g, ag_root, tree, 0);
    gvLayout(gvc, g, "dot");
    gvRender(gvc, g, "dot", stdout);
    gvFreeLayout(gvc, g);
    agclose(g);
    return 0;
}




/**
 * Render a tree to tree graph image via graphviz (dot)
 */
int r3_tree_render_file(node * tree, char * format, char * filename)
{
    Agraph_t *g;

    /* set up a graphviz context - but only once even for multiple graphs */
    static GVC_t *gvc;

    if (!gvc) {
        gvc = gvContext();
    }

    /* Create a simple digraph */
    g = agopen("g", Agdirected, 0);

    // create self node
    Agnode_t *ag_root = agnode(g, "{root}", 1);
    r3_tree_build_ag_nodes(g, ag_root, tree, 0);

    gvLayout(gvc, g, "dot");
    gvRenderFilename(gvc, g, format, filename);
    gvFreeLayout(gvc, g);

    agclose(g);
    return 0;
}

