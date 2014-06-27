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
#include "r3_str.h"
#include "zmalloc.h"

START_TEST (test_str_array)
{
    str_array * l = str_array_create(3);
    ck_assert(l);

    ck_assert(str_array_append(l, zstrdup("abc")));
    ck_assert( l->len == 1 );

    ck_assert(str_array_append(l, zstrdup("foo") ));
    ck_assert( l->len == 2 );

    ck_assert( str_array_append(l, zstrdup("bar") ) );
    ck_assert( l->len == 3 );

    ck_assert( str_array_append(l, zstrdup("zoo") ) );
    ck_assert( l->len == 4 );

    ck_assert( str_array_resize(l, l->cap * 2) );
    str_array_free(l);
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
