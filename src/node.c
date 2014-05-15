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

#include "str.h"
#include "node.h"
#include "token.h"

struct _rnode {
    redge ** edges;
    int      edge_len;
    int      edge_cap;

    /* the combined regexp pattern string from pattern_tokens */
    char * combined_pattern;
    int    combined_pattern_len;

    int endpoint;
};

struct _redge {
    char * pattern;
    int    pattern_len;
    bool   is_slug;
    rnode * child;
};

// String value as the index http://judy.sourceforge.net/doc/JudySL_3x.htm



/**
 * Create a rnode object
 */
rnode * rnode_create(int cap) {
    rnode * n = (rnode*) malloc( sizeof(rnode) );

    n->edges = (redge**) malloc( sizeof(redge*) * 10 );
    n->edge_len = 0;
    n->edge_cap = 10;
    n->endpoint = 0;
    n->combined_pattern = NULL;
    // n->edge_patterns = token_array_create(10);
    return n;
}

void rnode_free(rnode * tree) {
    for (int i = 0 ; i < tree->edge_len ; i++ ) {
        if (tree->edges[i]) {
            redge_free(tree->edges[ i ]);
        }
    }
    free(tree->edges);
    // token_array_free(tree->edge_patterns);
    free(tree);
    tree = NULL;
}



/* parent node, edge pattern, child */
redge * rnode_add_child(rnode * n, char * pat , rnode *child) {
    // find the same sub-pattern, if it does not exist, create one

    redge * e;

    e = rnode_find_edge(n, pat);
    if (e) {
        return e;
    }

    e = redge_create( pat, strlen(pat), child);
    rnode_append_edge(n, e);
    // token_array_append(n->edge_patterns, pat);
    // assert( token_array_len(n->edge_patterns) == n->edge_len );
    return e;
}



void rnode_append_edge(rnode *n, redge *e) {
    if (n->edge_len >= n->edge_cap) {
        n->edge_cap *= 2;
        n->edges = realloc(n->edges, sizeof(redge) * n->edge_cap);
    }
    n->edges[ n->edge_len++ ] = e;
}


redge * rnode_find_edge(rnode * n, char * pat) {
    redge * e;
    for (int i = 0 ; i < n->edge_len ; i++ ) {
        e = n->edges[i];
        if ( strcmp(e->pattern, pat) == 0 ) {
            return e;
        }
    }
    return NULL;
}



/**
 * This function combines ['/foo', '/bar', '/{slug}'] into (/foo)|(/bar)|/([^/]+)}
 *
 */
void rnode_combine_patterns(rnode * n) {
    char * cpat;
    char * p;

    cpat = malloc(128);
    if (cpat==NULL)
        return;

    p = cpat;

    redge *e = NULL;
    for ( int i = 0 ; i < n->edge_len ; i++ ) {
        e = n->edges[i];
        strncat(p++,"(", 1);
        strncat(p, e->pattern, e->pattern_len);

        p += e->pattern_len;

        strncat(p++,")", 1);

        if ( i + 1 < n->edge_len ) {
            strncat(p++,"|",1);
        }
    }
    n->combined_pattern = cpat;
    n->combined_pattern_len = p - cpat;
}


rnode * rnode_lookup(rnode * tree, char * path, int path_len) {
    token_array * tokens = split_route_pattern(path, path_len);

    rnode * n = tree;
    redge * e = NULL;
    for ( int i = 0 ; i < tokens->len ; i++ ) {
        e = rnode_find_edge(n, token_array_fetch(tokens, i) );
        if (!e) {
            return NULL;
        }
        n = e->child;
    }
    if (n->endpoint) {
        return n;
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
        rnode_add_child(n, strdup(token_array_fetch(tokens,i)) , child);
        n = child;
    }
    n->endpoint++;
    return n;
}

rnode * rnode_insert_route(rnode *tree, char *route)
{
    return rnode_insert_routel(tree, route, strlen(route) );
}

rnode * rnode_insert_routel(rnode *tree, char *route, int route_len)
{
    rnode * n = tree;
    redge * e = NULL;

    char * p = route;

    /* length of common prefix */
    int dl = 0;
    for( int i = 0 ; i < n->edge_len ; i++ ) {
        dl = strndiff(route, n->edges[i]->pattern, n->edges[i]->pattern_len);

        // printf("dl: %d   %s vs %s\n", dl, route, n->edges[i]->pattern );

        // no common, consider insert a new edge
        if ( dl > 0 ) {
            e = n->edges[i];
            break;
        }
    }


    if ( dl == 0 ) {
        // not found, we should just insert a whole new edge
        rnode * child = rnode_create(3);
        rnode_add_child(n, strndup(route, route_len) , child);
        printf("edge not found, insert one: %s\n", route);

        n = child;
        return n;
    } else if ( dl == e->pattern_len ) {    // fully-equal to the pattern of the edge

        char * subroute = route + dl;
        int    subroute_len = route_len - dl;

        // there are something more we can insert
        if ( subroute_len > 0 ) {
            return rnode_insert_routel(e->child, subroute, subroute_len);
        } else {
            // no more,
            e->child->endpoint++; // make it as an endpoint, TODO: put the route value
            return e->child;
        }

    } else if ( dl < e->pattern_len ) {
        // printf("branch the edge dl: %d\n", dl);

        // branch the edge at correct position (avoid broken slugs)
        char *slug_s = strchr(route, '{');
        char *slug_e = strchr(route, '}');
        if ( dl > (slug_s - route) && dl < (slug_e - route) ) {
            // break before '{'
            dl = slug_s - route;
        }



        /* it's partically matched with the pattern,
         * we should split the end point and make a branch here...
         */
        rnode *c1, *c2; // child 1, child 2
        redge *e1, *e2; // edge 1, edge 2
        char * s1 = e->pattern + dl;
        char * s2 = route + dl;
        int s1_len = 0, s2_len = 0;

        redge **tmp_edges = e->child->edges;
        int   **tmp_edge_len = e->child->edge_len;

        // the suffix edge of the leaf
        c1 = rnode_create(3);
        s1_len = e->pattern_len - dl;
        e1 = redge_create(strndup(s1, s1_len), s1_len, c1);
        // printf("edge left: %s\n", e1->pattern);

        // Migrate the child edges to the new edge we just created.
        for ( int i = 0 ; i < tmp_edge_len ; i++ ) {
            rnode_append_edge(c1, tmp_edges[i]);
            e->child->edges[i] = NULL;
        }
        e->child->edge_len = 0;

        rnode_append_edge(e->child, e1);




        // here is the new edge from.
        c2 = rnode_create(3);
        s2_len = route_len - dl;
        e2 = redge_create(strndup(s2, s2_len), s2_len, c2);
        // printf("edge right: %s\n", e2->pattern);
        rnode_append_edge(e->child, e2);

        // truncate the original edge pattern 
        free(e->pattern);
        e->pattern = strndup(e->pattern, dl);
        e->pattern_len = dl;



        // Move the child edges to the new suffix edge child 
        /*
        e->child->edge_len = 0;
        */


        // move n->edges to c1
        c1->endpoint++;
        c2->endpoint++;
        return c2;
    } else if ( dl > 0 ) {

    } else {
        printf("unexpected condition.");
        return NULL;
    }
    // token_array * t = split_route_pattern(route, strlen(route));
    // return rnode_insert_tokens(tree, t);
    // n->endpoint++;
    return n;
}

void rnode_dump(rnode * n, int level) {
    if ( n->edge_len ) {
        print_indent(level);
        printf("*\n");
        for ( int i = 0 ; i < n->edge_len ; i++ ) {
            redge * e = n->edges[i];
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
    edge->is_slug = 0;
    return edge;
}

void redge_free(redge * e) {
    if (e->pattern) {
        free(e->pattern);
    }
    if ( e->child ) {
        rnode_free(e->child);
    }
}



