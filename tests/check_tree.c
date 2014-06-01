#include "config.h"
#include <stdio.h>
#include <check.h>
#include <stdlib.h>
#include <assert.h>
#include "r3.h"
#include "r3_str.h"
#include "zmalloc.h"
#include "bench.h"



START_TEST (test_ltrim_slash)
{
    fail_if( strcmp( ltrim_slash("/blog") , "blog" ) != 0 );
    fail_if( strcmp( ltrim_slash("blog") , "blog" ) != 0 );
}
END_TEST

START_TEST (test_r3_node_construct_and_free)
{
    node * n = r3_tree_create(10);
    node * another_tree = r3_tree_create(3);
    r3_tree_free(n);
    r3_tree_free(another_tree);
}
END_TEST

START_TEST (test_r3_node_find_edge)
{
    node * n = r3_tree_create(10);

    node * child = r3_tree_create(3);

    fail_if( r3_node_connect(n, zstrdup("/add") , child) == FALSE );
    fail_if( r3_node_find_edge(n, "/add") == NULL );
    fail_if( r3_node_find_edge(n, "/bar") != NULL );

    r3_tree_free(n);
}
END_TEST


static node * create_simple_str_tree() {
    node * n;
    n = r3_tree_create(10);
    r3_tree_insert_path(n, "/zoo", NULL);
    r3_tree_insert_path(n, "/foo", NULL);
    r3_tree_insert_path(n, "/bar", NULL);
    r3_tree_compile(n, NULL);
    return n;
}



START_TEST (test_compile)
{
    node *n;
    node *m;
    edge *e;

    n = create_simple_str_tree();

#ifdef DEBUG
    r3_tree_dump(n, 0);
#endif

    r3_tree_insert_path(n, "/foo/{id}", NULL);
    r3_tree_insert_path(n, "/{id}", NULL);
    r3_tree_compile(n, NULL);
    r3_tree_compile(n, NULL); // test double compile
    r3_tree_dump(n, 0);
    match_entry * entry;

    entry = match_entry_createl( "foo" , strlen("/foo") );
    m = r3_tree_matchl( n , "/foo", strlen("/foo"), entry);
    ck_assert( m );

    entry = match_entry_createl( "/zoo" , strlen("/zoo") );
    m = r3_tree_matchl( n , "/zoo", strlen("/zoo"), entry);
    ck_assert( m );

    entry = match_entry_createl( "/bar" , strlen("/bar") );
    m = r3_tree_matchl( n , "/bar", strlen("/bar"), entry);
    ck_assert( m );

    entry = match_entry_createl( "/xxx" , strlen("/xxx") );
    m = r3_tree_matchl( n , "/xxx", strlen("/xxx"), entry);
    ck_assert( m );

    entry = match_entry_createl( "/foo/xxx" , strlen("/foo/xxx") );
    m = r3_tree_matchl( n , "/foo/xxx", strlen("/foo/xxx"), entry);
    ck_assert( m );

    entry = match_entry_createl( "/some_id" , strlen("/some_id") );
    m = r3_tree_matchl( n , "/some_id", strlen("/some_id"), entry);
    ck_assert( m );
}
END_TEST


START_TEST (test_incomplete_slug_path)
{
    node * n = r3_tree_create(10);

    node * ret_node;

    // r3_tree_insert_path(n, "/foo-{user}-{id}", NULL, NULL);
    ret_node = r3_tree_insert_path(n, "/post/{handle", NULL);
    assert(!ret_node);

    ret_node = r3_tree_insert_path(n, "/post/{handle:\\", NULL);
    assert(!ret_node);

    ret_node = r3_tree_insert_path(n, "/post/{handle:\\d", NULL);
    assert(!ret_node);

    ret_node = r3_tree_insert_path(n, "/post/{handle:\\d{", NULL);
    assert(!ret_node);

    ret_node = r3_tree_insert_path(n, "/post/{handle:\\d{3", NULL);
    assert(!ret_node);

    r3_tree_insert_path(n, "/post/{handle:\\d{3}", NULL);
    r3_tree_insert_path(n, "/post/{handle:\\d{3}}/{", NULL);
    r3_tree_insert_path(n, "/post/{handle:\\d{3}}/{a", NULL);
    r3_tree_insert_path(n, "/post/{handle:\\d{3}}/{a}", NULL);

    r3_tree_free(n);
}
END_TEST


START_TEST (test_pcre_patterns_insert)
{
    node * n = r3_tree_create(10);

    // r3_tree_insert_path(n, "/foo-{user}-{id}", NULL, NULL);
    r3_tree_insert_path(n, "/post/{handle:\\d+}-{id:\\d+}", NULL);

    r3_tree_insert_path(n, "/post/foo", NULL);
    r3_tree_insert_path(n, "/post/bar", NULL);

    char *errstr = NULL;
    int errcode;
    errcode = r3_tree_compile(n, &errstr);
    ck_assert(errcode == 0); // no error

    // r3_tree_dump(n, 0);

    node *matched;


    matched = r3_tree_match(n, "/post/foo", NULL);
    ck_assert(matched);
    ck_assert(matched->endpoint > 0);

    matched = r3_tree_match(n, "/post/bar", NULL);
    ck_assert(matched);
    ck_assert(matched->endpoint > 0);

    matched = r3_tree_match(n, "/post/kkkfoo", NULL);
    ck_assert(!matched);

    matched = r3_tree_match(n, "/post/kkkbar", NULL);
    ck_assert(!matched);



    matched = r3_tree_matchl(n, "/post/111-222", strlen("/post/111-222"), NULL);
    ck_assert(matched);
    ck_assert(matched->endpoint > 0);

    // incomplete string shouldn't match
    matched = r3_tree_matchl(n, "/post/111-", strlen("/post/111-"), NULL);
    ck_assert(! matched);

    r3_tree_free(n);
}
END_TEST


/**
 * Test for \d{2}/\d{2}
 */
START_TEST (test_pcre_patterns_insert_2)
{
    node * n = r3_tree_create(10);
    r3_tree_insert_path(n, "/post/{idx:\\d{2}}/{idy:\\d{2}}", NULL);
    r3_tree_insert_path(n, "/zoo", NULL);
    r3_tree_insert_path(n, "/foo", NULL);
    r3_tree_insert_path(n, "/bar", NULL);

    char *errstr = NULL;
    r3_tree_compile(n, &errstr);

    r3_tree_dump(n, 0);
    node *matched;
    matched = r3_tree_match(n, "/post/11/22", NULL);
    ck_assert((int)matched);
    ck_assert(matched->endpoint > 0);
}
END_TEST

/**
 * Test for (\d{2})/([^/]+)
 */
START_TEST (test_pcre_patterns_insert_3)
{
    node * n = r3_tree_create(10);
    printf("Inserting /post/{idx:\\d{2}}/{idy}\n");
    r3_tree_insert_path(n, "/post/{idx:\\d{2}}/{idy}", NULL);
    r3_tree_dump(n, 0);

    printf("Inserting /zoo\n");
    r3_tree_insert_path(n, "/zoo", NULL);
    r3_tree_dump(n, 0);

    r3_tree_insert_path(n, "/foo", NULL);
    r3_tree_insert_path(n, "/bar", NULL);

    char *errstr = NULL;
    r3_tree_compile(n, &errstr);

    r3_tree_dump(n, 0);
    node *matched;


    matched = r3_tree_match(n, "/post/11/22", NULL);
    ck_assert((int)matched);

    matched = r3_tree_match(n, "/post/11", NULL);
    ck_assert(!matched);

    matched = r3_tree_match(n, "/post/11/", NULL);
    ck_assert(!matched);

    /*
    matched = r3_tree_match(n, "/post/113", NULL);
    ck_assert(!matched);
    */
}
END_TEST







START_TEST (testr3_tree_insert_pathl)
{
    node * n = r3_tree_create(10);

    r3_tree_insert_path(n, "/foo/bar",  NULL);
    // r3_tree_dump(n, 0);

    r3_tree_insert_path(n, "/foo/zoo",  NULL);
    // r3_tree_dump(n, 0);

    r3_tree_insert_path(n, "/f/id" ,  NULL);
    // r3_tree_dump(n, 0);

    r3_tree_insert_path(n, "/post/{id}", NULL);
    // r3_tree_dump(n, 0);

    r3_tree_insert_path(n, "/post/{handle}", NULL);

    r3_tree_insert_path(n, "/post/{handle}-{id}", NULL);

    char * errstr = NULL;
    r3_tree_compile(n, &errstr);

#ifdef DEBUG
    r3_tree_dump(n, 0);
#endif
    r3_tree_free(n);
}
END_TEST





START_TEST (test_str_array)
{
    str_array * l = str_array_create(3);
    fail_if( l == NULL );

    fail_if( FALSE == str_array_append(l, zstrdup("abc") ) );
    fail_if( l->len != 1 );

    fail_if( FALSE == str_array_append(l, zstrdup("foo") ) );
    fail_if( l->len != 2 );

    fail_if( FALSE == str_array_append(l, zstrdup("bar") ) );
    fail_if( l->len != 3 );

    fail_if( FALSE == str_array_append(l, zstrdup("zoo") ) );
    fail_if( l->len != 4 );

    fail_if( FALSE == str_array_resize(l, l->cap * 2) );

    str_array_free(l);
}
END_TEST

START_TEST(test_route_cmp)
{
    route *r1 = r3_route_create("/blog/post");
    match_entry * m = match_entry_create("/blog/post");

    fail_if( r3_route_cmp(r1, m) == -1, "should match");

    r1->request_method = METHOD_GET;
    m->request_method = METHOD_GET;
    fail_if( r3_route_cmp(r1, m) == -1, "should match");

    r1->request_method = METHOD_GET;
    m->request_method = METHOD_POST;
    fail_if( r3_route_cmp(r1, m) == 0, "should be different");

    r1->request_method = METHOD_GET;
    m->request_method = METHOD_POST | METHOD_GET;
    fail_if( r3_route_cmp(r1, m) == -1, "should match");

    r3_route_free(r1);
    match_entry_free(m);
}
END_TEST


START_TEST(test_pcre_pattern_simple)
{
    match_entry * entry;
    entry = match_entry_createl( "/user/123" , strlen("/user/123") );
    node * n = r3_tree_create(10);
    r3_tree_insert_path(n, "/user/{id:\\d+}", NULL);
    r3_tree_insert_path(n, "/user", NULL);
    r3_tree_compile(n, NULL);
    // r3_tree_dump(n, 0);
    node *matched;
    matched = r3_tree_matchl(n, "/user/123", strlen("/user/123"), entry);
    ck_assert(matched);
    ck_assert(entry->vars->len > 0);
    ck_assert_str_eq(entry->vars->tokens[0],"123");
    r3_tree_free(n);
}
END_TEST


START_TEST(test_pcre_pattern_more)
{
    match_entry * entry;
    entry = match_entry_createl( "/user/123" , strlen("/user/123") );
    node * n = r3_tree_create(10);

    int var0 = 5;
    int var1 = 100;
    int var2 = 200;
    int var3 = 300;

    info("var0: %p\n", &var0);
    info("var1: %p\n", &var1);
    info("var2: %p\n", &var2);
    info("var3: %p\n", &var3);

    r3_tree_insert_path(n, "/user/{id:\\d+}", &var1);
    r3_tree_insert_path(n, "/user2/{id:\\d+}", &var2);
    r3_tree_insert_path(n, "/user3/{id:\\d{3}}", &var3);
    r3_tree_insert_path(n, "/user", &var0);
    r3_tree_compile(n, NULL);
    r3_tree_dump(n, 0);
    node *matched;

    matched = r3_tree_matchl(n, "/user/123", strlen("/user/123"), entry);
    ck_assert(matched);
    ck_assert(entry->vars->len > 0);
    ck_assert_str_eq(entry->vars->tokens[0],"123");

    info("matched %p\n", matched->data);
    info("matched %p\n", matched->data);
    ck_assert_int_eq( *((int*) matched->data), var1);

    matched = r3_tree_matchl(n, "/user2/123", strlen("/user2/123"), entry);
    ck_assert(matched);
    ck_assert(entry->vars->len > 0);
    ck_assert_str_eq(entry->vars->tokens[0],"123");
    ck_assert_int_eq( *((int*)matched->data), var2);

    matched = r3_tree_matchl(n, "/user3/123", strlen("/user3/123"), entry);
    ck_assert(matched);
    ck_assert(entry->vars->len > 0);
    ck_assert_str_eq(entry->vars->tokens[0],"123");
    ck_assert_int_eq( *((int*)matched->data), var3);

    r3_tree_free(n);
}
END_TEST




START_TEST(test_insert_route)
{
    int   var1 = 22;
    int   var2 = 33;

    match_entry * entry = match_entry_create("/blog/post");
    entry->request_method = METHOD_GET;

    node * n = r3_tree_create(2);
    r3_tree_insert_route(n, METHOD_GET, "/blog/post", &var1);
    r3_tree_insert_route(n, METHOD_POST, "/blog/post", &var2);

    route *c = r3_tree_match_route(n, entry);
    fail_if(c == NULL);

    r3_tree_free(n);
    match_entry_free(entry);
}
END_TEST


Suite* r3_suite (void) {
        Suite *suite = suite_create("blah");

        TCase *tcase = tcase_create("testcase");
        tcase_set_timeout(tcase, 30);
        tcase_add_test(tcase, test_r3_node_construct_and_free);
        tcase_add_test(tcase, test_str_array);
        tcase_add_test(tcase, test_ltrim_slash);
        tcase_add_test(tcase, test_r3_node_find_edge);
        tcase_add_test(tcase, testr3_tree_insert_pathl);
        tcase_add_test(tcase, test_compile);
        tcase_add_test(tcase, test_route_cmp);
        tcase_add_test(tcase, test_insert_route);
        tcase_add_test(tcase, test_pcre_pattern_simple);
        tcase_add_test(tcase, test_pcre_pattern_more);
        tcase_add_test(tcase, test_pcre_patterns_insert);
        tcase_add_test(tcase, test_pcre_patterns_insert_2);
        tcase_add_test(tcase, test_pcre_patterns_insert_3);
        tcase_add_test(tcase, test_incomplete_slug_path);
        tcase_set_timeout(tcase, 30);

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
