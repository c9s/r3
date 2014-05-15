#include <stdio.h>
#include <check.h>
#include "str.h"
#include "node.h"
#include "token.h"

START_TEST (test_route)
{
    split_route_pattern("/blog", strlen("/blog") );
    split_route_pattern("/foo/{id}", strlen("/foo/{id}") );
    split_route_pattern("/{title}", strlen("/{title}") );

}
END_TEST

START_TEST (test_token_list)
{
    token_list * l = token_list_create(10);
    fail_if( l == NULL );
    fail_if( FALSE == token_list_resize(l, l->cap * 2) );

    fail_if( FALSE == token_list_append(l, strdup("abc") ) );
    fail_if( FALSE == token_list_append(l, strdup("foo") ) );
    fail_if( FALSE == token_list_append(l, strdup("bar") ) );

    token_list_free(l);
}
END_TEST

Suite* r3_suite (void) {
        Suite *suite = suite_create("blah");

        TCase *tcase = tcase_create("testcase");
        tcase_add_test(tcase, test_route);
        tcase_add_test(tcase, test_token_list);

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
