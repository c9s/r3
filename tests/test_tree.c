#include <stdio.h>
#include <check.h>
#include "str.h"
#include "node.h"
#include "token.h"

START_TEST (test_ltrim_slash)
{
    fail_if( strcmp( ltrim_slash("/blog") , "blog" ) != 0 );
    fail_if( strcmp( ltrim_slash("blog") , "blog" ) != 0 );
}
END_TEST

START_TEST (test_rnode_construct_uniq)
{
    rnode * n = rnode_create(10);

    rnode * child = rnode_create(3);

    fail_if( rnode_add_child(n, strdup("/add") , child) == FALSE );
    fail_if( rnode_add_child(n, strdup("/add") , child) != FALSE );

    rnode_free(n);
}
END_TEST

START_TEST (test_rnode_find_edge)
{
    rnode * n = rnode_create(10);

    rnode * child = rnode_create(3);

    fail_if( rnode_add_child(n, strdup("/add") , child) == FALSE );

    fail_if( rnode_find_edge(n, "/add") == NULL );
    fail_if( rnode_find_edge(n, "/bar") != NULL );

    rnode_free(n);
}
END_TEST


START_TEST (test_rnode_insert_tokens)
{
    token_array *t;

    rnode * n = rnode_create(10);

    fail_if(n == NULL, "rnode tree");

    t = split_route_pattern("/foo/bar", strlen("/foo/bar") );
    fail_if( rnode_insert_tokens(n , t) == NULL );

    t = split_route_pattern("/foo/zoo", strlen("/foo/zoo") );
    fail_if( rnode_insert_tokens(n , t) == NULL );

    t = split_route_pattern("/a/bb", strlen("/a/bb") );
    fail_if( rnode_insert_tokens(n , t) == NULL );

    t = split_route_pattern("/a/bb/cc", strlen("/a/bb/cc") );
    fail_if( rnode_insert_tokens(n , t) == NULL );

    t = split_route_pattern("/a/jj/kk", strlen("/a/jj/kk") );
    fail_if( rnode_insert_tokens(n , t) == NULL );

    rnode_dump(n, 0);


    // fail_if( rnode_find_edge(n, "/add") == NULL );
    // fail_if( rnode_find_edge(n, "/bar") != NULL );

    rnode_free(n);
}
END_TEST




START_TEST (test_route_split)
{
    token_array *t;

    t = split_route_pattern("/blog", strlen("/blog") );
    fail_if( t == NULL );
    token_array_dump(t);
    token_array_free(t);

    t = split_route_pattern("/foo/{id}", strlen("/foo/{id}") );
    fail_if( t == NULL );
    token_array_dump(t);
    fail_if( t->len != 2 );
    token_array_free(t);

    t = split_route_pattern("/foo/bar/{id}", strlen("/foo/bar/{id}") );
    fail_if( t == NULL );
    token_array_dump(t);
    fail_if( t->len != 3 );
    token_array_free(t);

    t = split_route_pattern("/{title}", strlen("/{title}") );
    fail_if( t == NULL );
    token_array_dump(t);
    fail_if( t->len != 1 );
    token_array_free(t);

    t = split_route_pattern("/", strlen("/") );
    fail_if( t == NULL );
    token_array_dump(t);
    fail_if( t->len != 1 );
    token_array_free(t);

}
END_TEST

START_TEST (test_token_array)
{
    token_array * l = token_array_create(3);
    fail_if( l == NULL );

    fail_if( FALSE == token_array_append(l, strdup("abc") ) );
    fail_if( l->len != 1 );

    fail_if( FALSE == token_array_append(l, strdup("foo") ) );
    fail_if( l->len != 2 );

    fail_if( FALSE == token_array_append(l, strdup("bar") ) );
    fail_if( l->len != 3 );

    fail_if( FALSE == token_array_append(l, strdup("zoo") ) );
    fail_if( l->len != 4 );

    fail_if( FALSE == token_array_resize(l, l->cap * 2) );

    token_array_free(l);
}
END_TEST

Suite* r3_suite (void) {
        Suite *suite = suite_create("blah");

        TCase *tcase = tcase_create("testcase");
        tcase_add_test(tcase, test_route_split);
        tcase_add_test(tcase, test_token_array);
        tcase_add_test(tcase, test_ltrim_slash);
        tcase_add_test(tcase, test_rnode_construct_uniq);
        tcase_add_test(tcase, test_rnode_find_edge);
        tcase_add_test(tcase, test_rnode_insert_tokens);

        suite_add_tcase(suite, tcase);

        return suite;
}

int main (int argc, char *argv[]) {
        int number_failed;
        Suite *suite = r3_suite();
        SRunner *runner = srunner_create(suite);
        srunner_run_all(runner, CK_NORMAL);
        number_failed = srunner_ntests_failed(runner);
        srunner_free(runner);
        return number_failed;
}
