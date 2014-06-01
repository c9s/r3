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
#include "zmalloc.h"

START_TEST (test_pattern_to_opcode)
{
    ck_assert( r3_pattern_to_opcode("\\w+", strlen("\\w+")) == OP_EXPECT_MORE_WORDS );
    ck_assert( r3_pattern_to_opcode("\\d+", strlen("\\d+")) == OP_EXPECT_MORE_DIGITS );
    ck_assert( r3_pattern_to_opcode("[^/]+",strlen("[^/]+")) == OP_EXPECT_NOSLASH );
    ck_assert( r3_pattern_to_opcode("[^-]+",strlen("[^-]+")) == OP_EXPECT_NODASH );
}
END_TEST

START_TEST (test_slug_compile)
{
    char * path = "/user/{id}";
    char * c = NULL;
    ck_assert_str_eq( c = slug_compile(path, strlen(path) ) , "/user/([^/]+)" );
    zfree(c);

    char * path2 = "/what/{id}-foo";
    ck_assert_str_eq( c = slug_compile(path2, strlen(path2) ) , "/what/([^/]+)-foo" );
    zfree(c);

    char * path3 = "-{id}";
    ck_assert_str_eq( c = slug_compile(path3, strlen(path3)), "-([^/]+)" );
    zfree(c);

    char * path4 = "-{idx:\\d{3}}";
    ck_assert_str_eq( c = slug_compile(path4, strlen(path4)), "-(\\d{3})" );
    zfree(c);
}
END_TEST

START_TEST (test_contains_slug)
{
    char * path = "/user/{id}/{name}";
    ck_assert( contains_slug(path) );
}
END_TEST

START_TEST (test_slug_find_pattern)
{
    int len;
    char * namerex = slug_find_pattern("{name:\\s+}", &len);
    ck_assert( strncmp(namerex, "\\s+", len) == 0 );
}
END_TEST


START_TEST (test_slug_find_placeholder)
{
    int slug_len = 0;
    char * slug;
    slug = slug_find_placeholder("/user/{name:\\s+}/to/{id}", &slug_len);
    ck_assert( strncmp(slug, "{name:\\s+}", slug_len) == 0 );


    slug = slug_find_placeholder("/user/{idx:\\d{3}}/to/{idy:\\d{3}}", &slug_len);
    ck_assert( slug_len == strlen("{idx:\\d{3}}") );
    ck_assert( strncmp(slug, "{idx:\\d{3}}", slug_len) == 0 );
}
END_TEST

START_TEST (test_inside_slug)
{
    char * pattern = "/user/{name:\\s+}/to/{id}";
    char * offset = strchr(pattern, '{') + 2;
    ck_assert( (int)inside_slug(pattern, strlen(pattern), offset, NULL) );
    ck_assert( *(inside_slug(pattern, strlen(pattern), offset, NULL)) == '{' );
    ck_assert( ! inside_slug(pattern, strlen(pattern), pattern, NULL) );
}
END_TEST

START_TEST (test_slug_count)
{
    int cnt = 0;
    char * pattern = "/user/{name:\\s+}/to/{id}";
    char * errstr = NULL;
    cnt = slug_count(pattern, strlen(pattern), &errstr);
    ck_assert_int_eq(cnt, 2);

    char * pattern2 = "/user/{name:\\d{3}}/to/{id}";
    cnt = slug_count(pattern2, strlen(pattern2), &errstr);
    ck_assert_int_eq(cnt, 2);
}
END_TEST

START_TEST (test_slug_find_placeholder_with_broken_slug)
{
    int slug_len = 0;
    char * slug = slug_find_placeholder("/user/{name:\\s+/to/{id", &slug_len);
    ck_assert(slug == 0);
}
END_TEST


Suite* r3_suite (void) {
        Suite *suite = suite_create("slug test");
        TCase *tcase = tcase_create("test_slug");
        tcase_set_timeout(tcase, 30);
        tcase_add_test(tcase, test_contains_slug);
        tcase_add_test(tcase, test_inside_slug);
        tcase_add_test(tcase, test_slug_find_pattern);
        tcase_add_test(tcase, test_slug_find_placeholder);
        tcase_add_test(tcase, test_slug_find_placeholder_with_broken_slug);
        tcase_add_test(tcase, test_slug_count);
        tcase_add_test(tcase, test_slug_compile);
        tcase_add_test(tcase, test_pattern_to_opcode);

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
