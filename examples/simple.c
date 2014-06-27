/*
 * bench.c
 * Copyright (C) 2014 c9s <yoanlin93@gmail.com>
 *
 * Distributed under terms of the MIT license.
 */
#include <stdio.h>
#include <stdlib.h>
#include "r3.h"

int main()
{
    node * n = r3_tree_create(3);

    r3_tree_insert_path(n, "/foo/bar/baz",  NULL);
    r3_tree_insert_path(n, "/foo/bar/qux",  NULL);
    r3_tree_insert_path(n, "/foo/bar/quux",  NULL);
    r3_tree_insert_path(n, "/bar/foo/baz",  NULL);
    r3_tree_insert_path(n, "/bar/foo/quux",  NULL);
    r3_tree_insert_path(n, "/bar/garply/grault",  NULL);
    r3_tree_insert_path(n, "/baz/foo/bar",  NULL);
    r3_tree_insert_path(n, "/baz/foo/qux",  NULL);
    r3_tree_insert_path(n, "/baz/foo/quux",  NULL);
    r3_tree_insert_path(n, "/qux/foo/quux",  NULL);
    r3_tree_insert_path(n, "/qux/foo/corge",  NULL);
    r3_tree_insert_path(n, "/qux/foo/grault",  NULL);
    r3_tree_insert_path(n, "/corge/quux/foo",  NULL);
    r3_tree_insert_path(n, "/corge/quux/bar",  NULL);
    r3_tree_insert_path(n, "/corge/quux/baz",  NULL);
    r3_tree_insert_path(n, "/corge/quux/qux",  NULL);
    r3_tree_insert_path(n, "/corge/quux/grault",  NULL);
    r3_tree_insert_path(n, "/grault/foo/bar",  NULL);
    r3_tree_insert_path(n, "/grault/foo/baz",  NULL);
    r3_tree_insert_path(n, "/garply/baz/quux",  NULL);
    r3_tree_insert_path(n, "/garply/baz/corge",  NULL);
    r3_tree_insert_path(n, "/garply/baz/grault",  NULL);
    r3_tree_insert_path(n, "/garply/qux/foo",  NULL);

    char *errstr = NULL;
    int err = r3_tree_compile(n, &errstr);
    if(err) {
        printf("%s\n",errstr);
        free(errstr);
        return 1;
    }

    node *m;

    m = r3_tree_match(n , "/qux/bar/corge", NULL);

    match_entry * e = match_entry_createl("/garply/baz/grault", strlen("/garply/baz/grault") );
    m = r3_tree_match_entry(n , e);
    if (m) {
        printf("Matched! %s\n", e->path);
    }
    match_entry_free(e);
    return 0;
}
