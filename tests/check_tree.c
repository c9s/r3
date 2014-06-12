#include "config.h"
#include <stdio.h>
#include <check.h>
#include <stdlib.h>
#include <assert.h>
#include "r3.h"
#include "r3_str.h"
#include "zmalloc.h"
#include "bench.h"

#define SAFE_FREE(ptr) if(ptr) free(ptr);

START_TEST (test_find_common_prefix)
{
    node * n = r3_tree_create(10);
    edge * e = r3_edge_createl(zstrdup("/foo/{slug}"), sizeof("/foo/{slug}")-1, NULL);
    r3_node_append_edge(n,e);

    char *errstr = NULL;
    int prefix_len = 0;
    edge *ret_edge = NULL;


    errstr = NULL;
    ret_edge = r3_node_find_common_prefix(n, "/foo", sizeof("/foo")-1, &prefix_len, &errstr);
    ck_assert(ret_edge != NULL);
    ck_assert_int_eq(prefix_len, 4);
    SAFE_FREE(errstr);


    errstr = NULL;
    ret_edge = r3_node_find_common_prefix(n, "/foo/", sizeof("/foo/")-1, &prefix_len, &errstr);
    ck_assert(ret_edge != NULL);
    ck_assert_int_eq(prefix_len, 5);
    SAFE_FREE(errstr);


    errstr = NULL;
    prefix_len = 0;
    ret_edge = r3_node_find_common_prefix(n, "/foo/{slog}", sizeof("/foo/{slog}")-1, &prefix_len, &errstr);
    ck_assert(ret_edge != NULL);
    ck_assert_int_eq(prefix_len, 5);
    SAFE_FREE(errstr);


    errstr = NULL;
    ret_edge = r3_node_find_common_prefix(n, "/foo/{bar}", sizeof("/foo/{bar}")-1, &prefix_len, &errstr);
    ck_assert(ret_edge != NULL);
    ck_assert_int_eq(prefix_len, 5);
    SAFE_FREE(errstr);


    errstr = NULL;
    ret_edge = r3_node_find_common_prefix(n, "/foo/bar", sizeof("/foo/bar")-1, &prefix_len, &errstr);
    ck_assert(ret_edge != NULL);
    ck_assert_int_eq(prefix_len, 5);
    SAFE_FREE(errstr);


    errstr = NULL;
    ret_edge = r3_node_find_common_prefix(n, "/bar/", sizeof("/bar/")-1, &prefix_len, &errstr);
    ck_assert(ret_edge != NULL);
    ck_assert_int_eq(prefix_len, 1);
    SAFE_FREE(errstr);


    errstr = NULL;
    ret_edge = r3_node_find_common_prefix(n, "{bar}", sizeof("{bar}")-1, &prefix_len, &errstr);
    ck_assert(ret_edge == NULL);
    ck_assert_int_eq(prefix_len, 0);
    SAFE_FREE(errstr);


    r3_tree_free(n);
}
END_TEST





START_TEST (test_find_common_prefix_after)
{
    node * n = r3_tree_create(10);
    edge * e = r3_edge_createl(zstrdup("{slug}/foo"), sizeof("{slug}/foo")-1, NULL);
    r3_node_append_edge(n,e);

    int prefix_len = 0;
    edge *ret_edge = NULL;
    char *errstr = NULL;

    errstr = NULL;
    ret_edge = r3_node_find_common_prefix(n, "/foo", sizeof("/foo")-1, &prefix_len, &errstr);
    ck_assert(ret_edge == NULL);
    ck_assert_int_eq(prefix_len, 0);
    SAFE_FREE(errstr);


    errstr = NULL;
    ret_edge = r3_node_find_common_prefix(n, "{slug}/bar", sizeof("{slug}/bar")-1, &prefix_len, &errstr);
    ck_assert(ret_edge != NULL);
    ck_assert_int_eq(prefix_len, 7);
    SAFE_FREE(errstr);


    errstr = NULL;
    ret_edge = r3_node_find_common_prefix(n, "{slug}/foo", sizeof("{slug}/foo")-1, &prefix_len, &errstr);
    ck_assert(ret_edge != NULL);
    ck_assert_int_eq(prefix_len, 10);
    SAFE_FREE(errstr);


    r3_tree_free(n);
}
END_TEST



START_TEST (test_find_common_prefix_double_middle)
{
    node * n = r3_tree_create(10);
    edge * e = r3_edge_createl(zstrdup("{slug}/foo/{name}"), sizeof("{slug}/foo/{name}")-1, NULL);
    r3_node_append_edge(n,e);

    int prefix_len;
    edge *ret_edge = NULL;
    char *errstr;

    errstr = NULL;
    ret_edge = r3_node_find_common_prefix(n, "{slug}/foo/{number}", sizeof("{slug}/foo/{number}")-1, &prefix_len, &errstr);
    ck_assert(ret_edge);
    ck_assert_int_eq(prefix_len, 11);
    SAFE_FREE(errstr);

    r3_tree_free(n);
}
END_TEST



START_TEST (test_find_common_prefix_middle)
{
    node * n = r3_tree_create(10);
    edge * e = r3_edge_createl(zstrdup("/foo/{slug}/hate"), sizeof("/foo/{slug}/hate")-1, NULL);
    r3_node_append_edge(n,e);

    int prefix_len;
    edge *ret_edge = NULL;
    char *errstr = NULL;

    errstr = NULL;
    ret_edge = r3_node_find_common_prefix(n, "/foo/{slug}/bar", sizeof("/foo/{slug}/bar")-1, &prefix_len, &errstr);
    ck_assert(ret_edge);
    ck_assert_int_eq(prefix_len, 12);
    SAFE_FREE(errstr);

    errstr = NULL;
    ret_edge = r3_node_find_common_prefix(n, "/fo/{slug}/bar", sizeof("/fo/{slug}/bar")-1, &prefix_len, &errstr);
    ck_assert(ret_edge);
    ck_assert_int_eq(prefix_len, 3);
    SAFE_FREE(errstr);

    r3_tree_free(n);
}
END_TEST

START_TEST (test_find_common_prefix_same_pattern)
{
    node * n = r3_tree_create(10);
    edge * e = r3_edge_createl(zstrdup("/foo/{slug:xxx}/hate"), sizeof("/foo/{slug:xxx}/hate")-1, NULL);
    r3_node_append_edge(n,e);

    int prefix_len;
    edge *ret_edge = NULL;

    prefix_len = 0;
    ret_edge = r3_node_find_common_prefix(n, "/foo/{slug:yyy}/hate", sizeof("/foo/{slug:yyy}/hate")-1, &prefix_len, NULL);
    ck_assert(ret_edge);
    ck_assert_int_eq(prefix_len, 5);


    prefix_len = 0;
    ret_edge = r3_node_find_common_prefix(n, "/foo/{slug}/hate", sizeof("/foo/{slug}/hate")-1, &prefix_len, NULL);
    ck_assert(ret_edge != NULL);
    ck_assert_int_eq(prefix_len, 5);

    r3_tree_free(n);
}
END_TEST

START_TEST (test_find_common_prefix_same_pattern2)
{
    node * n = r3_tree_create(10);
    edge * e = r3_edge_createl(zstrdup("{slug:xxx}/hate"), sizeof("{slug:xxx}/hate")-1, NULL);
    r3_node_append_edge(n,e);

    int prefix_len;
    edge *ret_edge = NULL;

    prefix_len = 0;
    ret_edge = r3_node_find_common_prefix(n, "{slug:yyy}/hate", sizeof("{slug:yyy}/hate")-1, &prefix_len, NULL);
    ck_assert(ret_edge);
    ck_assert_int_eq(prefix_len, 0);

    r3_tree_free(n);
}
END_TEST

















START_TEST (test_node_construct_and_free)
{
    node * n = r3_tree_create(10);
    node * another_tree = r3_tree_create(3);
    r3_tree_free(n);
    r3_tree_free(another_tree);
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

    ret_node = r3_tree_insert_path(n, "/users/{idx:\\d{3}}/{idy}", NULL);
    ck_assert(ret_node);

    // OK to insert, but should return error when compiling patterns
    node * ret_node2 = r3_tree_insert_path(n, "/users/{idx:\\d{3}}/{idy:aaa}", NULL);
    ck_assert(ret_node2);
    ck_assert(ret_node2 != ret_node); // make sure it's another node


    char *errstr = NULL;
    r3_tree_compile(n, &errstr);
    ck_assert(errstr == NULL); // no error

    r3_tree_dump(n, 0);

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



START_TEST (test_insert_pathl_fail)
{
    node * n = r3_tree_create(10);

    node * ret;

    char *errstr = NULL;
    ret = r3_tree_insert_pathl_ex(n, "/foo/{name:\\d{5}", strlen("/foo/{name:\\d{5}"),  NULL, NULL, &errstr);
    ck_assert(ret == NULL);
    ck_assert(errstr != NULL);
    printf("%s\n", errstr); // Returns Incomplete slug pattern. PATTERN (16): '/foo/{name:\d{5}', OFFSET: 16, STATE: 1
    SAFE_FREE(errstr);

    errstr = NULL;
    r3_tree_compile(n, &errstr);
    ck_assert(errstr == NULL);

    r3_tree_free(n);
}
END_TEST




START_TEST (test_insert_pathl)
{
    node * n = r3_tree_create(10);

    node * ret;

    ret = r3_tree_insert_path(n, "/foo/bar",  NULL);
    ck_assert(ret);
    ret = r3_tree_insert_path(n, "/foo/zoo",  NULL);
    ck_assert(ret);
    ret = r3_tree_insert_path(n, "/foo/{id}",  NULL);
    ck_assert(ret);
    ret = r3_tree_insert_path(n, "/foo/{number:\\d+}",  NULL);
    ck_assert(ret);
    ret = r3_tree_insert_path(n, "/foo/{name:\\w+}",  NULL);
    ck_assert(ret);
    ret = r3_tree_insert_path(n, "/foo/{name:\\d+}",  NULL);
    ck_assert(ret);

    ret = r3_tree_insert_path(n, "/foo/{name:\\d{5}}",  NULL);
    ck_assert(ret);

    ret = r3_tree_insert_path(n, "/foo/{idx}/{idy}",  NULL);
    ck_assert(ret);

    ret = r3_tree_insert_path(n, "/foo/{idx}/{idh}",  NULL);
    ck_assert(ret);

    ret = r3_tree_insert_path(n, "/f/id" ,  NULL);
    ck_assert(ret);
    ret = r3_tree_insert_path(n, "/post/{id}", NULL);
    ck_assert(ret);
    ret = r3_tree_insert_path(n, "/post/{handle}", NULL);
    ck_assert(ret);
    ret = r3_tree_insert_path(n, "/post/{handle}-{id}", NULL);
    ck_assert(ret);

    char * errstr = NULL;
    r3_tree_compile(n, &errstr);
    ck_assert(errstr == NULL);

    r3_tree_dump(n, 0);
    r3_tree_free(n);
}
END_TEST



START_TEST (test_compile_fail)
{
    node * n = r3_tree_create(10);

    node * ret;

    ret = r3_tree_insert_path(n, "/foo/{idx}/{idy:)}",  NULL);
    ck_assert(ret);

    ret = r3_tree_insert_path(n, "/foo/{idx}/{idh:(}",  NULL);
    ck_assert(ret);

    char * errstr = NULL;
    r3_tree_compile(n, &errstr);
    ck_assert(errstr);
    fprintf(stderr, "Compile failed: %s\n", errstr);
    free(errstr);

    r3_tree_dump(n, 0);
    r3_tree_free(n);
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


    node * n = r3_tree_create(2);
    r3_tree_insert_route(n, METHOD_GET, "/blog/post", &var1);
    r3_tree_insert_route(n, METHOD_POST, "/blog/post", &var2);

    match_entry * entry;
    route *r;

    entry = match_entry_create("/blog/post");
    entry->request_method = METHOD_GET;
    r = r3_tree_match_route(n, entry);
    ck_assert(r != NULL);
    ck_assert(r->request_method & METHOD_GET );
    ck_assert(*((int*)r->data) == 22);
    match_entry_free(entry);


    entry = match_entry_create("/blog/post");
    entry->request_method = METHOD_POST;
    r = r3_tree_match_route(n, entry);
    ck_assert(r != NULL);
    ck_assert(r->request_method & METHOD_POST );
    ck_assert(*((int*)r->data) == 33);
    match_entry_free(entry);



    r3_tree_free(n);
}
END_TEST


Suite* r3_suite (void) {
        Suite *suite = suite_create("r3 core tests");

        TCase *tcase = tcase_create("testcase");

        tcase_add_test(tcase, test_find_common_prefix);
        tcase_add_test(tcase, test_find_common_prefix_after);
        tcase_add_test(tcase, test_find_common_prefix_double_middle);
        tcase_add_test(tcase, test_find_common_prefix_middle);
        tcase_add_test(tcase, test_find_common_prefix_same_pattern);
        tcase_add_test(tcase, test_find_common_prefix_same_pattern2);
        tcase_add_test(tcase, test_insert_pathl);
        tcase_add_test(tcase, test_insert_pathl_fail);
        tcase_add_test(tcase, test_node_construct_and_free);
        tcase_add_test(tcase, test_compile);
        tcase_add_test(tcase, test_compile_fail);
        tcase_add_test(tcase, test_route_cmp);
        tcase_add_test(tcase, test_insert_route);
        tcase_add_test(tcase, test_pcre_pattern_simple);
        tcase_add_test(tcase, test_pcre_pattern_more);
        tcase_add_test(tcase, test_pcre_patterns_insert);
        tcase_add_test(tcase, test_pcre_patterns_insert_2);
        tcase_add_test(tcase, test_pcre_patterns_insert_3);
        tcase_add_test(tcase, test_incomplete_slug_path);
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
