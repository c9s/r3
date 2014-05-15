#include <stdio.h>
#include <check.h>
#include "str.h"
#include "node.h"

START_TEST (test_route)
{

}
END_TEST

START_TEST (test_route2)
{

}
END_TEST

Suite* r3_suite (void) {
        Suite *suite = suite_create("blah");

        TCase *tcase = tcase_create("testcase");
        tcase_add_test(tcase, test_route);
        tcase_add_test(tcase, test_route2);

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
