#include <config.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

// Jemalloc memory management
#include <jemalloc/jemalloc.h>

// PCRE
#include <pcre.h>

// Judy array
// #include <Judy.h>

#include "r3_define.h"
#include "r3_str.h"
#include "r3.h"
#include "str_array.h"


// String value as the index http://judy.sourceforge.net/doc/JudySL_3x.htm

/**
 * Create a node object
 */
node * r3_tree_create(int cap) {
    node * n = (node*) malloc( sizeof(node) );

    n->edges = (edge**) malloc( sizeof(edge*) * 10 );
    n->edge_len = 0;
    n->edge_cap = cap;
    n->endpoint = 0;
    n->combined_pattern = NULL;
    n->pcre_pattern = NULL;
    n->pcre_extra = NULL;
    n->ov_cnt = 0;
    n->ov = NULL;
    return n;
}

void r3_tree_free(node * tree) {
    for (int i = 0 ; i < tree->edge_len ; i++ ) {
        if (tree->edges[i]) {
            r3_edge_free(tree->edges[ i ]);
        }
    }

    if (tree->combined_pattern)
        free(tree->combined_pattern);
    if (tree->pcre_pattern)
        free(tree->pcre_pattern);
    if (tree->pcre_extra)
        free(tree->pcre_extra);
    if (tree->ov) 
        free(tree->ov);
    free(tree->edges);
    // str_array_free(tree->edge_patterns);
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
    // str_array_append(n->edge_patterns, pat);
    // assert( str_array_len(n->edge_patterns) == n->edge_len );
    return e;
}



void r3_tree_append_edge(node *n, edge *e) {

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

edge * r3_node_find_edge(node * n, char * pat) {
    edge * e;
    for (int i = 0 ; i < n->edge_len ; i++ ) {
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

    for (int i = 0 ; i < n->edge_len ; i++ ) {
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

    n->ov_cnt = (1 + n->edge_len) * 3;
    n->ov = (int*) calloc(sizeof(int), n->ov_cnt);


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
        printf("PCRE compilation failed at offset %d: %s, pattern: %s\n", erroffset, error, n->combined_pattern);
        return;
    }
    n->pcre_extra = pcre_study(n->pcre_pattern, 0, &error);
    if (n->pcre_extra == NULL) {
        printf("PCRE study failed at offset %s\n", error);
        return;
    }
}


match_entry * match_entry_create(char * path, int path_len) {
    match_entry * entry = malloc(sizeof(match_entry));
    if(!entry)
        return NULL;
    entry->vars = str_array_create(3);
    entry->path = path;
    entry->path_len = path_len;
    entry->route_ptr = NULL;
    return entry;
}

void match_entry_free(match_entry * entry) {
    str_array_free(entry->vars);
    free(entry);
}


/**
 * This function matches the URL path and return the left node
 *
 * r3_tree_match returns NULL when the path does not match. returns *node when the path matches.
 *
 * @param node         n        the root of the tree
 * @param char*        path     the URL path to dispatch
 * @param int          path_len the length of the URL path.
 * @param match_entry* entry match_entry is used for saving the captured dynamic strings from pcre result.
 */
node * r3_tree_match(node * n, char * path, int path_len, match_entry * entry) {
    // info("try matching: %s\n", path);

    edge *e;
    int rc;
    int i;

    // if the pcre_pattern is found, and the pointer is not NULL, then it's
    // pcre pattern node, we use pcre_exec to match the nodes
    if (n->pcre_pattern) {
        info("pcre matching %s on %s\n", n->combined_pattern, path);

        rc = pcre_exec(
                n->pcre_pattern,   /* the compiled pattern */

                // PCRE Study makes this slow
                NULL, // n->pcre_extra,     /* no extra data - we didn't study the pattern */
                path,              /* the subject string */
                path_len,          /* the length of the subject */
                0,                 /* start at offset 0 in the subject */
                0,                 /* default options */
                n->ov,           /* output vector for substring information */
                n->ov_cnt);      /* number of elements in the output vector */

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
            char *substring_start = path + n->ov[2*i];
            int   substring_length = n->ov[2*i+1] - n->ov[2*i];
            info("%2d: %.*s\n", i, substring_length, substring_start);

            if ( substring_length > 0) {
                int restlen = path_len - n->ov[2*i+1]; // fully match to the end
                info("matched item => restlen:%d edges:%d i:%d\n", restlen, n->edge_len, i);

                e = n->edges[i - 1];

                if (entry && e->has_slug) {
                    // append captured token to entry
                    str_array_append(entry->vars , strndup(substring_start, substring_length));
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

    if ( (e = r3_node_find_edge_str(n, path, path_len)) != NULL ) {
        int restlen = path_len - e->pattern_len;
        if(restlen > 0) {
            return r3_tree_match(e->child, path + e->pattern_len, restlen, entry);
        }
        return e->child;
    }
    return NULL;
}


inline edge * r3_node_find_edge_str(node * n, char * str, int str_len) {
    int i = 0;
    for (; i < n->edge_len ; i++ ) {
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
    int i = 0;
    for (; i < tokens->len ; i++ ) {
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
    n->edge_len = 0;
    n->edge_cap = 0;
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
        // e->pattern = strndup(e->pattern, dl);
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
    for ( int i = 0 ; i < n->edge_len ; i++ ) {
        e = n->edges[i];
        e->has_slug = contains_slug(e->pattern);
        if (e->has_slug) 
            found = TRUE;
    }
    return found;
}



void r3_tree_dump(node * n, int level) {
    if ( n->edge_len ) {
        if ( n->combined_pattern ) {
            printf(" regexp:%s", n->combined_pattern);
        }

        printf(" endpoint:%d\n", n->endpoint);

        for ( int i = 0 ; i < n->edge_len ; i++ ) {
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
