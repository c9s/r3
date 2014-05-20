/*
 * check_slug.c
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
#include "str_array.h"

START_TEST (test_compile_slug)
{
    char * path = "/user/{id}";
    ck_assert_str_eq( compile_slug(path, strlen(path) ) , "/user/([^/]+)" );

    char * path2 = "/what/{id}-foo";
    ck_assert_str_eq( compile_slug(path2, strlen(path2) ) , "/what/([^/]+)-foo" );

    char * path3 = "-{id}";
    ck_assert_str_eq(compile_slug(path3, strlen(path3)), "-([^/]+)" );

    char * path4 = "-{idx:\\d{3}}";
    ck_assert_str_eq(compile_slug(path4, strlen(path4)), "-(\\d{3})" );
}
END_TEST

START_TEST (test_contains_slug)
{
    char * path = "/user/{id}/{name}";
    ck_assert( contains_slug(path) );
}
END_TEST

START_TEST (test_find_slug_pattern)
{
    int len;
    char * namerex = find_slug_pattern("{name:\\s+}", &len);
    ck_assert( strncmp(namerex, "\\s+", len) == 0 );
}
END_TEST


START_TEST (test_find_slug_placeholder)
{
    int slug_len = 0;
    char * slug;
    slug = find_slug_placeholder("/user/{name:\\s+}/to/{id}", &slug_len);
    ck_assert( strncmp(slug, "{name:\\s+}", slug_len) == 0 );


    slug = find_slug_placeholder("/user/{idx:\\d{3}}/to/{idy:\\d{3}}", &slug_len);
    ck_assert( slug_len == strlen("{idx:\\d{3}}") );
    ck_assert( strncmp(slug, "{idx:\\d{3}}", slug_len) == 0 );
}
END_TEST

START_TEST (test_inside_slug)
{
    int slug_len = 0;
    char * pattern = "/user/{name:\\s+}/to/{id}";
    char * offset = strchr(pattern, '{') + 2;
    ck_assert( inside_slug(pattern, strlen(pattern), offset) );
    ck_assert( *(inside_slug(pattern, strlen(pattern), offset)) == '{' );
    ck_assert( ! inside_slug(pattern, strlen(pattern), pattern) );
}
END_TEST

START_TEST (test_count_slug)
{
    int slug_len = 0;
    char * pattern = "/user/{name:\\s+}/to/{id}";
    ck_assert_int_eq( count_slug(pattern, strlen(pattern) ), 2 );

    char * pattern2 = "/user/{name:\\d{3}}/to/{id}";
    ck_assert_int_eq( count_slug(pattern2, strlen(pattern) ), 2 );
}
END_TEST

START_TEST (test_find_slug_placeholder_with_broken_slug)
{
    int slug_len = 0;
    char * slug = find_slug_placeholder("/user/{name:\\s+/to/{id", &slug_len);
    ck_assert(! slug);
}
END_TEST


Suite* r3_suite (void) {
        Suite *suite = suite_create("slug test");
        TCase *tcase = tcase_create("test_slug");
        tcase_add_test(tcase, test_contains_slug);
        tcase_add_test(tcase, test_inside_slug);
        tcase_add_test(tcase, test_find_slug_pattern);
        tcase_add_test(tcase, test_find_slug_placeholder);
        tcase_add_test(tcase, test_find_slug_placeholder_with_broken_slug);
        tcase_add_test(tcase, test_count_slug);
        tcase_add_test(tcase, test_compile_slug);

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
