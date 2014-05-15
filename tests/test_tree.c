#include <stdio.h>
#include <check.h>
#include "str.h"
#include "node.h"
#include "token.h"

START_TEST (test_route)
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
    token_array * l = token_array_create(10);
    fail_if( l == NULL );
    fail_if( FALSE == token_array_resize(l, l->cap * 2) );

    fail_if( FALSE == token_array_append(l, strdup("abc") ) );
    fail_if( FALSE == token_array_append(l, strdup("foo") ) );
    fail_if( FALSE == token_array_append(l, strdup("bar") ) );

    token_array_free(l);
}
END_TEST

Suite* r3_suite (void) {
        Suite *suite = suite_create("blah");

        TCase *tcase = tcase_create("testcase");
        tcase_add_test(tcase, test_route);
        tcase_add_test(tcase, test_token_array);

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
