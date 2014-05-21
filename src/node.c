#include <config.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

// Jemalloc memory management
// #include <jemalloc/jemalloc.h>

// PCRE
#include <pcre.h>

// Judy array
// #include <Judy.h>

#include "r3.h"
#include "r3_define.h"
#include "r3_str.h"
#include "str_array.h"
#include "zmalloc.h"

// String value as the index http://judy.sourceforge.net/doc/JudySL_3x.htm


static int strndiff(char * d1, char * d2, unsigned int n) {
    char * o = d1;
    while ( *d1 == *d2 && n-- > 0 ) {
        d1++;
        d2++;
    }
    return d1 - o;
}

static int strdiff(char * d1, char * d2) {
    char * o = d1;
    while( *d1 == *d2 ) {
        d1++;
        d2++;
    }
    return d1 - o;
}


/**
 * Create a node object
 */
node * r3_tree_create(int cap) {
    node * n = (node*) zmalloc( sizeof(node) );

    n->edges = (edge**) zmalloc( sizeof(edge*) * cap );
    n->edge_len = 0;
    n->edge_cap = cap;

    n->routes = NULL;
    n->route_len = 0;
    n->route_cap = 0;

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
    if (tree->edges) {
        zfree(tree->edges);
    }
    if (tree->routes) {
        zfree(tree->routes);
    }
    /*
    if (tree->pcre_pattern)
        zfree(tree->pcre_pattern);
    if (tree->pcre_extra)
        zfree(tree->pcre_extra);
    */
    if (tree->combined_pattern)
        zfree(tree->combined_pattern);
    if (tree->ov)
        zfree(tree->ov);
    zfree(tree);
    tree = NULL;
}



/* parent node, edge pattern, child */
edge * r3_node_add_child(node * n, char * pat , node *child) {
    // find the same sub-pattern, if it does not exist, create one

    edge * e;

    e = r3_node_find_edge(n, pat);
    if (e) {
        return e;
    }

    e = r3_edge_create( pat, strlen(pat), child);
    r3_node_append_edge(n, e);
    // str_array_append(n->edge_patterns, pat);
    // assert( str_array_len(n->edge_patterns) == n->edge_len );
    return e;
}



void r3_node_append_edge(node *n, edge *e) {
    if (n->edges == NULL) {
        n->edge_cap = 3;
        n->edges = zmalloc(sizeof(edge) * n->edge_cap);
    }
    if (n->edge_len >= n->edge_cap) {
        n->edge_cap *= 2;
        edge ** p = zrealloc(n->edges, sizeof(edge) * n->edge_cap);
        if(p) {
            n->edges = p;
        }
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

    cpat = zcalloc(sizeof(char) * 128);
    if (cpat==NULL)
        return;

    p = cpat;

    strncat(p, "^", 1);
    p++;

    edge *e = NULL;
    for ( int i = 0 ; i < n->edge_len ; i++ ) {
        e = n->edges[i];
        if ( e->has_slug ) {
            char * slug_pat = slug_compile(e->pattern, e->pattern_len);
            strcat(p, slug_pat);
        } else {
            strncat(p++,"(", 1);

            strncat(p, e->pattern, e->pattern_len);
            p += e->pattern_len;

            strncat(p++,")", 1);
        }

        if ( i + 1 < n->edge_len && n->edge_len > 1 ) {
            strncat(p++,"|",1);
        }
    }

    info("pattern: %s\n",cpat);

    n->ov_cnt = (1 + n->edge_len) * 3;
    n->ov = (int*) zcalloc(sizeof(int) * n->ov_cnt);


    n->combined_pattern = cpat;
    n->combined_pattern_len = p - cpat;


    const char *error;
    int erroffset;
    unsigned int option_bits = 0;

    /*
    if (n->pcre_pattern)
        zfree(n->pcre_pattern);
    if (n->pcre_extra)
        zfree(n->pcre_extra);
    */

    // n->pcre_pattern;
    n->pcre_pattern = pcre_compile(
            n->combined_pattern,              /* the pattern */
            option_bits,                                /* default options */
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


match_entry * match_entry_createl(char * path, int path_len) {
    match_entry * entry = zmalloc(sizeof(match_entry));
    if(!entry)
        return NULL;
    entry->vars = str_array_create(3);
    entry->path = path;
    entry->path_len = path_len;
    entry->data = NULL;
    return entry;
}

void match_entry_free(match_entry * entry) {
    if (entry) {
        str_array_free(entry->vars);
        zfree(entry);
    }
}



/**
 * This function matches the URL path and return the left node
 *
 * r3_tree_matchl returns NULL when the path does not match. returns *node when the path matches.
 *
 * @param node         n        the root of the tree
 * @param char*        path     the URL path to dispatch
 * @param int          path_len the length of the URL path.
 * @param match_entry* entry match_entry is used for saving the captured dynamic strings from pcre result.
 */
node * r3_tree_matchl(const node * n, char * path, int path_len, match_entry * entry) {
    info("try matching: %s\n", path);

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

        // info("rc: %d\n", rc );
        if (rc < 0) {
            switch(rc)
            {
                case PCRE_ERROR_NOMATCH: printf("pcre: no match\n"); break;
                /*
                Handle other special cases if you like
                */
                default: printf("pcre matching error '%d' on pattern '%s'\n", rc, n->combined_pattern); break;
            }
            // does not match all edges, return NULL;
            return NULL;
        }


        for (i = 1; i < rc; i++)
        {
            char *substring_start = path + n->ov[2*i];
            int   substring_length = n->ov[2*i+1] - n->ov[2*i];
            // info("%2d: %.*s\n", i, substring_length, substring_start);

            if ( substring_length > 0) {
                int restlen = path_len - n->ov[1]; // fully match to the end
                // info("matched item => restlen:%d edges:%d i:%d\n", restlen, n->edge_len, i);

                e = n->edges[i - 1];

                if (entry && e->has_slug) {
                    // append captured token to entry
                    str_array_append(entry->vars , zstrndup(substring_start, substring_length));
                }
                if (restlen == 0 ) {
                    return e->child && e->child->endpoint > 0 ? e->child : NULL;
                }
                // get the length of orginal string: $0
                return r3_tree_matchl( e->child, path + (n->ov[1] - n->ov[0]), restlen, entry);
            }
        }
        // does not match
        return NULL;
    }

    if ( (e = r3_node_find_edge_str(n, path, path_len)) != NULL ) {
        int restlen = path_len - e->pattern_len;
        if (restlen == 0) {
            return e->child && e->child->endpoint > 0 ? e->child : NULL;
        }
        return r3_tree_matchl(e->child, path + e->pattern_len, restlen, entry);
    }
    return NULL;
}

route * r3_tree_match_route(const node *tree, match_entry * entry) {
    node *n;
    n = r3_tree_match_entry(tree, entry);
    if (n->routes && n->route_len > 0) {
        int i;
        for (i = 0; i < n->route_len ; i++ ) {
            if ( r3_route_cmp(n->routes[i], entry) == 0 ) {
                return n->routes[i];
            }
        }
    }
    return NULL;
}

inline edge * r3_node_find_edge_str(const node * n, char * str, int str_len) {
    int i = 0;
    int matched_idx = 0;
    char firstbyte = *str;
    for (; i < n->edge_len ; i++ ) {
        if ( firstbyte == *(n->edges[i]->pattern) ) {
            matched_idx = i;
            break;
        }
    }

    info("matching '%s' with '%s'\n", str, node_edge_pattern(n,i) );
    if ( strncmp( node_edge_pattern(n,matched_idx), str, node_edge_pattern_len(n,matched_idx) ) == 0 ) {
        return n->edges[matched_idx];
    }
    return NULL;
}



node * r3_node_create() {
    node * n = (node*) zmalloc( sizeof(node) );
    n->edges = NULL;
    n->edge_len = 0;
    n->edge_cap = 0;

    n->routes = NULL;
    n->route_len = 0;
    n->route_cap = 0;

    n->endpoint = 0;
    n->combined_pattern = NULL;
    n->pcre_pattern = NULL;
    return n;
}


route * r3_route_create(char * path) {
    return r3_route_createl(path, strlen(path));
}

void r3_route_free(route * route) {
    if (route) {
        zfree(route);
    }
}

route * r3_route_createl(char * path, int path_len) {
    route * info = zmalloc(sizeof(route));
    info->path = path;
    info->path_len = path_len;
    info->request_method = 0; // can be (GET || POST)

    info->data = NULL;

    info->host = NULL; // required host name
    info->host_len = 0;

    info->remote_addr_pattern = NULL;
    info->remote_addr_pattern_len = 0;
    return info;
}

node * r3_tree_insert_pathl(node *tree, char *path, int path_len, void * data)
{
    return r3_tree_insert_pathl_(tree, path, path_len, NULL , data);
}


/**
 * Return the last inserted node.
 */
node * r3_tree_insert_pathl_(node *tree, char *path, int path_len, route * route, void * data)
{
    node * n = tree;
    edge * e = NULL;

    /* length of common prefix */
    int prefix_len = 0;
    for( int i = 0 ; i < n->edge_len ; i++ ) {
        prefix_len = strndiff(path, n->edges[i]->pattern, n->edges[i]->pattern_len);

        // printf("prefix_len: %d   %s vs %s\n", prefix_len, path, n->edges[i]->pattern );

        // no common, consider insert a new edge
        if ( prefix_len > 0 ) {
            e = n->edges[i];
            break;
        }
    }

    // branch the edge at correct position (avoid broken slugs)
    char *slug_s;
    if ( (slug_s = inside_slug(path, path_len, path + prefix_len)) != NULL ) {
        prefix_len = slug_s - path;
    }

    // common prefix not found, insert a new edge for this pattern
    if ( prefix_len == 0 ) {
        // there are two more slugs, we should break them into several parts
        if ( slug_count(path, path_len) > 1 ) {
            int   slug_len;
            char *p = find_slug_placeholder(path, &slug_len);

#ifdef DEBUG
            assert(p);
#endif

            // find the next one
            if(p) {
                p = find_slug_placeholder(p + slug_len + 1, NULL);
            }
#ifdef DEBUG
            assert(p);
#endif

            // insert the first one edge, and break at "p"
            node * child = r3_tree_create(3);
            r3_node_add_child(n, zstrndup(path, (int)(p - path)), child);
            child->endpoint = 0;

            // and insert the rest part to the child
            return r3_tree_insert_pathl_(child, p, path_len - (int)(p - path),  route, data);
        } else {
            node * child = r3_tree_create(3);
            r3_node_add_child(n, zstrndup(path, path_len) , child);
            // info("edge not found, insert one: %s\n", path);
            child->data = data;
            child->endpoint++;

            if (route) {
                route->data = data;
                r3_node_append_route(child, route);
            }
            return child;
        }
    } else if ( prefix_len == e->pattern_len ) {    // fully-equal to the pattern of the edge

        char * subpath = path + prefix_len;
        int    subpath_len = path_len - prefix_len;

        // there are something more we can insert
        if ( subpath_len > 0 ) {
            return r3_tree_insert_pathl_(e->child, subpath, subpath_len, route, data);
        } else {
            // there are no more path to insert

            // see if there is an endpoint already
            if (e->child->endpoint > 0) {
                // XXX: return an error code instead of NULL
                return NULL;
            }
            e->child->endpoint++; // make it as an endpoint
            e->child->data = data;
            if (route) {
                route->data = data;
                r3_node_append_route(e->child, route);
            }
            return e->child;
        }

    } else if ( prefix_len < e->pattern_len ) {
        /* it's partially matched with the pattern,
         * we should split the end point and make a branch here...
         */
        char * s2 = path + prefix_len;
        int   s2_len = path_len - prefix_len;
        r3_edge_branch(e, prefix_len);
        return r3_tree_insert_pathl_(e->child, s2 , s2_len, route , data);
    } else {
        printf("unexpected route.");
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
    print_indent(level);

    printf("(o)");

    if ( n->combined_pattern ) {
        printf(" regexp:%s", n->combined_pattern);
    }

    printf(" endpoint:%d", n->endpoint);

    if (n->data) {
        printf(" data:%p", n->data);
    }
    printf("\n");

    for ( int i = 0 ; i < n->edge_len ; i++ ) {
        edge * e = n->edges[i];
        print_indent(level + 1);
        printf("|-\"%s\"", e->pattern);

        if ( e->child ) {
            printf("\n");
            r3_tree_dump( e->child, level + 1);
        }
        printf("\n");
    }
}


/**
 * return 0 == equal
 *
 * -1 == different route
 */
int r3_route_cmp(route *r1, match_entry *r2) {
    if (r1->request_method != 0) {
        if (0 == (r1->request_method & r2->request_method) ) {
            return -1;
        }
    }

    if ( r1->path && r2->path ) {
        if ( strcmp(r1->path, r2->path) != 0 ) {
            return -1;
        }
    }

    if ( r1->host && r2->host ) {
        if (strcmp(r1->host, r2->host) != 0 ) {
            return -1;
        }
    }

    if (r1->remote_addr_pattern) {
        /*
         * XXX: consider "netinet/in.h"
        if (r2->remote_addr) {
            inet_addr(r2->remote_addr);
        }
        */
        if ( strcmp(r1->remote_addr_pattern, r2->remote_addr) != 0 ) {
            return -1;
        }
    }
    return 0;
}


/**
 *
 */
void r3_node_append_route(node * n, route * r) {
    if (n->routes == NULL) {
        n->route_cap = 3;
        n->routes = zmalloc(sizeof(route) * n->route_cap);
    }
    if (n->route_len >= n->route_cap) {
        n->route_cap *= 2;
        n->routes = zrealloc(n->routes, sizeof(route) * n->route_cap);
    }
    n->routes[ n->route_len++ ] = r;
}


