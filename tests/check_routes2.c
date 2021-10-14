#include "config.h"
#include <stdio.h>
#include <check.h>
#include <stdlib.h>
#include <assert.h>
#include "r3.h"
#include "r3_slug.h"

START_TEST (greedy_pattern)
{
    R3Node * n      = r3_tree_create(10);
    match_entry * entry;
    R3Route *matched_route;

    char * uri0 = "/foo{:.*}";
    r3_tree_insert_routel(n, 0, uri0, strlen(uri0), &uri0);

    char * err = NULL;
    r3_tree_compile(n, &err);
    ck_assert(err == NULL);

    entry = match_entry_create("/foo/bar");
    matched_route = r3_tree_match_route(n, entry);
    ck_assert(matched_route != NULL);
    ck_assert(matched_route->data == &uri0);
    match_entry_free(entry);

    entry = match_entry_create("/foo");
    matched_route = r3_tree_match_route(n, entry);
    ck_assert(matched_route != NULL);
    ck_assert(matched_route->data == &uri0);
    match_entry_free(entry);

    entry = match_entry_create("/foo/");
    matched_route = r3_tree_match_route(n, entry);
    ck_assert(matched_route != NULL);
    ck_assert(matched_route->data == &uri0);
    match_entry_free(entry);

    entry = match_entry_create("/foo/bar/foo/mmasdfasdfasd/f/asdf/as/df");
    matched_route = r3_tree_match_route(n, entry);
    ck_assert(matched_route != NULL);
    ck_assert(matched_route->data == &uri0);
    match_entry_free(entry);

    r3_tree_free(n);
}
END_TEST

START_TEST (common_pattern)
{
    R3Node * n = r3_tree_create(10);
    match_entry * entry;
    R3Route *r, *matched_route;

    char * uri0 = "/foo/{slug}";
    r = r3_tree_insert_routel(n, 0, uri0, strlen(uri0), &uri0);
    ck_assert(r != NULL);
    char * uri1 = "/foo/{slug}/bar";
    r = r3_tree_insert_routel(n, 0, uri1, strlen(uri1), &uri1);
    ck_assert(r != NULL);

    char * err = NULL;
    r3_tree_compile(n, &err);
    ck_assert(err == NULL);

    entry = match_entry_create("/foo/bar");
    matched_route = r3_tree_match_route(n, entry);
    ck_assert(matched_route != NULL);
    ck_assert(matched_route->data == &uri0);
    match_entry_free(entry);

    entry = match_entry_create("/foo/bar/bar");
    matched_route = r3_tree_match_route(n, entry);
    ck_assert(matched_route != NULL);
    ck_assert(matched_route->data == &uri1);
    match_entry_free(entry);

    r3_tree_free(n);
}
END_TEST

Suite* r3_suite (void) {
    Suite *suite = suite_create("r3 routes2 tests");
    TCase *tcase = tcase_create("testcase");
    tcase_add_test(tcase, greedy_pattern);
    tcase_add_test(tcase, common_pattern);
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
