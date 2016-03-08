#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>

// PCRE
#include <pcre.h>

#include "r3.h"
#include "r3_slug.h"
#include "slug.h"
#include "str.h"
#include "r3_debug.h"
#include "zmalloc.h"

#ifdef __GNUC__
#	define likely(x)   __builtin_expect(!!(x), 1)
#	define unlikely(x) __builtin_expect(!!(x), 0)
#else
#	define likely(x)   !!(x)
#	define unlikely(x) !!(x)
#endif

#define CHECK_PTR(ptr) if (ptr == NULL) return NULL;

// String value as the index http://judy.sourceforge.net/doc/JudySL_3x.htm


static int strndiff(char * d1, char * d2, unsigned int n) {
    char * o = d1;
    while ( *d1 == *d2 && n-- > 0 ) {
        d1++;
        d2++;
    }
    return d1 - o;
}

/*
static int strdiff(char * d1, char * d2) {
    char * o = d1;
    while( *d1 == *d2 ) {
        d1++;
        d2++;
    }
    return d1 - o;
}
*/


/**
 * Create a node object
 */
R3Node * r3_tree_create(int cap) {
    R3Node * n = (R3Node*) zmalloc( sizeof(R3Node) );
    CHECK_PTR(n);

    n->edges = zmalloc(sizeof(R3Edge) * cap);
    CHECK_PTR(n->edges);
    n->edge_len = 0;
    n->edge_cap = cap;

    n->routes = NULL;
    n->route_len = 0;
    n->route_cap = 0;

    n->endpoint = 0;
    n->compare_type = NODE_COMPARE_STR;
    n->combined_pattern = NULL;
    n->pcre_pattern = NULL;
    n->pcre_extra = NULL;
    n->data = NULL;
    return n;
}

void r3_tree_free(R3Node * tree) {
    if (tree->edges) {
        for (int j=0;j<tree->edge_len;j++) {
            r3_edge_free(tree->edges[j]);
        }
        zfree(tree->edges);
    }
    if (tree->routes) {
        for (int k=0;k<tree->route_len;k++) {
            r3_route_free(tree->routes[k]);
        }
        zfree(tree->routes);
    }
    if (tree->pcre_pattern) {
        pcre_free(tree->pcre_pattern);
    }
#ifdef PCRE_STUDY_JIT_COMPILE
    if (tree->pcre_extra) {
        pcre_free_study(tree->pcre_extra);
    }
#endif
    zfree(tree->combined_pattern);
    zfree(tree);
    tree = NULL;
}



/**
 * Connect two node objects, and create an edge object between them.
 */
R3Edge * r3_node_connectl(R3Node * n, const char * pat, int len, int dupl, R3Node *child) {
    // find the same sub-pattern, if it does not exist, create one
    R3Edge * e;

    e = r3_node_find_edge(n, pat, len);
    if (e) {
        return e;
    }

    if (dupl) {
        pat = zstrndup(pat, len);
    }
    e = r3_edge_createl(pat, len, child);
    CHECK_PTR(e);
    R3Edge * e2 = r3_node_append_edge(n, e);
    return e2;
}

R3Edge * r3_node_append_edge(R3Node *n, R3Edge *e)
{
    if (n->edges == NULL) {
        n->edge_cap = 3;
        n->edges = zmalloc(sizeof(R3Edge) * n->edge_cap);
    }
    if (n->edge_len >= n->edge_cap) {
        n->edge_cap *= 2;
        R3Edge * p = zrealloc(n->edges, sizeof(R3Edge) * n->edge_cap);
        if(p) {
            n->edges = p;
        }
    }

    // Insert edge into edge array
    n->edges[n->edge_len] = e;
    return n->edges[n->edge_len++];
}


/**
 * Find the existing edge with specified pattern (include slug)
 *
 * if "pat" is a slug, we should compare with the specified pattern.
 */
R3Edge * r3_node_find_edge(const R3Node * n, const char * pat, int pat_len) {
    R3Edge * e;
    int i;
    for (i = 0 ; i < n->edge_len ; i++ ) {
        e = n->edges[i];
        // there is a case: "{foo}" vs "{foo:xxx}",
        // we should return the match result: full-match or partial-match 
        if (strcmp(e->pattern, pat) == 0) {
            return e;
        }
    }
    return NULL;
}

int r3_tree_compile(R3Node *n, char **errstr)
{
    int i;
    int ret = 0;
    bool use_slug = r3_node_has_slug_edges(n);
    if ( use_slug ) {
        if ( (ret = r3_tree_compile_patterns(n, errstr)) ) {
            return ret;
        }
    } else {
        // use normal text matching...
        n->combined_pattern = NULL;
    }

    for (i = 0 ; i < n->edge_len ; i++ ) {
        if ((ret = r3_tree_compile(n->edges[i]->child, errstr))) {
            return ret; // stop here if error occurs
        }
    }
    return 0;
}


/**
 * This function combines ['/foo', '/bar', '/{slug}'] into (/foo)|(/bar)|/([^/]+)}
 *
 * Return -1 if error occurs
 * Return 0 if success
 */
int r3_tree_compile_patterns(R3Node * n, char **errstr) {
    R3Edge *e = NULL;
    char * p;
    char * cpat = zcalloc(sizeof(char) * 64 * 3); // XXX
    if (!cpat) {
        asprintf(errstr, "Can not allocate memory");
        return -1;
    }

    p = cpat;
    int opcode_cnt = 0;
    int i = 0;
    for (; i < n->edge_len ; i++) {
        e = n->edges[i];
        if (e->opcode) {
            opcode_cnt++;
        }

        if (e->has_slug) {
            // compile "foo/{slug}" to "foo/[^/]+"
            char * slug_pat = r3_slug_compile(e->pattern, e->pattern_len);
            strcat(p, slug_pat);
            zfree(slug_pat);
        } else {
            strncat(p,"^(", 2);
            p += 2;

            strncat(p, e->pattern, e->pattern_len);
            p += e->pattern_len;

            strncat(p++,")", 1);
        }

        if ( i + 1 < n->edge_len && n->edge_len > 1 ) {
            strncat(p++,"|",1);
        }
    }

    info("pattern: %s\n",cpat);

    // if all edges use opcode, we should skip the combined_pattern.
    if ( opcode_cnt == n->edge_len ) {
        // zfree(cpat);
        n->compare_type = NODE_COMPARE_OPCODE;
    } else {
        n->compare_type = NODE_COMPARE_PCRE;
    }

    n->combined_pattern = cpat;

    const char *pcre_error;
    int pcre_erroffset;
    unsigned int option_bits = 0;

    n->ov_cnt = (1 + n->edge_len) * 3;

    if (n->pcre_pattern) {
        pcre_free(n->pcre_pattern);
    }
    n->pcre_pattern = pcre_compile(
            n->combined_pattern,              /* the pattern */
            option_bits,                                /* default options */
            &pcre_error,               /* for error message */
            &pcre_erroffset,           /* for error offset */
            NULL);                /* use default character tables */
    if (n->pcre_pattern == NULL) {
        if (errstr) {
            asprintf(errstr, "PCRE compilation failed at offset %d: %s, pattern: %s", pcre_erroffset, pcre_error, n->combined_pattern);
        }
        return -1;
    }
#ifdef PCRE_STUDY_JIT_COMPILE
    if (n->pcre_extra) {
        pcre_free_study(n->pcre_extra);
    }
    n->pcre_extra = pcre_study(n->pcre_pattern, 0, &pcre_error);
    if (n->pcre_extra == NULL) {
        if (errstr) {
            asprintf(errstr, "PCRE study failed at offset %s, pattern: %s", pcre_error, n->combined_pattern);
        }
        return -1;
    }
#endif
    return 0;
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
R3Node * r3_tree_matchl(const R3Node * n, const char * path, int path_len, match_entry * entry) {
    info("try matching: %s\n", path);

    R3Edge *e;
    unsigned int i;
    unsigned int restlen;

    const char *pp;
    const char *pp_end;

    if (n->compare_type == NODE_COMPARE_OPCODE) {
        pp_end = path + path_len;

        for (i = n->edge_len; i--; ) {
            pp = path;
            e = n->edges[i];
            switch(e->opcode) {
                case OP_EXPECT_NOSLASH:
                    while (*pp != '/' && pp < pp_end) pp++;
                    break;
                case OP_EXPECT_MORE_ALPHA:
                    while ( isalpha(*pp) && pp < pp_end) pp++;
                    break;
                case OP_EXPECT_MORE_DIGITS:
                    while ( isdigit(*pp) && pp < pp_end) pp++;
                    break;
                case OP_EXPECT_MORE_WORDS:
                    while ( (isdigit(*pp) || isalpha(*pp)) && pp < pp_end) pp++;
                    break;
                case OP_EXPECT_NODASH:
                    while (*pp != '-' && pp < pp_end) pp++;
                    break;
            }
            // check match
            if ((pp - path) > 0) {
                if (entry) {
                    str_array_append(entry->vars , zstrndup(path, pp - path));
                }
                restlen = pp_end - pp;
                if (restlen == 0) {
                    return e->child && e->child->endpoint > 0 ? e->child : NULL;
                }
                return r3_tree_matchl(e->child, pp, restlen, entry);
            }
        }
    }

    // if the pcre_pattern is found, and the pointer is not NULL, then it's
    // pcre pattern node, we use pcre_exec to match the nodes
    if (n->pcre_pattern) {
        const char *substring_start = NULL;
        int   substring_length = 0;
        int   ov[ n->ov_cnt ];
        int   rc;

        info("pcre matching %s on %s\n", n->combined_pattern, path);

        rc = pcre_exec(
                n->pcre_pattern, /* the compiled pattern */
                n->pcre_extra,
                path,         /* the subject string */
                path_len,     /* the length of the subject */
                0,            /* start at offset 0 in the subject */
                0,            /* default options */
                ov,           /* output vector for substring information */
                n->ov_cnt);      /* number of elements in the output vector */

        // does not match all edges, return NULL;
        if (rc < 0) {
#ifdef DEBUG
            printf("pcre rc: %d\n", rc );
            switch(rc)
            {
                case PCRE_ERROR_NOMATCH:
                    printf("pcre: no match '%s' on pattern '%s'\n", path, n->combined_pattern);
                    break;

                // Handle other special cases if you like
                default:
                    printf("pcre matching error '%d' '%s' on pattern '%s'\n", rc, path, n->combined_pattern);
                    break;
            }
#endif
            return NULL;
        }



        restlen = path_len - ov[1]; // if it's fully matched to the end (rest string length)

        if (restlen == 0 ) {
            // Check the substring to decide we should go deeper on which edge 
            for (i = 1; i < rc; i++)
            {
                substring_length = ov[2*i+1] - ov[2*i];

                // if it's not matched for this edge, just skip them quickly
                if (substring_length == 0)
                    continue;

                substring_start = path + ov[2*i];
                e = n->edges[i - 1];

                if (entry && e->has_slug) {
                    // append captured token to entry
                    str_array_append(entry->vars , zstrndup(substring_start, substring_length));
                }

                // since restlen == 0 return the edge quickly.
                return e->child && e->child->endpoint > 0 ? e->child : NULL;
            }
        }


        // Check the substring to decide we should go deeper on which edge 
        for (i = 1; i < rc; i++)
        {
            substring_length = ov[2*i+1] - ov[2*i];

            // if it's not matched for this edge, just skip them quickly
            if ( substring_length == 0) {
                continue;
            }

            substring_start = path + ov[2*i];
            e = n->edges[i - 1];

            if (entry && e->has_slug) {
                // append captured token to entry
                str_array_append(entry->vars , zstrndup(substring_start, substring_length));
            }

            // get the length of orginal string: $0
            return r3_tree_matchl( e->child, path + (ov[1] - ov[0]), restlen, entry);
        }
        // does not match
        return NULL;
    }

    if ((e = r3_node_find_edge_str(n, path, path_len)) != NULL) {
        restlen = path_len - e->pattern_len;
        if (restlen == 0) {
            return e->child && e->child->endpoint > 0 ? e->child : NULL;
        }
        return r3_tree_matchl(e->child, path + e->pattern_len, restlen, entry);
    }
    return NULL;
}



R3Route * r3_tree_match_route(const R3Node *tree, match_entry * entry) {
    R3Node *n;
    int i;
    n = r3_tree_match_entry(tree, entry);
    if (n && n->routes && n->route_len > 0) {
        for (i = n->route_len; i--; ) {
            if ( r3_route_cmp(n->routes[i], entry) == 0 ) {
                // Add slugs from found route to match_entry
                for (int j = 0; j < n->routes[i]->slugs_len; j++) {
                    str_array_append_slug(entry->vars , n->routes[i]->slugs[j]);
                }
                return n->routes[i];
            }
        }
    }
    return NULL;
}

inline R3Edge * r3_node_find_edge_str(const R3Node * n, const char * str, int str_len) {
    R3Edge * e;
    unsigned int i;
    char firstbyte = *str;
    for (i = n->edge_len; i--; ) {
        e = n->edges[i];
        if (firstbyte == e->pattern[0]) {
            if (strncmp(e->pattern, str, e->pattern_len) == 0) {
                return n->edges[i];
            }
            return NULL;
        }
    }
    return NULL;
}

R3Node * r3_node_create() {
    R3Node * n = (R3Node*) zmalloc( sizeof(R3Node) );
    CHECK_PTR(n);
    n->edges = NULL;
    n->edge_len = 0;
    n->edge_cap = 0;

    n->routes = NULL;
    n->route_len = 0;
    n->route_cap = 0;

    n->endpoint = 0;
    n->combined_pattern = NULL;
    n->pcre_pattern = NULL;
    n->pcre_extra = NULL;
    n->data = NULL;
    return n;
}

void r3_route_free(R3Route * route) {
    assert(route);
    for ( int i = 0; i < route->slugs_len ; i++ ) {
        if (route->slugs[ i ]) {
            zfree(route->slugs[i]);
        }
    }
    zfree(route->slugs);
    zfree(route);
}

static bool router_slugs_full(const R3Route * route) {
    return route->slugs_len >= route->slugs_cap;
}

static bool router_slugs_resize(R3Route * route, int new_cap) {
    route->slugs = zrealloc(route->slugs, sizeof(char**) * new_cap);
    route->slugs_cap = new_cap;
    return route->slugs != NULL;
}

static bool router_append_slug(R3Route * route, char * slug) {
    if ( router_slugs_full(route) ) {
        bool ret = router_slugs_resize(route, route->slugs_cap + 20);
        if (ret == false ) {
            return false;
        }
    }
    route->slugs[ route->slugs_len++ ] = slug;
    return true;
}

static void get_slugs(const char * path, int path_len, R3Route * route) {
    char *plh = path;
    int l = 0;
    int namel;
    char *name;
    while(1) {
        plh = r3_slug_find_placeholder(plh+l,&l);
        if (plh == NULL) break;
        namel = 0;
        name = r3_slug_find_name(plh,&namel);
        router_append_slug(route,zstrndup(name,namel));
        if ((plh + l) >= (path + path_len)) break;
    }
}

R3Route * r3_route_createl(const char * path, int path_len) {
    R3Route * info = zmalloc(sizeof(R3Route));
    CHECK_PTR(info);
    info->slugs_cap = 3;
    info->slugs_len = 0;
    info->slugs = (char**) zmalloc( sizeof(char*) * info->slugs_cap);
    get_slugs(path, path_len, info);
    info->path = (char*) path;
    info->path_len = path_len;
    info->request_method = 0; // can be (GET || POST)

    info->data = NULL;

    info->host = NULL; // required host name
    info->host_len = 0;

    info->remote_addr_pattern = NULL;
    info->remote_addr_pattern_len = 0;
    return info;
}


/**
 * Helper function for creating routes from request URI path and request method
 *
 * method (int): METHOD_GET, METHOD_POST, METHOD_PUT, METHOD_DELETE ...
 */
R3Route * r3_tree_insert_routel_ex(R3Node *tree, int method, const char *path, int path_len, void *data, char **errstr) {
    R3Route *r = r3_route_createl(path, path_len);
    CHECK_PTR(r);
    r->request_method = method; // ALLOW GET OR POST METHOD
    R3Node * ret = r3_tree_insert_pathl_ex(tree, path, path_len, r, data, errstr);
    if (!ret) {
        // failed insert
        r3_route_free(r);
        return NULL;
    }
    return r;
}



/**
 * Find common prefix from the edges of the node.
 *
 * Some cases of the common prefix:
 *
 * 1.  "/foo/{slug}" vs "/foo/bar"                      => common prefix = "/foo/"
 * 2.  "{slug}/hate" vs "{slug}/bar"                    => common prefix = "{slug}/"
 * 2.  "/z/{slug}/hate" vs "/z/{slog}/bar"              => common prefix = "/z/"
 * 3.  "{slug:xxx}/hate" vs "{slug:yyy}/bar"            => common prefix = ""
 * 4.  "aaa{slug:xxx}/hate" vs "aab{slug:yyy}/bar"      => common prefix = "aa"
 * 5.  "/foo/{slug}/hate" vs "/fo{slug}/bar"            => common prefix = "/fo"
 */
R3Edge * r3_node_find_common_prefix(R3Node *n, const char *path, int path_len, int *prefix_len, char **errstr) {
    int i = 0;
    int prefix = 0;
    *prefix_len = 0;
    R3Edge *e = NULL;
    for(i = 0 ; i < n->edge_len ; i++ ) {
        // ignore all edges with slug
        prefix = strndiff( (char*) path, n->edges[i]->pattern, n->edges[i]->pattern_len);

        // no common, consider insert a new edge
        if ( prefix > 0 ) {
            e = n->edges[i];
            break;
        }
    }

    // found common prefix edge
    if (prefix > 0) {
        r3_slug_t *slug;
        int ret = 0;
        const char *offset = path;
        const char *p = path + prefix;

        slug = r3_slug_new(path, path_len);

        do {
            ret = r3_slug_parse(slug, path, path_len, offset, errstr);
            // found slug
            if (ret == 1) {
                // inside slug, backtrace to the begin of the slug
                if ( p >= slug->begin && p <= slug->end ) {
                    prefix = slug->begin - path - 1;
                    break;
                } else if ( p < slug->begin ) {
                    break;
                } else if ( p >= slug->end && p < (path + path_len) ) {
                    offset = slug->end + 1;
                    prefix = p - path;
                    continue;
                } else {
                    break;
                }
            } else if (ret == -1) {
                r3_slug_free(slug);
                return NULL;
            } else {
                break;
            }
        } while(ret == 1);

        // free the slug
        r3_slug_free(slug);
    }

    *prefix_len = prefix;
    return e;
}




/**
 * Return the last inserted node.
 */
R3Node * r3_tree_insert_pathl_ex(R3Node *tree, const char *path, int path_len, R3Route * route, void * data, char **errstr)
{
    R3Node * n = tree;

    // common edge
    R3Edge * e = NULL;

    // If there is no path to insert at the node, we just increase the mount
    // point on the node and append the route.
    if (path_len == 0) {
        tree->endpoint++;
        if (route) {
            route->data = data;
            r3_node_append_route(tree, route);
        }
        return tree;
    }

    /* length of common prefix */
    int prefix_len = 0;
    char *err = NULL;
    e = r3_node_find_common_prefix(tree, path, path_len, &prefix_len, &err);
    if (err) {
        // copy the error message pointer
        if (errstr) *errstr = err;
        return NULL;
    }

    const char * subpath = path + prefix_len;
    const int    subpath_len = path_len - prefix_len;

    // common prefix not found, insert a new edge for this pattern
    if ( prefix_len == 0 ) {
        // there are two more slugs, we should break them into several parts
        int slug_cnt = r3_slug_count(path, path_len, errstr);
        if (slug_cnt == -1) {
            return NULL;
        }

        if ( slug_cnt > 1 ) {
            int   slug_len;
            char *p = r3_slug_find_placeholder(path, &slug_len);

#ifdef DEBUG
            assert(p);
#endif

            // find the next one '{', then break there
            if(p) {
                p = r3_slug_find_placeholder(p + slug_len + 1, NULL);
            }
#ifdef DEBUG
            assert(p);
#endif

            // insert the first one edge, and break at "p"
            R3Node * child = r3_tree_create(3);
            CHECK_PTR(child);

            r3_node_connectl(n, path, p - path, 1, child); // duplicate

            // and insert the rest part to the child
            return r3_tree_insert_pathl_ex(child, p, path_len - (int)(p - path),  route, data, errstr);

        } else {
            if (slug_cnt == 1) {
                // there is one slug, let's see if it's optimiz-able by opcode
                int   slug_len = 0;
                char *slug_p = r3_slug_find_placeholder(path, &slug_len);
                int   slug_pattern_len = 0;
                char *slug_pattern = r3_slug_find_pattern(slug_p, &slug_pattern_len);

                int opcode = 0;
                // if there is a pattern defined.
                if (slug_pattern_len) {
                    char *cpattern = r3_slug_compile(slug_pattern, slug_pattern_len);
                    opcode = r3_pattern_to_opcode(cpattern, strlen(cpattern));
                    zfree(cpattern);
                } else {
                    opcode = OP_EXPECT_NOSLASH;
                }


                // if the slug starts after one+ charactor, for example foo{slug}
                R3Node *c1;
                if (slug_p > path) {
                    c1 = r3_tree_create(3);
                    CHECK_PTR(c1);
                    r3_node_connectl(n, path, slug_p - path, 1, c1); // duplicate
                } else {
                    c1 = n;
                }

                R3Node * c2 = r3_tree_create(3);
                CHECK_PTR(c2);

                R3Edge * op_edge = r3_node_connectl(c1, slug_p, slug_len , 1, c2);
                if(opcode) {
                    op_edge->opcode = opcode;
                }

                int restlen = path_len - ((slug_p - path) + slug_len);

                if (restlen) {
                    return r3_tree_insert_pathl_ex(c2, slug_p + slug_len, restlen, route, data, errstr);
                }

                c2->data = data;
                c2->endpoint++;
                if (route) {
                    route->data = data;
                    r3_node_append_route(c2, route);
                }
                return c2;
            }
            // only one slug
            R3Node * child = r3_tree_create(3);
            CHECK_PTR(child);
            child->endpoint++;
            if (data)
                child->data = data;

            r3_node_connectl(n, path, path_len, 1, child);
            if (route) {
                route->data = data;
                r3_node_append_route(child, route);
            }
            return child;
        }
    } else if ( prefix_len == e->pattern_len ) {    // fully-equal to the pattern of the edge

        // there are something more we can insert
        if ( subpath_len > 0 ) {
            return r3_tree_insert_pathl_ex(e->child, subpath, subpath_len, route, data, errstr);
        } else {
            // there are no more path to insert

            // see if there is an endpoint already, we should n't overwrite the data on child.
            // but we still need to append the route.

            if (route) {
                route->data = data;
                r3_node_append_route(e->child, route);
                e->child->endpoint++; // make it as an endpoint
                return e->child;
            }

            // insertion without route
            if (e->child->endpoint > 0) {
                // TODO: return an error code instead of NULL
                return NULL;
            }
            e->child->endpoint++; // make it as an endpoint
            e->child->data = data; // set data
            return e->child;
        }

    } else if ( prefix_len < e->pattern_len ) {
        /* it's partially matched with the pattern,
         * we should split the end point and make a branch here...
         */
        r3_edge_branch(e, prefix_len);
        return r3_tree_insert_pathl_ex(e->child, subpath, subpath_len, route , data, errstr);
    } else {
        fprintf(stderr, "unexpected route.");
        return NULL;
    }
    return n;
}

bool r3_node_has_slug_edges(const R3Node *n) {
    bool found = false;
    R3Edge *e;
    for ( int i = 0 ; i < n->edge_len ; i++ ) {
        e = n->edges[i];
        e->has_slug = r3_path_contains_slug_char(e->pattern);
        if (e->has_slug)
            found = true;
    }
    return found;
}



void r3_tree_dump(const R3Node * n, int level) {
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
        R3Edge * e = n->edges[i];
        print_indent(level + 1);
        printf("|-\"%s\"", e->pattern);

        if (e->opcode ) {
            printf(" opcode:%d", e->opcode);
        }

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
inline int r3_route_cmp(const R3Route *r1, const match_entry *r2) {
    if (r1->request_method != 0) {
        if (0 == (r1->request_method & r2->request_method) ) {
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
void r3_node_append_route(R3Node * n, R3Route * r) {
    if (n->routes == NULL) {
        n->route_cap = 3;
        n->routes = zmalloc(sizeof(R3Route) * n->route_cap);
    }
    if (n->route_len >= n->route_cap) {
        n->route_cap *= 2;
        n->routes = zrealloc(n->routes, sizeof(R3Route) * n->route_cap);
    }
    n->routes[ n->route_len++ ] = r;
}


