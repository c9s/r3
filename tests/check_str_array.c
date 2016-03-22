/*
 * check_str_array.c
 * Copyright (C) 2014 c9s <yoanlin93@gmail.com>
 *
 * Distributed under terms of the MIT license.
 */
#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <check.h>
#include "r3.h"
#include "r3_slug.h"
#include "zmalloc.h"

START_TEST (test_str_array)
{
    match_entry * entry = match_entry_create("/foo");
    ck_assert(entry);

    char *test_str = "abc";
    ck_assert( str_array_append(&entry->vars, test_str, strlen(test_str)));
    ck_assert( entry->vars.tokens.size == 1 );

    char *test_str1 = "foo";
    ck_assert( str_array_append(&entry->vars, test_str1, strlen(test_str)));
    ck_assert( entry->vars.tokens.size == 2 );

    char *test_str2 = "bar";
    ck_assert( str_array_append(&entry->vars, test_str2, strlen(test_str)));
    ck_assert( entry->vars.tokens.size == 3 );

    char *test_str3 = "zoo";
    ck_assert( str_array_append(&entry->vars, test_str3, strlen(test_str)));
    ck_assert( entry->vars.tokens.size == 4 );

    match_entry_free(entry);
}
END_TEST

Suite* r3_suite (void) {
    Suite *suite = suite_create("str_array test");
    TCase *tcase = tcase_create("testcase");
    tcase_add_test(tcase, test_str_array);
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
