
/** DO NOT MODIFY THIS FILE, THIS TEST FILE IS AUTO-GENERATED. **/

#include "config.h"
#include <stdio.h>
#include <check.h>
#include <stdlib.h>
#include <assert.h>
#include "r3.h"
#include "r3_slug.h"

START_TEST (test_hosts)
{
    R3Node * n      = r3_tree_create(10);
    R3Route * route  = NULL;
    match_entry * entry;
    R3Route *matched_route;

    char * uri0 = "/foo";
    char * host0 = "foo.com";
    route = r3_tree_insert_routel(n, 0, uri0, strlen(uri0), &uri0);
    route->host.base = host0;
    route->host.len = strlen(host0);

    char * uri1 = "/bar";
    char * host1 = "*.bar.com";
    route = r3_tree_insert_routel(n, 0, uri1, strlen(uri1), &uri1);
    route->host.base = host1;
    route->host.len = strlen(host1);

    char * err = NULL;
    r3_tree_compile(n, &err);
    ck_assert(err == NULL);


    entry = match_entry_create("/foo");
    entry->host.base = host0;
    entry->host.len = strlen(host0);
    matched_route = r3_tree_match_route(n, entry);
    ck_assert(matched_route != NULL);
    ck_assert(matched_route->data == &uri0);


    entry = match_entry_create("/bar");
    entry->host.base = "www.bar.com";
    entry->host.len = strlen("www.bar.com");
    matched_route = r3_tree_match_route(n, entry);
    ck_assert(matched_route != NULL);
    ck_assert(matched_route->data == &uri1);


    entry = match_entry_create("/bar");
    entry->host.base = "bar.com";
    entry->host.len = strlen("bar.com");
    matched_route = r3_tree_match_route(n, entry);
    ck_assert(matched_route == NULL);

    entry = match_entry_create("/bar");
    entry->host.base = ".bar.com";
    entry->host.len = strlen(".bar.com");
    matched_route = r3_tree_match_route(n, entry);
    ck_assert(matched_route == NULL);

    entry = match_entry_create("/bar");
    entry->host.base = "a.bar.com";
    entry->host.len = strlen("a.bar.com");
    matched_route = r3_tree_match_route(n, entry);
    ck_assert(matched_route != NULL);
    ck_assert(matched_route->data == &uri1);

    r3_tree_free(n);
}
END_TEST



Suite* r3_suite (void) {
    Suite *suite = suite_create("r3 host tests");
    TCase *tcase = tcase_create("testcase");
    tcase_add_test(tcase, test_hosts);
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

