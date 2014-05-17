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

// int main(int argc, char **argv)
int r3_tree_render_file(node * tree, char * format, char * filename)
{
    Agraph_t *g;
    Agnode_t *n, *m;
    Agedge_t *e;

    /* set up a graphviz context - but only once even for multiple graphs */
    static GVC_t *gvc;

    if (!gvc) {
        gvc = gvContext();
    }

    /* Create a simple digraph */
    g = agopen("g", Agdirected, 0);

    n = agnode(g, "n", 1);
    m = agnode(g, "m", 1);
    e = agedge(g, n, m, 0, 1);

    /* Set an attribute - in this case one that affects the visible rendering */
    agsafeset(n, "color", "red", "");
    /* Use the directed graph layout engine */
    gvLayout(gvc, g, "dot");
    gvRenderFilename(gvc, g, format, filename);
    gvFreeLayout(gvc, g);

    agclose(g);
    return 0;
}

