#include <stdio.h>
#include <check.h>
#include <stdlib.h>
#include "str.h"
#include "node.h"
#include "token.h"

#include <sys/time.h>

#define MICRO_IN_SEC 1000000.00
#define SEC_IN_MIN 60
#define NUL  '\0'
double microtime() {
    struct timeval tp;
    long sec = 0L;
    double msec = 0.0;
    char ret[100];
    
    if (gettimeofday((struct timeval *) &tp, (NUL)) == 0) {
        msec = (double) (tp.tv_usec / MICRO_IN_SEC);
        sec = tp.tv_sec;
        if (msec >= 1.0)
            msec -= (long) msec;
        return sec + msec;
    }
    return 0;
}


START_TEST (test_ltrim_slash)
{
    fail_if( strcmp( ltrim_slash("/blog") , "blog" ) != 0 );
    fail_if( strcmp( ltrim_slash("blog") , "blog" ) != 0 );
}
END_TEST

START_TEST (test_node_construct_uniq)
{
    node * n = rtree_create(10);

    node * child = rtree_create(3);

    // fail_if( rtree_add_child(n, strdup("/add") , child) != NULL );
    // fail_if( rtree_add_child(n, strdup("/add") , child) != NULL );

    rtree_free(n);
}
END_TEST

START_TEST (test_node_find_edge)
{
    node * n = rtree_create(10);

    node * child = rtree_create(3);

    fail_if( rtree_add_child(n, strdup("/add") , child) == FALSE );

    fail_if( node_find_edge(n, "/add") == NULL );
    fail_if( node_find_edge(n, "/bar") != NULL );

    rtree_free(n);
}
END_TEST


START_TEST (test_compile)
{
    str_array *t;
    node * n;
    n = rtree_create(10);


    match_entry * entry;
    node *m;
    edge *e ;

    rtree_insert_pathn(n, "/zoo", strlen("/zoo"), NULL);
    rtree_insert_pathn(n, "/foo", strlen("/foo"), NULL);
    rtree_insert_pathn(n, "/bar", strlen("/bar"), NULL);
    rtree_compile(n);
    fail_if( n->combined_pattern );
    fail_if( NULL == node_find_edge_str(n, "/", strlen("/") ) );

#ifdef DEBUG
    rtree_dump(n, 0);
#endif

    rtree_insert_pathn(n, "/foo/{id}", strlen("/foo/{id}"), NULL);
    rtree_insert_pathn(n, "/{id}", strlen("/{id}"), NULL);
    rtree_compile(n);
    rtree_compile(n); // test double compile
#ifdef DEBUG
    rtree_dump(n, 0);
#endif
    /*
    fail_if(n->edges[0]->child->combined_pattern == NULL);

    e = node_find_edge_str(n, "/", strlen("/") );
    fail_if( NULL == e );
    */
    /*
    printf( "%s\n", e->pattern );
    printf( "%s\n", e->child->combined_pattern );
    printf( "%s\n", n->edges[0]->child->combined_pattern);
    printf( "%s\n", n->combined_pattern );
    */

    entry = calloc( sizeof(entry) , 1 );


    m = rtree_match( n , "/foo", strlen("/foo"), entry);
    fail_if( NULL == m );

    m = rtree_match( n , "/zoo", strlen("/zoo"), entry);
    fail_if( NULL == m );

    m = rtree_match( n , "/bar", strlen("/bar"), entry);
    fail_if( NULL == m );

    m = rtree_match( n , "/xxx", strlen("/xxx"), entry);
    fail_if( NULL == m );

    m = rtree_match( n , "/foo/xxx", strlen("/foo/xxx"), entry);
    fail_if( NULL == m );

    m = rtree_match( n , "/some_id", strlen("/some_id"), entry);
    fail_if( NULL == m );
    ck_assert_int_gt( m->endpoint , 0 ); // should not be an endpoint
}
END_TEST

START_TEST (test_compile_slug)
{
    /*
    char * pattern;
    pattern = compile_slug("{id}", strlen("{id}"));
    ck_assert_str_eq( pattern, "([^/]+)" );
    free(pattern);

    pattern = compile_slug("/{id}", strlen("/{id}"));
    ck_assert_str_eq( pattern, "/([^/]+)" );
    free(pattern);

    pattern = compile_slug("-{id}", strlen("-{id}"));
    ck_assert_str_eq( pattern, "-([^-]+)" );
    free(pattern);

    pattern = compile_slug("{id}-{title}", strlen("{id}-{title}"));
    ck_assert_str_eq( pattern, "([^/]+)-([^-]+)" );
    free(pattern);


    pattern = compile_slug("{id:[a-z]+}", strlen("{id:[a-z]+}") );
    ck_assert_str_eq( pattern, "([a-z]+)" );
    free(pattern);


    pattern = compile_slug("/path/{id:[a-z]+}", strlen("/path/{id:[a-z]+}") );
    ck_assert_str_eq( pattern, "/path/([a-z]+)" );
    free(pattern);
    */

    /*
    char * p = malloc(sizeof(char) * 10);
    strncat(p, "foo", 3);
    free(p);
    */





}
END_TEST


START_TEST (test_rtree_insert_pathn)
{
    node * n = rtree_create(10);

    info("Inserting /foo/bar\n");
    rtree_insert_path(n, "/foo/bar", NULL);
    // rtree_dump(n, 0);

    info("Inserting /foo/zoo\n");
    rtree_insert_path(n, "/foo/zoo", NULL);
    // rtree_dump(n, 0);

    info("Inserting /f/id\n");
    rtree_insert_path(n, "/f/id" , NULL);
    // rtree_dump(n, 0);

    info("Inserting /post/{id}\n");
    rtree_insert_pathn(n, "/post/{id}", strlen("/post/{id}"), NULL);
    // rtree_dump(n, 0);

    info("Inserting /post/{handle}\n");
    rtree_insert_pathn(n, "/post/{handle}", strlen("/post/{handle}"), NULL);
    // rtree_dump(n, 0);

    info("Inserting /post/{handle}-{id}\n");
    rtree_insert_pathn(n, "/post/{handle}-{id}", strlen("/post/{handle}-{id}"), NULL);
    rtree_compile(n);

#ifdef DEBUG
    rtree_dump(n, 0);
#endif
    rtree_free(n);
}
END_TEST




START_TEST (test_route_split)
{
    str_array *t;

    t = split_route_pattern("/blog", strlen("/blog") );
    fail_if( t == NULL );
    str_array_dump(t);
    str_array_free(t);

    t = split_route_pattern("/foo/{id}", strlen("/foo/{id}") );
    fail_if( t == NULL );
    str_array_dump(t);
    fail_if( t->len != 2 );
    str_array_free(t);

    t = split_route_pattern("/foo/bar/{id}", strlen("/foo/bar/{id}") );
    fail_if( t == NULL );
    str_array_dump(t);
    fail_if( t->len != 3 );
    str_array_free(t);

    t = split_route_pattern("/{title}", strlen("/{title}") );
    fail_if( t == NULL );
    str_array_dump(t);
    fail_if( t->len != 1 );
    str_array_free(t);

    t = split_route_pattern("/", strlen("/") );
    fail_if( t == NULL );
    str_array_dump(t);
    fail_if( t->len != 1 );
    str_array_free(t);

}
END_TEST

START_TEST (test_str_array)
{
    str_array * l = str_array_create(3);
    fail_if( l == NULL );

    fail_if( FALSE == str_array_append(l, strdup("abc") ) );
    fail_if( l->len != 1 );

    fail_if( FALSE == str_array_append(l, strdup("foo") ) );
    fail_if( l->len != 2 );

    fail_if( FALSE == str_array_append(l, strdup("bar") ) );
    fail_if( l->len != 3 );

    fail_if( FALSE == str_array_append(l, strdup("zoo") ) );
    fail_if( l->len != 4 );

    fail_if( FALSE == str_array_resize(l, l->cap * 2) );

    str_array_free(l);
}
END_TEST

START_TEST(benchmark_str)
{
    match_entry * entry = calloc( sizeof(entry) , 1 );
    node * n = rtree_create(1);

    rtree_insert_path(n, "/foo/bar/baz", NULL);
    rtree_insert_path(n, "/foo/bar/qux", NULL);
    rtree_insert_path(n, "/foo/bar/quux", NULL);
    rtree_insert_path(n, "/foo/bar/corge", NULL);
    rtree_insert_path(n, "/foo/bar/grault", NULL);
    rtree_insert_path(n, "/foo/bar/garply", NULL);
    rtree_insert_path(n, "/foo/baz/bar", NULL);
    rtree_insert_path(n, "/foo/baz/qux", NULL);
    rtree_insert_path(n, "/foo/baz/quux", NULL);
    rtree_insert_path(n, "/foo/baz/corge", NULL);
    rtree_insert_path(n, "/foo/baz/grault", NULL);
    rtree_insert_path(n, "/foo/baz/garply", NULL);
    rtree_insert_path(n, "/foo/qux/bar", NULL);
    rtree_insert_path(n, "/foo/qux/baz", NULL);
    rtree_insert_path(n, "/foo/qux/quux", NULL);
    rtree_insert_path(n, "/foo/qux/corge", NULL);
    rtree_insert_path(n, "/foo/qux/grault", NULL);
    rtree_insert_path(n, "/foo/qux/garply", NULL);
    rtree_insert_path(n, "/foo/quux/bar", NULL);
    rtree_insert_path(n, "/foo/quux/baz", NULL);
    rtree_insert_path(n, "/foo/quux/qux", NULL);
    rtree_insert_path(n, "/foo/quux/corge", NULL);
    rtree_insert_path(n, "/foo/quux/grault", NULL);
    rtree_insert_path(n, "/foo/quux/garply", NULL);
    rtree_insert_path(n, "/foo/corge/bar", NULL);
    rtree_insert_path(n, "/foo/corge/baz", NULL);
    rtree_insert_path(n, "/foo/corge/qux", NULL);
    rtree_insert_path(n, "/foo/corge/quux", NULL);
    rtree_insert_path(n, "/foo/corge/grault", NULL);
    rtree_insert_path(n, "/foo/corge/garply", NULL);
    rtree_insert_path(n, "/foo/grault/bar", NULL);
    rtree_insert_path(n, "/foo/grault/baz", NULL);
    rtree_insert_path(n, "/foo/grault/qux", NULL);
    rtree_insert_path(n, "/foo/grault/quux", NULL);
    rtree_insert_path(n, "/foo/grault/corge", NULL);
    rtree_insert_path(n, "/foo/grault/garply", NULL);
    rtree_insert_path(n, "/foo/garply/bar", NULL);
    rtree_insert_path(n, "/foo/garply/baz", NULL);
    rtree_insert_path(n, "/foo/garply/qux", NULL);
    rtree_insert_path(n, "/foo/garply/quux", NULL);
    rtree_insert_path(n, "/foo/garply/corge", NULL);
    rtree_insert_path(n, "/foo/garply/grault", NULL);
    rtree_insert_path(n, "/bar/foo/baz", NULL);
    rtree_insert_path(n, "/bar/foo/qux", NULL);
    rtree_insert_path(n, "/bar/foo/quux", NULL);
    rtree_insert_path(n, "/bar/foo/corge", NULL);
    rtree_insert_path(n, "/bar/foo/grault", NULL);
    rtree_insert_path(n, "/bar/foo/garply", NULL);
    rtree_insert_path(n, "/bar/baz/foo", NULL);
    rtree_insert_path(n, "/bar/baz/qux", NULL);
    rtree_insert_path(n, "/bar/baz/quux", NULL);
    rtree_insert_path(n, "/bar/baz/corge", NULL);
    rtree_insert_path(n, "/bar/baz/grault", NULL);
    rtree_insert_path(n, "/bar/baz/garply", NULL);
    rtree_insert_path(n, "/bar/qux/foo", NULL);
    rtree_insert_path(n, "/bar/qux/baz", NULL);
    rtree_insert_path(n, "/bar/qux/quux", NULL);
    rtree_insert_path(n, "/bar/qux/corge", NULL);
    rtree_insert_path(n, "/bar/qux/grault", NULL);
    rtree_insert_path(n, "/bar/qux/garply", NULL);
    rtree_insert_path(n, "/bar/quux/foo", NULL);
    rtree_insert_path(n, "/bar/quux/baz", NULL);
    rtree_insert_path(n, "/bar/quux/qux", NULL);
    rtree_insert_path(n, "/bar/quux/corge", NULL);
    rtree_insert_path(n, "/bar/quux/grault", NULL);
    rtree_insert_path(n, "/bar/quux/garply", NULL);
    rtree_insert_path(n, "/bar/corge/foo", NULL);
    rtree_insert_path(n, "/bar/corge/baz", NULL);
    rtree_insert_path(n, "/bar/corge/qux", NULL);
    rtree_insert_path(n, "/bar/corge/quux", NULL);
    rtree_insert_path(n, "/bar/corge/grault", NULL);
    rtree_insert_path(n, "/bar/corge/garply", NULL);
    rtree_insert_path(n, "/bar/grault/foo", NULL);
    rtree_insert_path(n, "/bar/grault/baz", NULL);
    rtree_insert_path(n, "/bar/grault/qux", NULL);
    rtree_insert_path(n, "/bar/grault/quux", NULL);
    rtree_insert_path(n, "/bar/grault/corge", NULL);
    rtree_insert_path(n, "/bar/grault/garply", NULL);
    rtree_insert_path(n, "/bar/garply/foo", NULL);
    rtree_insert_path(n, "/bar/garply/baz", NULL);
    rtree_insert_path(n, "/bar/garply/qux", NULL);
    rtree_insert_path(n, "/bar/garply/quux", NULL);
    rtree_insert_path(n, "/bar/garply/corge", NULL);
    rtree_insert_path(n, "/bar/garply/grault", NULL);
    rtree_insert_path(n, "/baz/foo/bar", NULL);
    rtree_insert_path(n, "/baz/foo/qux", NULL);
    rtree_insert_path(n, "/baz/foo/quux", NULL);
    rtree_insert_path(n, "/baz/foo/corge", NULL);
    rtree_insert_path(n, "/baz/foo/grault", NULL);
    rtree_insert_path(n, "/baz/foo/garply", NULL);
    rtree_insert_path(n, "/baz/bar/foo", NULL);
    rtree_insert_path(n, "/baz/bar/qux", NULL);
    rtree_insert_path(n, "/baz/bar/quux", NULL);
    rtree_insert_path(n, "/baz/bar/corge", NULL);
    rtree_insert_path(n, "/baz/bar/grault", NULL);
    rtree_insert_path(n, "/baz/bar/garply", NULL);
    rtree_insert_path(n, "/baz/qux/foo", NULL);
    rtree_insert_path(n, "/baz/qux/bar", NULL);
    rtree_insert_path(n, "/baz/qux/quux", NULL);
    rtree_insert_path(n, "/baz/qux/corge", NULL);
    rtree_insert_path(n, "/baz/qux/grault", NULL);
    rtree_insert_path(n, "/baz/qux/garply", NULL);
    rtree_insert_path(n, "/baz/quux/foo", NULL);
    rtree_insert_path(n, "/baz/quux/bar", NULL);
    rtree_insert_path(n, "/baz/quux/qux", NULL);
    rtree_insert_path(n, "/baz/quux/corge", NULL);
    rtree_insert_path(n, "/baz/quux/grault", NULL);
    rtree_insert_path(n, "/baz/quux/garply", NULL);
    rtree_insert_path(n, "/baz/corge/foo", NULL);
    rtree_insert_path(n, "/baz/corge/bar", NULL);
    rtree_insert_path(n, "/baz/corge/qux", NULL);
    rtree_insert_path(n, "/baz/corge/quux", NULL);
    rtree_insert_path(n, "/baz/corge/grault", NULL);
    rtree_insert_path(n, "/baz/corge/garply", NULL);
    rtree_insert_path(n, "/baz/grault/foo", NULL);
    rtree_insert_path(n, "/baz/grault/bar", NULL);
    rtree_insert_path(n, "/baz/grault/qux", NULL);
    rtree_insert_path(n, "/baz/grault/quux", NULL);
    rtree_insert_path(n, "/baz/grault/corge", NULL);
    rtree_insert_path(n, "/baz/grault/garply", NULL);
    rtree_insert_path(n, "/baz/garply/foo", NULL);
    rtree_insert_path(n, "/baz/garply/bar", NULL);
    rtree_insert_path(n, "/baz/garply/qux", NULL);
    rtree_insert_path(n, "/baz/garply/quux", NULL);
    rtree_insert_path(n, "/baz/garply/corge", NULL);
    rtree_insert_path(n, "/baz/garply/grault", NULL);
    rtree_insert_path(n, "/qux/foo/bar", NULL);
    rtree_insert_path(n, "/qux/foo/baz", NULL);
    rtree_insert_path(n, "/qux/foo/quux", NULL);
    rtree_insert_path(n, "/qux/foo/corge", NULL);
    rtree_insert_path(n, "/qux/foo/grault", NULL);
    rtree_insert_path(n, "/qux/foo/garply", NULL);
    rtree_insert_path(n, "/qux/bar/foo", NULL);
    rtree_insert_path(n, "/qux/bar/baz", NULL);
    rtree_insert_path(n, "/qux/bar/quux", NULL);
    rtree_insert_path(n, "/qux/bar/corge",  (void*) 999);
    rtree_insert_path(n, "/qux/bar/grault", NULL);
    rtree_insert_path(n, "/qux/bar/garply", NULL);
    rtree_insert_path(n, "/qux/baz/foo", NULL);
    rtree_insert_path(n, "/qux/baz/bar", NULL);
    rtree_insert_path(n, "/qux/baz/quux", NULL);
    rtree_insert_path(n, "/qux/baz/corge", NULL);
    rtree_insert_path(n, "/qux/baz/grault", NULL);
    rtree_insert_path(n, "/qux/baz/garply", NULL);
    rtree_insert_path(n, "/qux/quux/foo", NULL);
    rtree_insert_path(n, "/qux/quux/bar", NULL);
    rtree_insert_path(n, "/qux/quux/baz", NULL);
    rtree_insert_path(n, "/qux/quux/corge", NULL);
    rtree_insert_path(n, "/qux/quux/grault", NULL);
    rtree_insert_path(n, "/qux/quux/garply", NULL);
    rtree_insert_path(n, "/qux/corge/foo", NULL);
    rtree_insert_path(n, "/qux/corge/bar", NULL);
    rtree_insert_path(n, "/qux/corge/baz", NULL);
    rtree_insert_path(n, "/qux/corge/quux", NULL);
    rtree_insert_path(n, "/qux/corge/grault", NULL);
    rtree_insert_path(n, "/qux/corge/garply", NULL);
    rtree_insert_path(n, "/qux/grault/foo", NULL);
    rtree_insert_path(n, "/qux/grault/bar", NULL);
    rtree_insert_path(n, "/qux/grault/baz", NULL);
    rtree_insert_path(n, "/qux/grault/quux", NULL);
    rtree_insert_path(n, "/qux/grault/corge", NULL);
    rtree_insert_path(n, "/qux/grault/garply", NULL);
    rtree_insert_path(n, "/qux/garply/foo", NULL);
    rtree_insert_path(n, "/qux/garply/bar", NULL);
    rtree_insert_path(n, "/qux/garply/baz", NULL);
    rtree_insert_path(n, "/qux/garply/quux", NULL);
    rtree_insert_path(n, "/qux/garply/corge", NULL);
    rtree_insert_path(n, "/qux/garply/grault", NULL);
    rtree_insert_path(n, "/quux/foo/bar", NULL);
    rtree_insert_path(n, "/quux/foo/baz", NULL);
    rtree_insert_path(n, "/quux/foo/qux", NULL);
    rtree_insert_path(n, "/quux/foo/corge", NULL);
    rtree_insert_path(n, "/quux/foo/grault", NULL);
    rtree_insert_path(n, "/quux/foo/garply", NULL);
    rtree_insert_path(n, "/quux/bar/foo", NULL);
    rtree_insert_path(n, "/quux/bar/baz", NULL);
    rtree_insert_path(n, "/quux/bar/qux", NULL);
    rtree_insert_path(n, "/quux/bar/corge", NULL);
    rtree_insert_path(n, "/quux/bar/grault", NULL);
    rtree_insert_path(n, "/quux/bar/garply", NULL);
    rtree_insert_path(n, "/quux/baz/foo", NULL);
    rtree_insert_path(n, "/quux/baz/bar", NULL);
    rtree_insert_path(n, "/quux/baz/qux", NULL);
    rtree_insert_path(n, "/quux/baz/corge", NULL);
    rtree_insert_path(n, "/quux/baz/grault", NULL);
    rtree_insert_path(n, "/quux/baz/garply", NULL);
    rtree_insert_path(n, "/quux/qux/foo", NULL);
    rtree_insert_path(n, "/quux/qux/bar", NULL);
    rtree_insert_path(n, "/quux/qux/baz", NULL);
    rtree_insert_path(n, "/quux/qux/corge", NULL);
    rtree_insert_path(n, "/quux/qux/grault", NULL);
    rtree_insert_path(n, "/quux/qux/garply", NULL);
    rtree_insert_path(n, "/quux/corge/foo", NULL);
    rtree_insert_path(n, "/quux/corge/bar", NULL);
    rtree_insert_path(n, "/quux/corge/baz", NULL);
    rtree_insert_path(n, "/quux/corge/qux", NULL);
    rtree_insert_path(n, "/quux/corge/grault", NULL);
    rtree_insert_path(n, "/quux/corge/garply", NULL);
    rtree_insert_path(n, "/quux/grault/foo", NULL);
    rtree_insert_path(n, "/quux/grault/bar", NULL);
    rtree_insert_path(n, "/quux/grault/baz", NULL);
    rtree_insert_path(n, "/quux/grault/qux", NULL);
    rtree_insert_path(n, "/quux/grault/corge", NULL);
    rtree_insert_path(n, "/quux/grault/garply", NULL);
    rtree_insert_path(n, "/quux/garply/foo", NULL);
    rtree_insert_path(n, "/quux/garply/bar", NULL);
    rtree_insert_path(n, "/quux/garply/baz", NULL);
    rtree_insert_path(n, "/quux/garply/qux", NULL);
    rtree_insert_path(n, "/quux/garply/corge", NULL);
    rtree_insert_path(n, "/quux/garply/grault", NULL);
    rtree_insert_path(n, "/corge/foo/bar", NULL);
    rtree_insert_path(n, "/corge/foo/baz", NULL);
    rtree_insert_path(n, "/corge/foo/qux", NULL);
    rtree_insert_path(n, "/corge/foo/quux", NULL);
    rtree_insert_path(n, "/corge/foo/grault", NULL);
    rtree_insert_path(n, "/corge/foo/garply", NULL);
    rtree_insert_path(n, "/corge/bar/foo", NULL);
    rtree_insert_path(n, "/corge/bar/baz", NULL);
    rtree_insert_path(n, "/corge/bar/qux", NULL);
    rtree_insert_path(n, "/corge/bar/quux", NULL);
    rtree_insert_path(n, "/corge/bar/grault", NULL);
    rtree_insert_path(n, "/corge/bar/garply", NULL);
    rtree_insert_path(n, "/corge/baz/foo", NULL);
    rtree_insert_path(n, "/corge/baz/bar", NULL);
    rtree_insert_path(n, "/corge/baz/qux", NULL);
    rtree_insert_path(n, "/corge/baz/quux", NULL);
    rtree_insert_path(n, "/corge/baz/grault", NULL);
    rtree_insert_path(n, "/corge/baz/garply", NULL);
    rtree_insert_path(n, "/corge/qux/foo", NULL);
    rtree_insert_path(n, "/corge/qux/bar", NULL);
    rtree_insert_path(n, "/corge/qux/baz", NULL);
    rtree_insert_path(n, "/corge/qux/quux", NULL);
    rtree_insert_path(n, "/corge/qux/grault", NULL);
    rtree_insert_path(n, "/corge/qux/garply", NULL);
    rtree_insert_path(n, "/corge/quux/foo", NULL);
    rtree_insert_path(n, "/corge/quux/bar", NULL);
    rtree_insert_path(n, "/corge/quux/baz", NULL);
    rtree_insert_path(n, "/corge/quux/qux", NULL);
    rtree_insert_path(n, "/corge/quux/grault", NULL);
    rtree_insert_path(n, "/corge/quux/garply", NULL);
    rtree_insert_path(n, "/corge/grault/foo", NULL);
    rtree_insert_path(n, "/corge/grault/bar", NULL);
    rtree_insert_path(n, "/corge/grault/baz", NULL);
    rtree_insert_path(n, "/corge/grault/qux", NULL);
    rtree_insert_path(n, "/corge/grault/quux", NULL);
    rtree_insert_path(n, "/corge/grault/garply", NULL);
    rtree_insert_path(n, "/corge/garply/foo", NULL);
    rtree_insert_path(n, "/corge/garply/bar", NULL);
    rtree_insert_path(n, "/corge/garply/baz", NULL);
    rtree_insert_path(n, "/corge/garply/qux", NULL);
    rtree_insert_path(n, "/corge/garply/quux", NULL);
    rtree_insert_path(n, "/corge/garply/grault", NULL);
    rtree_insert_path(n, "/grault/foo/bar", NULL);
    rtree_insert_path(n, "/grault/foo/baz", NULL);
    rtree_insert_path(n, "/grault/foo/qux", NULL);
    rtree_insert_path(n, "/grault/foo/quux", NULL);
    rtree_insert_path(n, "/grault/foo/corge", NULL);
    rtree_insert_path(n, "/grault/foo/garply", NULL);
    rtree_insert_path(n, "/grault/bar/foo", NULL);
    rtree_insert_path(n, "/grault/bar/baz", NULL);
    rtree_insert_path(n, "/grault/bar/qux", NULL);
    rtree_insert_path(n, "/grault/bar/quux", NULL);
    rtree_insert_path(n, "/grault/bar/corge", NULL);
    rtree_insert_path(n, "/grault/bar/garply", NULL);
    rtree_insert_path(n, "/grault/baz/foo", NULL);
    rtree_insert_path(n, "/grault/baz/bar", NULL);
    rtree_insert_path(n, "/grault/baz/qux", NULL);
    rtree_insert_path(n, "/grault/baz/quux", NULL);
    rtree_insert_path(n, "/grault/baz/corge", NULL);
    rtree_insert_path(n, "/grault/baz/garply", NULL);
    rtree_insert_path(n, "/grault/qux/foo", NULL);
    rtree_insert_path(n, "/grault/qux/bar", NULL);
    rtree_insert_path(n, "/grault/qux/baz", NULL);
    rtree_insert_path(n, "/grault/qux/quux", NULL);
    rtree_insert_path(n, "/grault/qux/corge", NULL);
    rtree_insert_path(n, "/grault/qux/garply", NULL);
    rtree_insert_path(n, "/grault/quux/foo", NULL);
    rtree_insert_path(n, "/grault/quux/bar", NULL);
    rtree_insert_path(n, "/grault/quux/baz", NULL);
    rtree_insert_path(n, "/grault/quux/qux", NULL);
    rtree_insert_path(n, "/grault/quux/corge", NULL);
    rtree_insert_path(n, "/grault/quux/garply", NULL);
    rtree_insert_path(n, "/grault/corge/foo", NULL);
    rtree_insert_path(n, "/grault/corge/bar", NULL);
    rtree_insert_path(n, "/grault/corge/baz", NULL);
    rtree_insert_path(n, "/grault/corge/qux", NULL);
    rtree_insert_path(n, "/grault/corge/quux", NULL);
    rtree_insert_path(n, "/grault/corge/garply", NULL);
    rtree_insert_path(n, "/grault/garply/foo", NULL);
    rtree_insert_path(n, "/grault/garply/bar", NULL);
    rtree_insert_path(n, "/grault/garply/baz", NULL);
    rtree_insert_path(n, "/grault/garply/qux", NULL);
    rtree_insert_path(n, "/grault/garply/quux", NULL);
    rtree_insert_path(n, "/grault/garply/corge", NULL);
    rtree_insert_path(n, "/garply/foo/bar", NULL);
    rtree_insert_path(n, "/garply/foo/baz", NULL);
    rtree_insert_path(n, "/garply/foo/qux", NULL);
    rtree_insert_path(n, "/garply/foo/quux", NULL);
    rtree_insert_path(n, "/garply/foo/corge", NULL);
    rtree_insert_path(n, "/garply/foo/grault", NULL);
    rtree_insert_path(n, "/garply/bar/foo", NULL);
    rtree_insert_path(n, "/garply/bar/baz", NULL);
    rtree_insert_path(n, "/garply/bar/qux", NULL);
    rtree_insert_path(n, "/garply/bar/quux", NULL);
    rtree_insert_path(n, "/garply/bar/corge", NULL);
    rtree_insert_path(n, "/garply/bar/grault", NULL);
    rtree_insert_path(n, "/garply/baz/foo", NULL);
    rtree_insert_path(n, "/garply/baz/bar", NULL);
    rtree_insert_path(n, "/garply/baz/qux", NULL);
    rtree_insert_path(n, "/garply/baz/quux", NULL);
    rtree_insert_path(n, "/garply/baz/corge", NULL);
    rtree_insert_path(n, "/garply/baz/grault", NULL);
    rtree_insert_path(n, "/garply/qux/foo", NULL);
    rtree_insert_path(n, "/garply/qux/bar", NULL);
    rtree_insert_path(n, "/garply/qux/baz", NULL);
    rtree_insert_path(n, "/garply/qux/quux", NULL);
    rtree_insert_path(n, "/garply/qux/corge", NULL);
    rtree_insert_path(n, "/garply/qux/grault", NULL);
    rtree_insert_path(n, "/garply/quux/foo", NULL);
    rtree_insert_path(n, "/garply/quux/bar", NULL);
    rtree_insert_path(n, "/garply/quux/baz", NULL);
    rtree_insert_path(n, "/garply/quux/qux", NULL);
    rtree_insert_path(n, "/garply/quux/corge", NULL);
    rtree_insert_path(n, "/garply/quux/grault", NULL);
    rtree_insert_path(n, "/garply/corge/foo", NULL);
    rtree_insert_path(n, "/garply/corge/bar", NULL);
    rtree_insert_path(n, "/garply/corge/baz", NULL);
    rtree_insert_path(n, "/garply/corge/qux", NULL);
    rtree_insert_path(n, "/garply/corge/quux", NULL);
    rtree_insert_path(n, "/garply/corge/grault", NULL);
    rtree_insert_path(n, "/garply/grault/foo", NULL);
    rtree_insert_path(n, "/garply/grault/bar", NULL);
    rtree_insert_path(n, "/garply/grault/baz", NULL);
    rtree_insert_path(n, "/garply/grault/qux", NULL);
    rtree_insert_path(n, "/garply/grault/quux", NULL);
    rtree_insert_path(n, "/garply/grault/corge", NULL);

    rtree_compile(n);
    // rtree_dump(n, 0);
    // match_entry *entry = calloc( sizeof(entry) , 1 );

    node *m;
    m = rtree_match(n , "/qux/bar/corge", strlen("/qux/bar/corge"), NULL);
    fail_if( m == NULL );
    // rtree_dump( m, 0 );
    ck_assert_int_eq( (int) m->route_ptr, 999 );


    printf("Benchmarking...\n");
    double s = microtime();
    unsigned int N = 5000000;
    for (int i = 0; i < 5000000 ; i++ ) {
        rtree_match(n , "/qux/bar/corge", strlen("/qux/bar/corge"), NULL);
    }
    double e = microtime();

    printf("%.2f i/sec\n", e - s / N );
    printf("%lf seconds\n", e - s );

    FILE *fp = fopen("bench_str.csv", "a+");
    fprintf(fp, "%.2f,\"%s\"\n", e - s / N, "using strcmp" );
    fclose(fp);

}
END_TEST

Suite* r3_suite (void) {
        Suite *suite = suite_create("blah");

        TCase *tcase = tcase_create("testcase");
        tcase_add_test(tcase, test_route_split);
        tcase_add_test(tcase, test_str_array);
        tcase_add_test(tcase, test_ltrim_slash);
        tcase_add_test(tcase, test_node_construct_uniq);
        tcase_add_test(tcase, test_node_find_edge);
        tcase_add_test(tcase, test_rtree_insert_pathn);
        tcase_add_test(tcase, test_compile_slug);
        tcase_add_test(tcase, test_compile);

        tcase_add_test(tcase, benchmark_str);

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
