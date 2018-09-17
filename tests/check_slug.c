/*
 * check_slug.c
 * Copyright (C) 2014 c9s <yoanlin93@gmail.com>
 *
 * Distributed under terms of the MIT license.
 */
#include "config.h"
#include <stdio.h>
#include <check.h>
#include <stdlib.h>
#include <stdint.h>
#include "r3.h"
#include "r3_slug.h"
#include "slug.h"
#include "r3_debug.h"

START_TEST (test_pattern_to_opcode)
{
    ck_assert( r3_pattern_to_opcode("\\w+", strlen("\\w+")) == OP_EXPECT_MORE_WORDS );
    ck_assert( r3_pattern_to_opcode("\\d+", strlen("\\d+")) == OP_EXPECT_MORE_DIGITS );
    ck_assert( r3_pattern_to_opcode("[^/]+",strlen("[^/]+")) == OP_EXPECT_NOSLASH );
    ck_assert( r3_pattern_to_opcode("[^-]+",strlen("[^-]+")) == OP_EXPECT_NODASH );
}
END_TEST

START_TEST (test_r3_slug_compile)
{
    char * path = "/user/{id}";
    char * c = NULL;
    ck_assert_str_eq( c = r3_slug_compile(path, strlen(path) ) , "^/user/([^/]+)" );
    free(c);

    char * path2 = "/what/{id}-foo";
    ck_assert_str_eq( c = r3_slug_compile(path2, strlen(path2) ) , "^/what/([^/]+)-foo" );
    free(c);

    char * path3 = "-{id}";
    ck_assert_str_eq( c = r3_slug_compile(path3, strlen(path3)), "^-([^/]+)" );
    free(c);

    char * path4 = "-{idx:\\d{3}}";
    ck_assert_str_eq( c = r3_slug_compile(path4, strlen(path4)), "^-(\\d{3})" );
    free(c);
}
END_TEST

START_TEST (test_contains_slug)
{
    char *test_str = "/user/{id}/{name}";
    ck_assert( r3_path_contains_slug_char(test_str, strlen(test_str)) );
}
END_TEST

START_TEST (test_r3_slug_find_pattern)
{
    int len;
    char *test_str = "{name:\\s+}";
    const char * namerex = r3_slug_find_pattern(test_str, strlen(test_str), &len);
    ck_assert( strncmp(namerex, "\\s+", len) == 0 );
}
END_TEST

START_TEST (test_r3_slug_find_name)
{
    int len;
    char *test_str = "{name:\\s+}";
    const char * namerex = r3_slug_find_name(test_str, strlen(test_str), &len);
    ck_assert( strncmp(namerex, "name", len) == 0 );
}
END_TEST

START_TEST (test_r3_slug_find_name_without_pattern)
{
    int len;
    char *test_str = "{name}";
    const char * namerex = r3_slug_find_name(test_str, strlen(test_str), &len);
    ck_assert( strncmp(namerex, "name", len) == 0 );
}
END_TEST

START_TEST (test_r3_slug_find_name_with_multiple_slug)
{
    int len;
    char *test_str = "{name}/{name2}";
    const char * namerex = r3_slug_find_name(test_str, strlen(test_str), &len);
    ck_assert( strncmp(namerex, "name", len) == 0 );
}
END_TEST

START_TEST (test_r3_slug_find_placeholder)
{
    int slug_len = 0;
    const char * slug;
    char *test_str = "/user/{name:\\s+}/to/{id}";
    slug = r3_slug_find_placeholder(test_str, strlen(test_str), &slug_len);
    ck_assert( strncmp(slug, "{name:\\s+}", slug_len) == 0 );

    test_str = "/user/{idx:\\d{3}}/to/{idy:\\d{3}}";
    slug = r3_slug_find_placeholder(test_str, strlen(test_str), &slug_len);
    ck_assert( slug_len == strlen("{idx:\\d{3}}") );
    ck_assert( strncmp(slug, "{idx:\\d{3}}", slug_len) == 0 );
}
END_TEST

START_TEST (test_r3_inside_slug)
{
    char * pattern = "/user/{name:\\s+}/to/{id}";
    char * offset = strchr(pattern, '{') + 2;
    ck_assert( r3_inside_slug(pattern, strlen(pattern), offset, NULL) != NULL );
    ck_assert( *(r3_inside_slug(pattern, strlen(pattern), offset, NULL)) == '{' );
    ck_assert( ! r3_inside_slug(pattern, strlen(pattern), pattern, NULL) );
}
END_TEST

START_TEST (test_incomplete_slug)
{
    int cnt = 0;
    char * errstr = NULL;
    char * pattern = "/user/{name:\\d{3}}/to/{id";
    cnt = r3_slug_count(pattern, strlen(pattern), &errstr);
    ck_assert_int_eq(cnt, -1);
    ck_assert(errstr);
    printf("%s\n",errstr);
    free(errstr);
}
END_TEST


/*
START_TEST (test_slug_parse_with_pattern)
{
    char * pattern = "/user/{name:\\d{3}}";
    char * errstr = NULL;
    r3_slug_t s;
    int ret;
    ret = r3_slug_parse(&s, pattern, strlen(pattern), pattern, &errstr);
    ck_assert(ret);

    char * out = r3_slug_to_str(&s);
    ck_assert(out);
    printf("%s\n",out);
    free(out);
}
END_TEST


START_TEST (test_slug_parse_without_pattern)
{
    char * pattern = "/user/{name}";
    char * errstr = NULL;
    r3_slug_t *s = r3_slug_new(pattern, strlen(pattern));
    int ret;
    ret = r3_slug_parse(s, pattern, strlen(pattern), pattern, &errstr);
    ck_assert(s);

    char * out = r3_slug_to_str(s);
    ck_assert(out);
    printf("%s\n",out);
    free(out);

    r3_slug_free(s);
}
END_TEST
*/






START_TEST (test_r3_slug_count)
{
    int cnt = 0;
    char * pattern = "/user/{name:\\s+}/to/{id}";
    char * errstr = NULL;
    cnt = r3_slug_count(pattern, strlen(pattern), &errstr);
    ck_assert_int_eq(cnt, 2);
    if(errstr) free(errstr);

    char * pattern2 = "/user/{name:\\d{3}}/to/{id}";
    cnt = r3_slug_count(pattern2, strlen(pattern2), &errstr);
    ck_assert_int_eq(cnt, 2);
    if(errstr) free(errstr);

    char * pattern3 = "/user/{name:\\d{3}}/to/{id}";
    cnt = r3_slug_count(pattern3, strlen(pattern3), &errstr);
    ck_assert_int_eq(cnt, 2);
    if(errstr) free(errstr);
}
END_TEST

START_TEST (test_r3_slug_find_placeholder_with_broken_slug)
{
    int slug_len = 0;
    char *sl_test = "/user/{name:\\s+/to/{id";
    const char * slug = r3_slug_find_placeholder(sl_test, strlen(sl_test), &slug_len);
    ck_assert(slug == 0);
}
END_TEST


Suite* r3_suite (void) {
        Suite *suite = suite_create("slug test");
        TCase *tcase = tcase_create("test_slug");
        tcase_set_timeout(tcase, 30);
        tcase_add_test(tcase, test_contains_slug);
        tcase_add_test(tcase, test_r3_inside_slug);
        tcase_add_test(tcase, test_r3_slug_find_pattern);
        tcase_add_test(tcase, test_r3_slug_find_placeholder);
        tcase_add_test(tcase, test_r3_slug_find_placeholder_with_broken_slug);
        tcase_add_test(tcase, test_r3_slug_count);
        tcase_add_test(tcase, test_r3_slug_compile);
        tcase_add_test(tcase, test_pattern_to_opcode);
        tcase_add_test(tcase, test_incomplete_slug);
        tcase_add_test(tcase, test_r3_slug_find_name);
        tcase_add_test(tcase, test_r3_slug_find_name_without_pattern);
        tcase_add_test(tcase, test_r3_slug_find_name_with_multiple_slug);
        
        // tcase_add_test(tcase, test_slug_parse_with_pattern);
        // tcase_add_test(tcase, test_slug_parse_without_pattern);

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
