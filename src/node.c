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
node * r3_tree_create(int cap) {
    node * n = (node*) malloc( sizeof(node) );

    n->edges = (edge**) malloc( sizeof(edge*) * 10 );
    n->r3_edge_len = 0;
    n->r3_edge_cap = 10;
    n->endpoint = 0;
    n->combined_pattern = NULL;
    return n;
}

void r3_tree_free(node * tree) {
    for (int i = 0 ; i < tree->r3_edge_len ; i++ ) {
        if (tree->edges[i]) {
            r3_edge_free(tree->edges[ i ]);
        }
    }

    if (tree->combined_pattern)
        free(tree->combined_pattern);

    free(tree->edges);
    // str_array_free(tree->r3_edge_patterns);
    free(tree);
    tree = NULL;
}



/* parent node, edge pattern, child */
edge * r3_tree_add_child(node * n, char * pat , node *child) {
    // find the same sub-pattern, if it does not exist, create one

    edge * e;

    e = r3_node_find_edge(n, pat);
    if (e) {
        return e;
    }

    e = r3_edge_create( pat, strlen(pat), child);
    r3_tree_append_edge(n, e);
    // str_array_append(n->r3_edge_patterns, pat);
    // assert( str_array_len(n->r3_edge_patterns) == n->r3_edge_len );
    return e;
}



void r3_tree_append_edge(node *n, edge *e) {

    if (!n->edges) {
        n->r3_edge_cap = 3;
        n->edges = malloc(sizeof(edge) * n->r3_edge_cap);
    }
    if (n->r3_edge_len >= n->r3_edge_cap) {
        n->r3_edge_cap *= 2;
        n->edges = realloc(n->edges, sizeof(edge) * n->r3_edge_cap);
    }
    n->edges[ n->r3_edge_len++ ] = e;
}


edge * r3_node_find_edge(node * n, char * pat) {
    edge * e;
    for (int i = 0 ; i < n->r3_edge_len ; i++ ) {
        e = n->edges[i];
        if ( strcmp(e->pattern, pat) == 0 ) {
            return e;
        }
    }
    return NULL;
}

void r3_tree_compile(node *n)
{
    bool use_slug = r3_node_has_slug_edges(n);
    if ( use_slug ) {
        r3_tree_compile_patterns(n);
    } else {
        // use normal text matching...
        n->combined_pattern = NULL;
    }

    for (int i = 0 ; i < n->r3_edge_len ; i++ ) {
        r3_tree_compile(n->edges[i]->child);
    }
}


/**
 * This function combines ['/foo', '/bar', '/{slug}'] into (/foo)|(/bar)|/([^/]+)}
 *
 */
void r3_tree_compile_patterns(node * n) {
    char * cpat;
    char * p;

    cpat = calloc(sizeof(char),128);
    if (cpat==NULL)
        return;

    p = cpat;

    strncat(p, "^", 1);
    p++;

    edge *e = NULL;
    for ( int i = 0 ; i < n->r3_edge_len ; i++ ) {
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

        if ( i + 1 < n->r3_edge_len ) {
            strncat(p++,"|",1);
        }
    }
    n->combined_pattern = cpat;
    n->combined_pattern_len = p - cpat;


    const char *error;
    int erroffset;
    unsigned int option_bits;

    if (n->pcre_pattern)
        free(n->pcre_pattern);
    if (n->pcre_extra)
        free(n->pcre_extra);

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




node * r3_tree_match(node * n, char * path, int path_len, match_entry * entry) {
    // info("try matching: %s\n", path);

    edge *e;
    if (n->combined_pattern && n->pcre_pattern) {
        info("pcre matching %s on %s\n", n->combined_pattern, path);
        // int ovector_count = (n->r3_edge_len + 1) * 2;
        int ovector_count = 30;
        int ovector[ovector_count];
        int rc;
        int i;

        rc = pcre_exec(
                n->pcre_pattern,   /* the compiled pattern */

                // PCRE Study makes this slow
                NULL, // n->pcre_extra,     /* no extra data - we didn't study the pattern */
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

        for (i = 1; i < rc; i++)
        {
            char *substring_start = path + ovector[2*i];
            int   substring_length = ovector[2*i+1] - ovector[2*i];
            info("%2d: %.*s\n", i, substring_length, substring_start);

            if ( substring_length > 0) {
                int restlen = path_len - ovector[2*i+1]; // fully match to the end
                info("matched item => restlen:%d edges:%d i:%d\n", restlen, n->r3_edge_len, i);

                e = n->edges[i - 1];

                if (entry && e->has_slug) {
                    // entry->
                }
                if (restlen == 0) {
                    return e->child;
                }
                return r3_tree_match( e->child, substring_start + substring_length, restlen, entry);
            }
        }
        // does not match
        return NULL;
    }

    e = r3_node_find_edge_str(n, path, path_len);
    if (e) {
        int restlen = path_len - e->pattern_len;
        if(restlen) {
            return r3_tree_match(e->child, path + e->pattern_len, restlen, entry);
        } else {
            return e->child;
        }
    }
    return NULL;
}

#define node_edge_pattern(node,i) node->edges[i]->pattern
#define node_edge_pattern_len(node,i) node->edges[i]->pattern_len

inline edge * r3_node_find_edge_str(node * n, char * str, int str_len) {
    int i = 0;
    for (; i < n->r3_edge_len ; i++ ) {
        info("matching '%s' with '%s'\n", str, node_edge_pattern(n,i) );
        if ( strncmp( node_edge_pattern(n,i), str, node_edge_pattern_len(n,i) ) == 0 ) {
            return n->edges[i];
        }
    }
    return NULL;
}


node * r3_tree_lookup(node * tree, char * path, int path_len) {
    str_array * tokens = split_route_pattern(path, path_len);

    node * n = tree;
    edge * e = NULL;
    for ( int i = 0 ; i < tokens->len ; i++ ) {
        e = r3_node_find_edge(n, str_array_fetch(tokens, i) );
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

node * r3_node_create() {
    node * n = (node*) malloc( sizeof(node) );
    n->edges = NULL;
    n->r3_edge_len = 0;
    n->r3_edge_cap = 0;
    n->endpoint = 0;
    n->combined_pattern = NULL;
    n->pcre_pattern = NULL;
    return n;
}


node * r3_tree_insert_path(node *tree, char *route, void * route_ptr)
{
    return r3_tree_insert_pathn(tree, route, strlen(route) , route_ptr);
}

node * r3_tree_insert_pathn(node *tree, char *route, int route_len, void * route_ptr)
{
    node * n = tree;
    edge * e = NULL;

    char * p = route;

    /* length of common prefix */
    int dl = 0;
    for( int i = 0 ; i < n->r3_edge_len ; i++ ) {
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
        node * child = r3_tree_create(3);
        r3_tree_add_child(n, strndup(route, route_len) , child);
        info("edge not found, insert one: %s\n", route);
        child->route_ptr = route_ptr;
        child->endpoint++;
        return child;
    } else if ( dl == e->pattern_len ) {    // fully-equal to the pattern of the edge

        char * subroute = route + dl;
        int    subroute_len = route_len - dl;

        // there are something more we can insert
        if ( subroute_len > 0 ) {
            return r3_tree_insert_pathn(e->child, subroute, subroute_len, route_ptr);
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

        r3_edge_branch(e, dl);

        // here is the new edge from.
        c2 = r3_tree_create(3);
        s2_len = route_len - dl;
        e2 = r3_edge_create(strndup(s2, s2_len), s2_len, c2);
        // printf("edge right: %s\n", e2->pattern);
        r3_tree_append_edge(e->child, e2);

        // truncate the original edge pattern 
        free(e->pattern);
        e->pattern = strndup(e->pattern, dl);
        e->pattern_len = dl;

        // move n->edges to c1
        c2->endpoint++;
        c2->route_ptr = route_ptr;
        return c2;
    } else {
        printf("unexpected condition.");
        return NULL;
    }
    return n;
}

bool r3_node_has_slug_edges(node *n) {
    bool found = FALSE;
    edge *e;
    for ( int i = 0 ; i < n->r3_edge_len ; i++ ) {
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
void r3_edge_branch(edge *e, int dl) {
    node *c1; // child 1, child 2
    edge *e1; // edge 1, edge 2
    char * s1 = e->pattern + dl;
    int s1_len = 0;

    edge **tmp_edges = e->child->edges;
    int   tmp_r3_edge_len = e->child->r3_edge_len;

    // the suffix edge of the leaf
    c1 = r3_tree_create(3);
    s1_len = e->pattern_len - dl;
    e1 = r3_edge_create(strndup(s1, s1_len), s1_len, c1);
    // printf("edge left: %s\n", e1->pattern);

    // Migrate the child edges to the new edge we just created.
    for ( int i = 0 ; i < tmp_r3_edge_len ; i++ ) {
        r3_tree_append_edge(c1, tmp_edges[i]);
        e->child->edges[i] = NULL;
    }
    e->child->r3_edge_len = 0;
    e->child->endpoint--;

    info("branched pattern: %s\n", e1->pattern);

    r3_tree_append_edge(e->child, e1);
    c1->endpoint++;
}


edge * r3_edge_create(char * pattern, int pattern_len, node * child) {
    edge * e = (edge*) malloc( sizeof(edge) );
    e->pattern = pattern;
    e->pattern_len = pattern_len;
    e->child = child;
    return e;
}

void r3_edge_free(edge * e) {
    if (e->pattern) {
        free(e->pattern);
    }
    if ( e->child ) {
        r3_tree_free(e->child);
    }
}




void r3_tree_dump(node * n, int level) {
    if ( n->r3_edge_len ) {
        if ( n->combined_pattern ) {
            printf(" regexp:%s", n->combined_pattern);
        }

        printf(" endpoint:%d\n", n->endpoint);

        for ( int i = 0 ; i < n->r3_edge_len ; i++ ) {
            edge * e = n->edges[i];
            print_indent(level);
            printf("  |-\"%s\"", e->pattern);

            if (e->has_slug) {
                printf(" slug:");
                printf("%s", compile_slug(e->pattern, e->pattern_len) );
            }

            if ( e->child && e->child->edges ) {
                r3_tree_dump( e->child, level + 1);
            }
            printf("\n");
        }
    }
}

/*
char * r3_node_trace(node * n) {

}
*/
