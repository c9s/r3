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
#include "str_array.h"

START_TEST (test_str_array)
{
    str_array *vars = r3_mem_alloc(sizeof(str_array));
    memset(vars, 0, sizeof(*vars));

    char *test_str = "abc";
    ck_assert( str_array_append(vars, test_str, strlen(test_str)));
    ck_assert( vars->tokens.size == 1 );

    char *test_str1 = "foo";
    ck_assert( str_array_append(vars, test_str1, strlen(test_str1)));
    ck_assert( vars->tokens.size == 2 );

    char *test_str2 = "bar";
    ck_assert( str_array_append(vars, test_str2, strlen(test_str2)));
    ck_assert( vars->tokens.size == 3 );

    char *test_str3 = "zoo";
    ck_assert( str_array_append(vars, test_str3, strlen(test_str3)));
    ck_assert( vars->tokens.size == 4 );

    str_array_free(vars);
    free(vars);
}
END_TEST

START_TEST (test_access_macros)
{
    str_array *vars = r3_mem_alloc(sizeof(str_array));
    memset(vars, 0, sizeof(*vars));
    ck_assert( str_array_len(vars) == 0);
    ck_assert( str_array_cap(vars) == 0);

    r3_vector_reserve(&vars->tokens, 4);
    ck_assert( str_array_len(vars) == 0);
    ck_assert( str_array_cap(vars) == 4);

    char *token1 = "first";
    char *token2 = "second";
    ck_assert( str_array_append(vars, token1, strlen(token1)));
    ck_assert( str_array_append(vars, token2, strlen(token2)));
    ck_assert( str_array_len(vars) == 2);
    ck_assert( str_array_cap(vars) == 4);

    ck_assert( strncmp(str_array_fetch(vars,0).base, "first", 5) == 0);
    ck_assert( str_array_fetch(vars,0).len == 5);
    ck_assert( strncmp(str_array_fetch(vars,1).base, "second", 6) == 0);
    ck_assert( str_array_fetch(vars,1).len == 6);

    str_array_free(vars);
    free(vars);
}
END_TEST

Suite* r3_suite (void) {
    Suite *suite = suite_create("str_array test");
    TCase *tcase = tcase_create("testcase");
    tcase_add_test(tcase, test_str_array);
    tcase_add_test(tcase, test_access_macros);
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
