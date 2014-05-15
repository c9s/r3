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

struct _rnode {
    redge ** children;
    int      children_len;
    int      children_cap;

    /* the combined regexp pattern string from pattern_tokens */
    char* combined_pattern;
};

struct _redge {
    char * pattern;
    int    pattern_len;
    rnode * child;
};

// String value as the index http://judy.sourceforge.net/doc/JudySL_3x.htm

rnode * rnode_create(int cap) {
    rnode * n = (rnode*) calloc( sizeof(rnode) , 1);

    n->children = (redge**) malloc( sizeof(redge*) * 10 );
    n->children_len = 0;
    n->children_cap = 10;
    // n->edge_patterns = token_array_create(10);
    return n;
}

void rnode_free(rnode * tree) {
    for (int i = 0 ; i < tree->children_len ; i++ ) {
        redge_free(tree->children[ i ]);
    }
    free(tree->children);
    // token_array_free(tree->edge_patterns);
    free(tree);
    tree = NULL;
}



/* parent node, edge pattern, child */
bool rnode_add_child(rnode * n, char * pat , rnode *child) {
    // find the same sub-pattern, if it does not exist, create one

    redge * e;

    e = rnode_find_edge(n, pat);
    if (e) {
        return FALSE;
    }

    e = redge_create( pat, strlen(pat), child);
    rnode_append_edge(n, e);
    // token_array_append(n->edge_patterns, pat);
    // assert( token_array_len(n->edge_patterns) == n->children_len );
    return TRUE;
}



void rnode_append_edge(rnode *n, redge *e) {
    if (n->children_len >= n->children_cap) {
        n->children_cap *= 2;
        n->children = realloc(n->children, sizeof(redge) * n->children_cap);
    }
    n->children[ n->children_len++ ] = e;
}


redge * rnode_find_edge(rnode * n, char * pat) {
    redge * e;
    for (int i = 0 ; i < n->children_len ; i++ ) {
        e = n->children[i];
        if ( strcmp(e->pattern, pat) == 0 ) {
            return e;
        }
    }
    return NULL;
}

rnode * rnode_insert_tokens(rnode * tree, token_array * tokens) {
    rnode * n = tree;
    redge * e = NULL;
    for ( int i = 0 ; i < tokens->len ; i++ ) {
        e = rnode_find_edge(n, token_array_fetch(tokens, i) );
        if (e) {
            n = e->child;
            continue;
        }
        // insert node
        rnode * child = rnode_create(3);
        rnode_add_child(n, token_array_fetch(tokens,i) , child);
        n = child;
    }
    return n;
}

void rnode_dump(rnode * n, int level) {
    if ( n->children_len ) {
        print_indent(level);
        printf("*\n");
        for ( int i = 0 ; i < n->children_len ; i++ ) {
            redge * e = n->children[i];
            print_indent(level + 1);
            printf("+ \"%s\"\n", e->pattern);
            rnode_dump( e->child, level + 1);
        }
    }
}

redge * redge_create(char * pattern, int pattern_len, rnode * child) {
    redge * edge = (redge*) malloc( sizeof(redge) );
    edge->pattern = pattern;
    edge->pattern_len = pattern_len;
    edge->child = child;
    return edge;
}

void redge_free(redge * e) {
    free(e->pattern);
    if ( e->child ) {
        rnode_free(e->child);
    }
}



