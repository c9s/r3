/*
 * check_json.c
 * Copyright (C) 2014 c9s <yoanlin93@gmail.com>
 *
 * Distributed under terms of the MIT license.
 */
#include "config.h"
#include <stdio.h>
#include <check.h>
#include <stdlib.h>
#include <json-c/json.h>
#include "r3.h"
#include "r3_str.h"
#include "r3_json.h"
#include "zmalloc.h"

START_TEST (test_json_encode)
{
    node * n;
    n = r3_tree_create(10);

    ck_assert(n);

    r3_tree_insert_path(n, "/zoo", NULL);
    r3_tree_insert_path(n, "/foo", NULL);
    r3_tree_insert_path(n, "/bar", NULL);

    r3_tree_insert_route(n, METHOD_GET, "/post/get", NULL);
    r3_tree_insert_route(n, METHOD_POST, "/post/post", NULL);

    r3_tree_compile(n, NULL);

    json_object * obj = r3_node_to_json_object(n);
    ck_assert(obj);

    const char *json = r3_node_to_json_pretty_string(n);

    printf("JSON: %s\n",json);
}
END_TEST

Suite* r3_suite (void) {
    Suite *suite = suite_create("json test");
    TCase *tcase = tcase_create("json test");
    tcase_add_test(tcase, test_json_encode);
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

