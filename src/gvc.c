/*
 * gvz.c
 * Copyright (C) 2014 c9s <yoanlin93@gmail.com>
 *
 * Distributed under terms of the MIT license.
 */
#include "config.h"
#include <gvc.h>
#include <stdio.h>
#include <stdlib.h>
#include "r3.h"
#include "r3_gvc.h"
#include "zmalloc.h"

void r3_tree_build_ag_nodes(Agraph_t * g, Agnode_t * ag_parent_node, const node * n, int * node_cnt) {
    if (!n)
        return;

    for ( int i = 0 ; i < n->edge_len ; i++ ) {
        edge * e = n->edges[i];
        (*node_cnt)++;

        Agnode_t *agn_child = NULL;
        Agedge_t *agn_edge = NULL;

        char *nodename = NULL;
        if ( e && e->child && e->child->combined_pattern ) {
            asprintf(&nodename,"%s", e->child->combined_pattern);
        } else {
            asprintf(&nodename,"#%d", *node_cnt);
        }

        agn_child = agnode(g, nodename, 1);
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
int r3_tree_render(const node * tree, const char *layout, const char * format, FILE *fp)
{
    Agraph_t *g;
    /* set up a graphviz context - but only once even for multiple graphs */
    GVC_t *gvc = NULL;
    gvc = gvContext();
    /* Create a simple digraph */
    // g = agopen("g", Agdirected, 0);
    g = agopen("g", Agundirected, 0);

    // create self node
    Agnode_t *ag_root = agnode(g, "{root}", 1);
    int n = 0;
    r3_tree_build_ag_nodes(g, ag_root, tree, &n);
    gvLayout(gvc, g, layout);
    gvRender(gvc, g, format, fp);
    gvFreeLayout(gvc, g);
    agclose(g);
    return 0;
}






/**
 * Render a tree to tree graph image via graphviz (dot)
 */
int r3_tree_render_dot(const node * tree, const char *layout, FILE *fp)
{
    return r3_tree_render(tree, layout, "dot", fp);
}


/**
 * Render a tree to tree graph image via graphviz (dot)
 */
int r3_tree_render_file(const node * tree, const char * format, const char * filename)
{
    Agraph_t *g;

    GVC_t *gvc = NULL;
    gvc = gvContext();
    /*
    // set up a graphviz context - but only once even for multiple graphs
    static GVC_t *gvc;
    if (!gvc) {
        gvc = gvContext();
    }
    */

    /* Create a simple digraph */
    // g = agopen("g", Agdirected, 0);
    g = agopen("g", Agundirected, 0);

    // create self node
    Agnode_t *ag_root = agnode(g, "{root}", 1);
    int n = 0;
    r3_tree_build_ag_nodes(g, ag_root, tree, &n);

    gvLayout(gvc, g, "dot");
    gvRenderFilename(gvc, g, format, filename);
    gvFreeLayout(gvc, g);

    agclose(g);
    return 0;
}

