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

#include "define.h"
#include "str.h"
#include "node.h"
#include "token.h"


// String value as the index http://judy.sourceforge.net/doc/JudySL_3x.htm

/**
 * Create a node object
 */
node * rtree_create(int cap) {
    node * n = (node*) malloc( sizeof(node) );

    n->edges = (edge**) malloc( sizeof(edge*) * 10 );
    n->edge_len = 0;
    n->edge_cap = 10;
    n->endpoint = 0;
    n->combined_pattern = NULL;
    return n;
}

void rtree_free(node * tree) {
    for (int i = 0 ; i < tree->edge_len ; i++ ) {
        if (tree->edges[i]) {
            edge_free(tree->edges[ i ]);
        }
    }

    if (tree->combined_pattern)
        free(tree->combined_pattern);

    free(tree->edges);
    // token_array_free(tree->edge_patterns);
    free(tree);
    tree = NULL;
}



/* parent node, edge pattern, child */
edge * rtree_add_child(node * n, char * pat , node *child) {
    // find the same sub-pattern, if it does not exist, create one

    edge * e;

    e = node_find_edge(n, pat);
    if (e) {
        return e;
    }

    e = edge_create( pat, strlen(pat), child);
    rtree_append_edge(n, e);
    // token_array_append(n->edge_patterns, pat);
    // assert( token_array_len(n->edge_patterns) == n->edge_len );
    return e;
}



void rtree_append_edge(node *n, edge *e) {

    if (!n->edges) {
        n->edge_cap = 3;
        n->edges = malloc(sizeof(edge) * n->edge_cap);
    }
    if (n->edge_len >= n->edge_cap) {
        n->edge_cap *= 2;
        n->edges = realloc(n->edges, sizeof(edge) * n->edge_cap);
    }
    n->edges[ n->edge_len++ ] = e;
}


edge * node_find_edge(node * n, char * pat) {
    edge * e;
    for (int i = 0 ; i < n->edge_len ; i++ ) {
        e = n->edges[i];
        if ( strcmp(e->pattern, pat) == 0 ) {
            return e;
        }
    }
    return NULL;
}

void rtree_compile(node *n)
{
    bool use_slug = node_has_slug_edges(n);
    if ( use_slug ) {
        rtree_compile_patterns(n);
    } else {
        // use normal text matching...
        n->combined_pattern = NULL;
    }

    for (int i = 0 ; i < n->edge_len ; i++ ) {
        rtree_compile(n->edges[i]->child);
    }
}


/**
 * This function combines ['/foo', '/bar', '/{slug}'] into (/foo)|(/bar)|/([^/]+)}
 *
 */
void rtree_compile_patterns(node * n) {
    char * cpat;
    char * p;

    cpat = calloc(sizeof(char),128);
    if (cpat==NULL)
        return;

    p = cpat;

    edge *e = NULL;
    for ( int i = 0 ; i < n->edge_len ; i++ ) {
        e = n->edges[i];
        if ( e->has_slug ) {
            char * slug_pat = compile_slug(e->pattern, e->pattern_len);
            strcat(p, slug_pat);
        } else {
            strncat(p++,"(", 1);

            strncat(p, e->pattern, e->pattern_len);
            p += e->pattern_len;

            strncat(p++,")", 1);
        }

        if ( i + 1 < n->edge_len ) {
            strncat(p++,"|",1);
        }
    }
    n->combined_pattern = cpat;
    n->combined_pattern_len = p - cpat;


    const char *error;
    int erroffset;

    // n->pcre_pattern;
    n->pcre_pattern = pcre_compile(
            n->combined_pattern,              /* the pattern */
            0,                                /* default options */
            &error,               /* for error message */
            &erroffset,           /* for error offset */
            NULL);                /* use default character tables */
    if (n->pcre_pattern == NULL) {
        printf("PCRE compilation failed at offset %d: %s\n", erroffset, error);
        return;
    }
    n->pcre_extra = pcre_study(n->pcre_pattern, 0, &error);
    if (n->pcre_extra == NULL) {
        printf("PCRE study failed at offset %s\n", error);
        return;
    }
}




node * rtree_match(node * n, char * path, int path_len) {
    if (n->combined_pattern && n->pcre_pattern) {
        info("pcre matching %s on %s\n", n->combined_pattern, path);
        // int ovector_count = (n->edge_len + 1) * 2;
        int ovector_count = 30;
        int ovector[ovector_count];
        int rc;
        rc = pcre_exec(
                n->pcre_pattern,   /* the compiled pattern */
                NULL,              /* no extra data - we didn't study the pattern */
                path,              /* the subject string */
                path_len,          /* the length of the subject */
                0,                 /* start at offset 0 in the subject */
                0,                 /* default options */
                ovector,           /* output vector for substring information */
                ovector_count);      /* number of elements in the output vector */

        info("rc: %d\n", rc );
        if (rc < 0) {
            switch(rc)
            {
                case PCRE_ERROR_NOMATCH: printf("No match\n"); break;
                /*
                Handle other special cases if you like
                */
                default: printf("Matching error %d\n", rc); break;
            }
            // does not match all edges, return NULL;
            return NULL;
        }

        int i;
        for (i = 1; i < rc; i++)
        {
            char *substring_start = path + ovector[2*i];
            int   substring_length = ovector[2*i+1] - ovector[2*i];
            info("%2d: %.*s\n", i, substring_length, substring_start);
            if ( substring_length > 0) {
                int restlen = path_len - ovector[2*i+1]; // fully match to the end
                info("matched item => restlen:%d edges:%d i:%d\n", restlen, n->edge_len, i);
                if (restlen) {
                    return rtree_match( n->edges[i - 1]->child, substring_start + substring_length, restlen);
                }
                return n->edges[i - 1]->child;
            }
        }
        // does not match
        return NULL;
    }

    edge *e = node_find_edge_str(n, path, path_len);
    if (e) {
        int len = path_len - e->pattern_len;
        if(len == 0) {
            return e->child;
        } else {
            return rtree_match(e->child, path + e->pattern_len, len);
        }
    }
    return NULL;
}

edge * node_find_edge_str(node * n, char * str, int str_len) {
    edge *e;
    for ( int i = 0 ; i < n->edge_len ; i++ ) {
        e = n->edges[i];
        char *p = e->pattern;
        while ( *p == *str ) {
            p++;
        }
        if ( p - e->pattern == e->pattern_len ) {
            return e;
        }
    }
    return NULL;
}


node * rtree_lookup(node * tree, char * path, int path_len) {
    token_array * tokens = split_route_pattern(path, path_len);

    node * n = tree;
    edge * e = NULL;
    for ( int i = 0 ; i < tokens->len ; i++ ) {
        e = node_find_edge(n, token_array_fetch(tokens, i) );
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

node * node_create() {
    node * n = (node*) malloc( sizeof(node) );
    n->edges = NULL;
    n->edge_len = 0;
    n->edge_cap = 0;
    n->endpoint = 0;
    n->combined_pattern = NULL;
    n->pcre_pattern = NULL;
    return n;
}



node * rtree_insert_tokens(node * tree, token_array * tokens) {
    node * n = tree;
    edge * e = NULL;
    for ( int i = 0 ; i < tokens->len ; i++ ) {
        e = node_find_edge(n, token_array_fetch(tokens, i) );
        if (e) {
            n = e->child;
            continue;
        }
        // insert node
        node * child = node_create();
        rtree_add_child(n, strdup(token_array_fetch(tokens,i)) , child);
        n = child;
    }
    n->endpoint++;
    return n;
}

node * rtree_insert_path(node *tree, char *route, void * route_ptr)
{
    return rtree_insert_pathn(tree, route, strlen(route) , route_ptr);
}

node * rtree_insert_pathn(node *tree, char *route, int route_len, void * route_ptr)
{
    node * n = tree;
    edge * e = NULL;

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

    // branch the edge at correct position (avoid broken slugs)
    char *slug_s = strchr(route, '{');
    char *slug_e = strchr(route, '}');
    if ( slug_s && slug_e ) {
        if ( dl > (slug_s - route) && dl < (slug_e - route) ) {
            // break before '{'
            dl = slug_s - route;
        }
    }

    if ( dl == 0 ) {
        // not found, we should just insert a whole new edge
        node * child = rtree_create(3);
        rtree_add_child(n, strndup(route, route_len) , child);
        info("edge not found, insert one: %s\n", route);
        child->route_ptr = route_ptr;
        return child;
    } else if ( dl == e->pattern_len ) {    // fully-equal to the pattern of the edge

        char * subroute = route + dl;
        int    subroute_len = route_len - dl;

        // there are something more we can insert
        if ( subroute_len > 0 ) {
            return rtree_insert_pathn(e->child, subroute, subroute_len, route_ptr);
        } else {
            // no more,
            e->child->endpoint++; // make it as an endpoint, TODO: put the route value
            e->child->route_ptr = route_ptr;
            return e->child;
        }

    } else if ( dl < e->pattern_len ) {
        // printf("branch the edge dl: %d\n", dl);


        /* it's partically matched with the pattern,
         * we should split the end point and make a branch here...
         */
        node *c2; // child 1, child 2
        edge *e2; // edge 1, edge 2
        char * s2 = route + dl;
        int s2_len = 0;

        edge_branch(e, dl);

        // here is the new edge from.
        c2 = rtree_create(3);
        s2_len = route_len - dl;
        e2 = edge_create(strndup(s2, s2_len), s2_len, c2);
        // printf("edge right: %s\n", e2->pattern);
        rtree_append_edge(e->child, e2);

        // truncate the original edge pattern 
        free(e->pattern);
        e->pattern = strndup(e->pattern, dl);
        e->pattern_len = dl;

        // move n->edges to c1
        c2->endpoint++;
        c2->route_ptr = route_ptr;
        return c2;
    } else if ( dl > 0 ) {

    } else {
        printf("unexpected condition.");
        return NULL;
    }
    // token_array * t = split_route_pattern(route, strlen(route));
    // return rtree_insert_tokens(tree, t);
    // n->endpoint++;
    return n;
}

bool node_has_slug_edges(node *n) {
    bool found = FALSE;
    edge *e;
    for ( int i = 0 ; i < n->edge_len ; i++ ) {
        e = n->edges[i];
        e->has_slug = contains_slug(e->pattern);
        if (e->has_slug) 
            found = TRUE;
    }
    return found;
}


/**
 * branch the edge pattern at "dl" offset
 *
 */
void edge_branch(edge *e, int dl) {
    node *c1; // child 1, child 2
    edge *e1; // edge 1, edge 2
    char * s1 = e->pattern + dl;
    int s1_len = 0;

    edge **tmp_edges = e->child->edges;
    int   tmp_edge_len = e->child->edge_len;

    // the suffix edge of the leaf
    c1 = rtree_create(3);
    s1_len = e->pattern_len - dl;
    e1 = edge_create(strndup(s1, s1_len), s1_len, c1);
    // printf("edge left: %s\n", e1->pattern);

    // Migrate the child edges to the new edge we just created.
    for ( int i = 0 ; i < tmp_edge_len ; i++ ) {
        rtree_append_edge(c1, tmp_edges[i]);
        e->child->edges[i] = NULL;
    }
    e->child->edge_len = 0;
    e->child->endpoint--;

    info("branched pattern: %s", e1->pattern);

    rtree_append_edge(e->child, e1);
    c1->endpoint++;
}


edge * edge_create(char * pattern, int pattern_len, node * child) {
    edge * e = (edge*) malloc( sizeof(edge) );
    e->pattern = pattern;
    e->pattern_len = pattern_len;
    e->child = child;
    return e;
}

void edge_free(edge * e) {
    if (e->pattern) {
        free(e->pattern);
    }
    if ( e->child ) {
        rtree_free(e->child);
    }
}




void rtree_dump(node * n, int level) {
    if ( n->edge_len ) {
        if ( n->combined_pattern ) {
            printf(" regexp: %s", n->combined_pattern);
        }
        printf("\n");
        for ( int i = 0 ; i < n->edge_len ; i++ ) {
            edge * e = n->edges[i];
            print_indent(level);
            printf("  |-\"%s\"", e->pattern);

            if (e->has_slug) {
                printf(" slug:");
                printf("%s", compile_slug(e->pattern, e->pattern_len) );
            }

            if ( e->child && e->child->edges ) {
                rtree_dump( e->child, level + 1);
            }
            printf("\n");
        }
    }
}
