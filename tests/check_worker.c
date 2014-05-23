/*
 * check_worker.c
 * Copyright (C) 2014 c9s <c9s@c9smba.local>
 *
 * Distributed under terms of the MIT license.
 */
#include "config.h"
#include <stdio.h>
#include <check.h>
#include <stdlib.h>
#include "r3.h"
#include "r3_str.h"
#include "r3_workers.h"
#include "str_array.h"
#include "bench.h"

START_TEST (test_feedback_worker)
{
    node * n = r3_tree_create(1);

    r3_tree_insert_path(n, "/foo/bar/baz",  NULL);
    r3_tree_insert_path(n, "/foo/bar/qux",  NULL);
    r3_tree_insert_path(n, "/foo/bar/quux",  NULL);
    r3_tree_insert_path(n, "/foo/bar/corge",  NULL);
    r3_tree_insert_path(n, "/foo/bar/grault",  NULL);
    r3_tree_insert_path(n, "/garply/grault/foo",  NULL);
    r3_tree_insert_path(n, "/garply/grault/bar",  NULL);
    r3_tree_compile(n);

    r3_tree_free(n);
}
END_TEST


Suite* r3_suite (void) {
        Suite *suite = suite_create("Worker Test");
        TCase *tcase = tcase_create("worker test");
        tcase_add_test(tcase, test_feedback_worker);
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
