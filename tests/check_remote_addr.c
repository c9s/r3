#include "config.h"
#include <stdio.h>
#include <check.h>
#include <stdlib.h>
#include <assert.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "r3.h"
#include "r3_slug.h"

START_TEST (test_remote_addrs)
{
    R3Node * n      = r3_tree_create(10);
    R3Route * route  = NULL;
    match_entry * entry;
    R3Route *matched_route;

    char * uri0 = "/foo";
    route = r3_tree_insert_routel(n, 0, uri0, strlen(uri0), &uri0);
    route->remote_addr_v4 = 0;
    route->remote_addr_v4_bits = 0;

    char * uri1 = "/bar";
    route = r3_tree_insert_routel(n, 0, uri1, strlen(uri1), &uri1);
    route->remote_addr_v4 = inet_network("127.0.0.1");
    route->remote_addr_v4_bits = 32;

    char * uri2 = "/boo";
    route = r3_tree_insert_routel(n, 0, uri2, strlen(uri2), &uri2);
    route->remote_addr_v4 = inet_network("127.0.0.1");
    route->remote_addr_v4_bits = 24;

    char * err = NULL;
    r3_tree_compile(n, &err);
    ck_assert(err == NULL);


    entry = match_entry_create("/foo");
    entry->remote_addr.base = "127.0.0.1";
    entry->remote_addr.len = sizeof("127.0.0.1") - 1;
    matched_route = r3_tree_match_route(n, entry);
    ck_assert(matched_route != NULL);
    ck_assert(matched_route->data == &uri0);

    entry = match_entry_create("/bar");
    entry->remote_addr.base = "127.0.0.1";
    entry->remote_addr.len = sizeof("127.0.0.1") - 1;
    matched_route = r3_tree_match_route(n, entry);
    ck_assert(matched_route != NULL);
    ck_assert(matched_route->data == &uri1);

    entry = match_entry_create("/bar");
    entry->remote_addr.base = "127.0.0.2";
    entry->remote_addr.len = sizeof("127.0.0.2") - 1;
    matched_route = r3_tree_match_route(n, entry);
    ck_assert(matched_route == NULL);


    entry = match_entry_create("/boo");
    entry->remote_addr.base = "127.0.0.1";
    entry->remote_addr.len = sizeof("127.0.0.1") - 1;
    matched_route = r3_tree_match_route(n, entry);
    ck_assert(matched_route != NULL);
    ck_assert(matched_route->data == &uri2);

    entry = match_entry_create("/boo");
    entry->remote_addr.base = "127.0.0.2";
    entry->remote_addr.len = sizeof("127.0.0.2") - 1;
    matched_route = r3_tree_match_route(n, entry);
    ck_assert(matched_route != NULL);
    ck_assert(matched_route->data == &uri2);

    entry = match_entry_create("/boo");
    entry->remote_addr.base = "127.0.1.2";
    entry->remote_addr.len = sizeof("127.0.1.2") - 1;
    matched_route = r3_tree_match_route(n, entry);
    ck_assert(matched_route == NULL);

    r3_tree_free(n);
}
END_TEST



Suite* r3_suite (void) {
    Suite *suite = suite_create("r3 remote_addr tests");
    TCase *tcase = tcase_create("testcase");
    tcase_add_test(tcase, test_remote_addrs);
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
