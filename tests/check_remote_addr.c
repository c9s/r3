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
    match_entry_free(entry);

    entry = match_entry_create("/bar");
    entry->remote_addr.base = "127.0.0.1";
    entry->remote_addr.len = sizeof("127.0.0.1") - 1;
    matched_route = r3_tree_match_route(n, entry);
    ck_assert(matched_route != NULL);
    ck_assert(matched_route->data == &uri1);
    match_entry_free(entry);

    entry = match_entry_create("/bar");
    entry->remote_addr.base = "127.0.0.2";
    entry->remote_addr.len = sizeof("127.0.0.2") - 1;
    matched_route = r3_tree_match_route(n, entry);
    ck_assert(matched_route == NULL);
    match_entry_free(entry);

    entry = match_entry_create("/boo");
    entry->remote_addr.base = "127.0.0.1";
    entry->remote_addr.len = sizeof("127.0.0.1") - 1;
    matched_route = r3_tree_match_route(n, entry);
    ck_assert(matched_route != NULL);
    ck_assert(matched_route->data == &uri2);
    match_entry_free(entry);

    entry = match_entry_create("/boo");
    entry->remote_addr.base = "127.0.0.2";
    entry->remote_addr.len = sizeof("127.0.0.2") - 1;
    matched_route = r3_tree_match_route(n, entry);
    ck_assert(matched_route != NULL);
    ck_assert(matched_route->data == &uri2);
    match_entry_free(entry);

    entry = match_entry_create("/boo");
    entry->remote_addr.base = "127.0.1.2";
    entry->remote_addr.len = sizeof("127.0.1.2") - 1;
    matched_route = r3_tree_match_route(n, entry);
    ck_assert(matched_route == NULL);
    match_entry_free(entry);

    entry = match_entry_create("/boo");
    entry->remote_addr.base = "127.0.1.333";   // invalid ip address
    entry->remote_addr.len = sizeof("127.0.1.333") - 1;
    matched_route = r3_tree_match_route(n, entry);
    ck_assert(matched_route == NULL);
    match_entry_free(entry);

    r3_tree_free(n);
}
END_TEST


void parse_ipv6(const char *ipv6, int nmask, R3Route * route)
{
    struct in6_addr    addr6;
    int ret = inet_pton(AF_INET6, ipv6, (void *)&addr6);
    ck_assert(ret == 1);

    for (int i = 0; i < 4; i++) {
        route->remote_addr_v6[i] = ntohl(addr6.__in6_u.__u6_addr32[i]);

        if (nmask >= 32) {
            route->remote_addr_v6_bits[i] = 32;
        } else if (nmask > 0) {
            route->remote_addr_v6_bits[i] = nmask;
        } else {
            route->remote_addr_v6_bits[i] = 0;
        }

        nmask -= 32;
    }
}


START_TEST (test_remote_addrs_ipv6)
{
    R3Node * n      = r3_tree_create(10);
    R3Route * route  = NULL;
    match_entry * entry;
    R3Route *matched_route;

    char * uri0 = "/foo";
    route = r3_tree_insert_routel(n, 0, uri0, strlen(uri0), &uri0);
    parse_ipv6("fe80:fe80::1", 128, route);  // "fe80:fe80::1"

    char * uri1 = "/bar";
    route = r3_tree_insert_routel(n, 0, uri1, strlen(uri1), &uri1);
    parse_ipv6("fe80:fe80::", 32, route);  // "fe80:fe80::/32"

    char * uri2 = "/goo";
    route = r3_tree_insert_routel(n, 0, uri2, strlen(uri2), &uri2);
    parse_ipv6("::1", 128, route);  // "::1"

    char * err = NULL;
    r3_tree_compile(n, &err);
    ck_assert(err == NULL);

    entry = match_entry_create("/foo");
    entry->remote_addr.base = "fe80:fe80::1";
    entry->remote_addr.len = sizeof("fe80:fe80::1") - 1;
    matched_route = r3_tree_match_route(n, entry);
    ck_assert(matched_route != NULL);
    ck_assert(matched_route->data == &uri0);
    match_entry_free(entry);

    entry = match_entry_create("/foo");
    entry->remote_addr.base = "fe80:fe80::2";
    entry->remote_addr.len = sizeof("fe80:fe80::2") - 1;
    matched_route = r3_tree_match_route(n, entry);
    ck_assert(matched_route == NULL);
    match_entry_free(entry);

    entry = match_entry_create("/foo");
    entry->remote_addr.base = "fe88:fe80::1";
    entry->remote_addr.len = sizeof("fe88:fe80::1") - 1;
    matched_route = r3_tree_match_route(n, entry);
    ck_assert(matched_route == NULL);
    match_entry_free(entry);

    entry = match_entry_create("/bar");
    entry->remote_addr.base = "fe80:fe80::1";
    entry->remote_addr.len = sizeof("fe80:fe80::1") - 1;
    matched_route = r3_tree_match_route(n, entry);
    ck_assert(matched_route != NULL);
    ck_assert(matched_route->data == &uri1);
    match_entry_free(entry);

    entry = match_entry_create("/bar");
    entry->remote_addr.base = "fe88:fe80::1";
    entry->remote_addr.len = sizeof("fe88:fe80::1") - 1;
    matched_route = r3_tree_match_route(n, entry);
    ck_assert(matched_route == NULL);
    match_entry_free(entry);

    entry = match_entry_create("/goo");
    entry->remote_addr.base = "::1";
    entry->remote_addr.len = sizeof("::1") - 1;
    matched_route = r3_tree_match_route(n, entry);
    ck_assert(matched_route != NULL);
    ck_assert(matched_route->data == &uri2);
    match_entry_free(entry);

    entry = match_entry_create("/goo");
    entry->remote_addr.base = "::2";
    entry->remote_addr.len = sizeof("::2") - 1;
    matched_route = r3_tree_match_route(n, entry);
    ck_assert(matched_route == NULL);
    match_entry_free(entry);

    r3_tree_free(n);
}
END_TEST


Suite* r3_suite (void) {
    Suite *suite = suite_create("r3 remote_addr tests");
    TCase *tcase = tcase_create("testcase");
    tcase_add_test(tcase, test_remote_addrs);
    tcase_add_test(tcase, test_remote_addrs_ipv6);
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
