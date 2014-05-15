#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

// Jemalloc memory management
#include <jemalloc/jemalloc.h>

// PCRE
#include <pcre.h>

// Judy array
#include <Judy.h>

#include "node.h"
#include "token.h"

// String value as the index http://judy.sourceforge.net/doc/JudySL_3x.htm

rnode * rnode_create(int cap) {
    rnode * n = (rnode*) calloc( sizeof(rnode) , 1);

    n->children = (rnode**) malloc( sizeof(rnode*) * 10 );
    n->children_len = 0;
    n->children_cap = 10;

    n->edge_patterns = token_array_create(10);
    return n;
}

void rnode_free(rnode * tree) {
    for (int i = 0 ; i < tree->children_len ; i++ ) {
        rnode_free(tree->children[ i ]);
    }
    free(tree->children);
    token_array_free(tree->edge_patterns);
    free(tree);
    tree = NULL;
}


/* parent node, edge pattern, child */
bool rnode_add_edge(rnode * n, char * pat , rnode *child) {
    // find the same sub-pattern, if it does not exist, create one

    rnode * c = rnode_find_edge(n, pat);
    if (c) {
        return FALSE;
    }


    token_array_append(n->edge_patterns, pat);
    rnode_append_child(n, child);

    assert( token_array_len(n->edge_patterns) == n->children_len );
    return TRUE;
}


void rnode_append_child(rnode *n, rnode *child) {
    if (n->children_len >= n->children_cap) {
        n->children_cap *= 2;
        n->children = realloc(n->children, n->children_cap);
    }
    n->children[ n->children_len++ ] = child;
}


rnode * rnode_find_edge(rnode * n, char * pat) {
    for (int i = 0 ; i < n->edge_patterns->len ; i++ ) {
        char * edge_pat = token_array_fetch( n->edge_patterns, i );
        if (strcmp(edge_pat, pat) == 0 ) {
            return n->children[i];
        }
    }
    return NULL;
}

void rnode_append_route(rnode * tree, token_array * tokens) {

}
