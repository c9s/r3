/*
 * bench.c
 * Copyright (C) 2014 c9s <yoanlin93@gmail.com>
 *
 * Distributed under terms of the MIT license.
 */
#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/time.h>
#include <stdarg.h>     /* va_list, va_start, va_arg, va_end */

#include "r3.h"
#include "r3_str.h"
#include "zmalloc.h"
#include "bench.h"



unsigned long unixtime() {
    struct timeval tp;
    if (gettimeofday((struct timeval *) &tp, (NUL)) == 0) {
        return tp.tv_sec;
    }
    return 0;
}

double microtime() {
    struct timeval tp;
    long sec = 0L;
    double msec = 0.0;
    if (gettimeofday((struct timeval *) &tp, (NUL)) == 0) {
        msec = (double) (tp.tv_usec / MICRO_IN_SEC);
        sec = tp.tv_sec;
        if (msec >= 1.0)
            msec -= (long) msec;
        return sec + msec;
    }
    return 0;
}

void bench_start(bench *b) {
    b->start = microtime();
}

void bench_stop(bench *b) {
    b->end = microtime();
}

double bench_iteration_speed(bench *b) {
    return (b->N * b->R) / (b->end - b->start);
}

double bench_duration(bench *b) {
    return (b->end - b->start);
}

void bench_print_summary(bench *b) {
    printf("%ld runs, ", b->R);
    printf("%ld iterations each run, ", b->N);
    printf("finished in %lf seconds\n", bench_duration(b) );
    printf("%.2f i/sec\n", bench_iteration_speed(b) );
}

/**
 * Combine multiple benchmark result into one measure entry.
 *
 * bench_append_csv("benchmark.csv", 3, &b1, &b2)
 */
void bench_append_csv(char *filename, int countOfB, ...) {
    FILE *fp = fopen(filename, "a+");
    if(!fp) {
        return;
    }

    unsigned long ts = unixtime();
    fprintf(fp, "%ld", ts);


    int i;
    bench * b;
    va_list vl;
    va_start(vl,countOfB);
    for (i=0 ; i < countOfB ; i++) {
        b = va_arg(vl, bench*);
        fprintf(fp, ",%.2f", bench_iteration_speed(b) );
    }
    va_end(vl);

    fprintf(fp, "\n");
    fclose(fp);
}



int main()
{
    node * n = r3_tree_create(1);

    int route_data = 999;

r3_tree_insert_path(n, "/foo/bar/baz",  NULL);
r3_tree_insert_path(n, "/foo/bar/qux",  NULL);
r3_tree_insert_path(n, "/foo/bar/quux",  NULL);
r3_tree_insert_path(n, "/foo/bar/corge",  NULL);
r3_tree_insert_path(n, "/foo/bar/grault",  NULL);
r3_tree_insert_path(n, "/foo/bar/garply",  NULL);
r3_tree_insert_path(n, "/foo/baz/bar",  NULL);
r3_tree_insert_path(n, "/foo/baz/qux",  NULL);
r3_tree_insert_path(n, "/foo/baz/quux",  NULL);
r3_tree_insert_path(n, "/foo/baz/corge",  NULL);
r3_tree_insert_path(n, "/foo/baz/grault",  NULL);
r3_tree_insert_path(n, "/foo/baz/garply",  NULL);
r3_tree_insert_path(n, "/foo/qux/bar",  NULL);
r3_tree_insert_path(n, "/foo/qux/baz",  NULL);
r3_tree_insert_path(n, "/foo/qux/quux",  NULL);
r3_tree_insert_path(n, "/foo/qux/corge",  NULL);
r3_tree_insert_path(n, "/foo/qux/grault",  NULL);
r3_tree_insert_path(n, "/foo/qux/garply",  NULL);
r3_tree_insert_path(n, "/foo/quux/bar",  NULL);
r3_tree_insert_path(n, "/foo/quux/baz",  NULL);
r3_tree_insert_path(n, "/foo/quux/qux",  NULL);
r3_tree_insert_path(n, "/foo/quux/corge",  NULL);
r3_tree_insert_path(n, "/foo/quux/grault",  NULL);
r3_tree_insert_path(n, "/foo/quux/garply",  NULL);
r3_tree_insert_path(n, "/foo/corge/bar",  NULL);
r3_tree_insert_path(n, "/foo/corge/baz",  NULL);
r3_tree_insert_path(n, "/foo/corge/qux",  NULL);
r3_tree_insert_path(n, "/foo/corge/quux",  NULL);
r3_tree_insert_path(n, "/foo/corge/grault",  NULL);
r3_tree_insert_path(n, "/foo/corge/garply",  NULL);
r3_tree_insert_path(n, "/foo/grault/bar",  NULL);
r3_tree_insert_path(n, "/foo/grault/baz",  NULL);
r3_tree_insert_path(n, "/foo/grault/qux",  NULL);
r3_tree_insert_path(n, "/foo/grault/quux",  NULL);
r3_tree_insert_path(n, "/foo/grault/corge",  NULL);
r3_tree_insert_path(n, "/foo/grault/garply",  NULL);
r3_tree_insert_path(n, "/foo/garply/bar",  NULL);
r3_tree_insert_path(n, "/foo/garply/baz",  NULL);
r3_tree_insert_path(n, "/foo/garply/qux",  NULL);
r3_tree_insert_path(n, "/foo/garply/quux",  NULL);
r3_tree_insert_path(n, "/foo/garply/corge",  NULL);
r3_tree_insert_path(n, "/foo/garply/grault",  NULL);
r3_tree_insert_path(n, "/bar/foo/baz",  NULL);
r3_tree_insert_path(n, "/bar/foo/qux",  NULL);
r3_tree_insert_path(n, "/bar/foo/quux",  NULL);
r3_tree_insert_path(n, "/bar/foo/corge",  NULL);
r3_tree_insert_path(n, "/bar/foo/grault",  NULL);
r3_tree_insert_path(n, "/bar/foo/garply",  NULL);
r3_tree_insert_path(n, "/bar/baz/foo",  NULL);
r3_tree_insert_path(n, "/bar/baz/qux",  NULL);
r3_tree_insert_path(n, "/bar/baz/quux",  NULL);
r3_tree_insert_path(n, "/bar/baz/corge",  NULL);
r3_tree_insert_path(n, "/bar/baz/grault",  NULL);
r3_tree_insert_path(n, "/bar/baz/garply",  NULL);
r3_tree_insert_path(n, "/bar/qux/foo",  NULL);
r3_tree_insert_path(n, "/bar/qux/baz",  NULL);
r3_tree_insert_path(n, "/bar/qux/quux",  NULL);
r3_tree_insert_path(n, "/bar/qux/corge",  NULL);
r3_tree_insert_path(n, "/bar/qux/grault",  NULL);
r3_tree_insert_path(n, "/bar/qux/garply",  NULL);
r3_tree_insert_path(n, "/bar/quux/foo",  NULL);
r3_tree_insert_path(n, "/bar/quux/baz",  NULL);
r3_tree_insert_path(n, "/bar/quux/qux",  NULL);
r3_tree_insert_path(n, "/bar/quux/corge",  NULL);
r3_tree_insert_path(n, "/bar/quux/grault",  NULL);
r3_tree_insert_path(n, "/bar/quux/garply",  NULL);
r3_tree_insert_path(n, "/bar/corge/foo",  NULL);
r3_tree_insert_path(n, "/bar/corge/baz",  NULL);
r3_tree_insert_path(n, "/bar/corge/qux",  NULL);
r3_tree_insert_path(n, "/bar/corge/quux",  NULL);
r3_tree_insert_path(n, "/bar/corge/grault",  NULL);
r3_tree_insert_path(n, "/bar/corge/garply",  NULL);
r3_tree_insert_path(n, "/bar/grault/foo",  NULL);
r3_tree_insert_path(n, "/bar/grault/baz",  NULL);
r3_tree_insert_path(n, "/bar/grault/qux",  NULL);
r3_tree_insert_path(n, "/bar/grault/quux",  NULL);
r3_tree_insert_path(n, "/bar/grault/corge",  NULL);
r3_tree_insert_path(n, "/bar/grault/garply",  NULL);
r3_tree_insert_path(n, "/bar/garply/foo",  NULL);
r3_tree_insert_path(n, "/bar/garply/baz",  NULL);
r3_tree_insert_path(n, "/bar/garply/qux",  NULL);
r3_tree_insert_path(n, "/bar/garply/quux",  NULL);
r3_tree_insert_path(n, "/bar/garply/corge",  NULL);
r3_tree_insert_path(n, "/bar/garply/grault",  NULL);
r3_tree_insert_path(n, "/baz/foo/bar",  NULL);
r3_tree_insert_path(n, "/baz/foo/qux",  NULL);
r3_tree_insert_path(n, "/baz/foo/quux",  NULL);
r3_tree_insert_path(n, "/baz/foo/corge",  NULL);
r3_tree_insert_path(n, "/baz/foo/grault",  NULL);
r3_tree_insert_path(n, "/baz/foo/garply",  NULL);
r3_tree_insert_path(n, "/baz/bar/foo",  NULL);
r3_tree_insert_path(n, "/baz/bar/qux",  NULL);
r3_tree_insert_path(n, "/baz/bar/quux",  NULL);
r3_tree_insert_path(n, "/baz/bar/corge",  NULL);
r3_tree_insert_path(n, "/baz/bar/grault",  NULL);
r3_tree_insert_path(n, "/baz/bar/garply",  NULL);
r3_tree_insert_path(n, "/baz/qux/foo",  NULL);
r3_tree_insert_path(n, "/baz/qux/bar",  NULL);
r3_tree_insert_path(n, "/baz/qux/quux",  NULL);
r3_tree_insert_path(n, "/baz/qux/corge",  NULL);
r3_tree_insert_path(n, "/baz/qux/grault",  NULL);
r3_tree_insert_path(n, "/baz/qux/garply",  NULL);
r3_tree_insert_path(n, "/baz/quux/foo",  NULL);
r3_tree_insert_path(n, "/baz/quux/bar",  NULL);
r3_tree_insert_path(n, "/baz/quux/qux",  NULL);
r3_tree_insert_path(n, "/baz/quux/corge",  NULL);
r3_tree_insert_path(n, "/baz/quux/grault",  NULL);
r3_tree_insert_path(n, "/baz/quux/garply",  NULL);
r3_tree_insert_path(n, "/baz/corge/foo",  NULL);
r3_tree_insert_path(n, "/baz/corge/bar",  NULL);
r3_tree_insert_path(n, "/baz/corge/qux",  NULL);
r3_tree_insert_path(n, "/baz/corge/quux",  NULL);
r3_tree_insert_path(n, "/baz/corge/grault",  NULL);
r3_tree_insert_path(n, "/baz/corge/garply",  NULL);
r3_tree_insert_path(n, "/baz/grault/foo",  NULL);
r3_tree_insert_path(n, "/baz/grault/bar",  NULL);
r3_tree_insert_path(n, "/baz/grault/qux",  NULL);
r3_tree_insert_path(n, "/baz/grault/quux",  NULL);
r3_tree_insert_path(n, "/baz/grault/corge",  NULL);
r3_tree_insert_path(n, "/baz/grault/garply",  NULL);
r3_tree_insert_path(n, "/baz/garply/foo",  NULL);
r3_tree_insert_path(n, "/baz/garply/bar",  NULL);
r3_tree_insert_path(n, "/baz/garply/qux",  NULL);
r3_tree_insert_path(n, "/baz/garply/quux",  NULL);
r3_tree_insert_path(n, "/baz/garply/corge",  NULL);
r3_tree_insert_path(n, "/baz/garply/grault",  NULL);
r3_tree_insert_path(n, "/qux/foo/bar",  NULL);
r3_tree_insert_path(n, "/qux/foo/baz",  NULL);
r3_tree_insert_path(n, "/qux/foo/quux",  NULL);
r3_tree_insert_path(n, "/qux/foo/corge",  NULL);
r3_tree_insert_path(n, "/qux/foo/grault",  NULL);
r3_tree_insert_path(n, "/qux/foo/garply",  NULL);
r3_tree_insert_path(n, "/qux/bar/foo",  NULL);
r3_tree_insert_path(n, "/qux/bar/baz",  NULL);
r3_tree_insert_path(n, "/qux/bar/quux",  NULL);
r3_tree_insert_path(n, "/qux/bar/corge",  &route_data);
r3_tree_insert_path(n, "/qux/bar/grault",  NULL);
r3_tree_insert_path(n, "/qux/bar/garply",  NULL);
r3_tree_insert_path(n, "/qux/baz/foo",  NULL);
r3_tree_insert_path(n, "/qux/baz/bar",  NULL);
r3_tree_insert_path(n, "/qux/baz/quux",  NULL);
r3_tree_insert_path(n, "/qux/baz/corge",  NULL);
r3_tree_insert_path(n, "/qux/baz/grault",  NULL);
r3_tree_insert_path(n, "/qux/baz/garply",  NULL);
r3_tree_insert_path(n, "/qux/quux/foo",  NULL);
r3_tree_insert_path(n, "/qux/quux/bar",  NULL);
r3_tree_insert_path(n, "/qux/quux/baz",  NULL);
r3_tree_insert_path(n, "/qux/quux/corge",  NULL);
r3_tree_insert_path(n, "/qux/quux/grault",  NULL);
r3_tree_insert_path(n, "/qux/quux/garply",  NULL);
r3_tree_insert_path(n, "/qux/corge/foo",  NULL);
r3_tree_insert_path(n, "/qux/corge/bar",  NULL);
r3_tree_insert_path(n, "/qux/corge/baz",  NULL);
r3_tree_insert_path(n, "/qux/corge/quux",  NULL);
r3_tree_insert_path(n, "/qux/corge/grault",  NULL);
r3_tree_insert_path(n, "/qux/corge/garply",  NULL);
r3_tree_insert_path(n, "/qux/grault/foo",  NULL);
r3_tree_insert_path(n, "/qux/grault/bar",  NULL);
r3_tree_insert_path(n, "/qux/grault/baz",  NULL);
r3_tree_insert_path(n, "/qux/grault/quux",  NULL);
r3_tree_insert_path(n, "/qux/grault/corge",  NULL);
r3_tree_insert_path(n, "/qux/grault/garply",  NULL);
r3_tree_insert_path(n, "/qux/garply/foo",  NULL);
r3_tree_insert_path(n, "/qux/garply/bar",  NULL);
r3_tree_insert_path(n, "/qux/garply/baz",  NULL);
r3_tree_insert_path(n, "/qux/garply/quux",  NULL);
r3_tree_insert_path(n, "/qux/garply/corge",  NULL);
r3_tree_insert_path(n, "/qux/garply/grault",  NULL);
r3_tree_insert_path(n, "/quux/foo/bar",  NULL);
r3_tree_insert_path(n, "/quux/foo/baz",  NULL);
r3_tree_insert_path(n, "/quux/foo/qux",  NULL);
r3_tree_insert_path(n, "/quux/foo/corge",  NULL);
r3_tree_insert_path(n, "/quux/foo/grault",  NULL);
r3_tree_insert_path(n, "/quux/foo/garply",  NULL);
r3_tree_insert_path(n, "/quux/bar/foo",  NULL);
r3_tree_insert_path(n, "/quux/bar/baz",  NULL);
r3_tree_insert_path(n, "/quux/bar/qux",  NULL);
r3_tree_insert_path(n, "/quux/bar/corge",  NULL);
r3_tree_insert_path(n, "/quux/bar/grault",  NULL);
r3_tree_insert_path(n, "/quux/bar/garply",  NULL);
r3_tree_insert_path(n, "/quux/baz/foo",  NULL);
r3_tree_insert_path(n, "/quux/baz/bar",  NULL);
r3_tree_insert_path(n, "/quux/baz/qux",  NULL);
r3_tree_insert_path(n, "/quux/baz/corge",  NULL);
r3_tree_insert_path(n, "/quux/baz/grault",  NULL);
r3_tree_insert_path(n, "/quux/baz/garply",  NULL);
r3_tree_insert_path(n, "/quux/qux/foo",  NULL);
r3_tree_insert_path(n, "/quux/qux/bar",  NULL);
r3_tree_insert_path(n, "/quux/qux/baz",  NULL);
r3_tree_insert_path(n, "/quux/qux/corge",  NULL);
r3_tree_insert_path(n, "/quux/qux/grault",  NULL);
r3_tree_insert_path(n, "/quux/qux/garply",  NULL);
r3_tree_insert_path(n, "/quux/corge/foo",  NULL);
r3_tree_insert_path(n, "/quux/corge/bar",  NULL);
r3_tree_insert_path(n, "/quux/corge/baz",  NULL);
r3_tree_insert_path(n, "/quux/corge/qux",  NULL);
r3_tree_insert_path(n, "/quux/corge/grault",  NULL);
r3_tree_insert_path(n, "/quux/corge/garply",  NULL);
r3_tree_insert_path(n, "/quux/grault/foo",  NULL);
r3_tree_insert_path(n, "/quux/grault/bar",  NULL);
r3_tree_insert_path(n, "/quux/grault/baz",  NULL);
r3_tree_insert_path(n, "/quux/grault/qux",  NULL);
r3_tree_insert_path(n, "/quux/grault/corge",  NULL);
r3_tree_insert_path(n, "/quux/grault/garply",  NULL);
r3_tree_insert_path(n, "/quux/garply/foo",  NULL);
r3_tree_insert_path(n, "/quux/garply/bar",  NULL);
r3_tree_insert_path(n, "/quux/garply/baz",  NULL);
r3_tree_insert_path(n, "/quux/garply/qux",  NULL);
r3_tree_insert_path(n, "/quux/garply/corge",  NULL);
r3_tree_insert_path(n, "/quux/garply/grault",  NULL);
r3_tree_insert_path(n, "/corge/foo/bar",  NULL);
r3_tree_insert_path(n, "/corge/foo/baz",  NULL);
r3_tree_insert_path(n, "/corge/foo/qux",  NULL);
r3_tree_insert_path(n, "/corge/foo/quux",  NULL);
r3_tree_insert_path(n, "/corge/foo/grault",  NULL);
r3_tree_insert_path(n, "/corge/foo/garply",  NULL);
r3_tree_insert_path(n, "/corge/bar/foo",  NULL);
r3_tree_insert_path(n, "/corge/bar/baz",  NULL);
r3_tree_insert_path(n, "/corge/bar/qux",  NULL);
r3_tree_insert_path(n, "/corge/bar/quux",  NULL);
r3_tree_insert_path(n, "/corge/bar/grault",  NULL);
r3_tree_insert_path(n, "/corge/bar/garply",  NULL);
r3_tree_insert_path(n, "/corge/baz/foo",  NULL);
r3_tree_insert_path(n, "/corge/baz/bar",  NULL);
r3_tree_insert_path(n, "/corge/baz/qux",  NULL);
r3_tree_insert_path(n, "/corge/baz/quux",  NULL);
r3_tree_insert_path(n, "/corge/baz/grault",  NULL);
r3_tree_insert_path(n, "/corge/baz/garply",  NULL);
r3_tree_insert_path(n, "/corge/qux/foo",  NULL);
r3_tree_insert_path(n, "/corge/qux/bar",  NULL);
r3_tree_insert_path(n, "/corge/qux/baz",  NULL);
r3_tree_insert_path(n, "/corge/qux/quux",  NULL);
r3_tree_insert_path(n, "/corge/qux/grault",  NULL);
r3_tree_insert_path(n, "/corge/qux/garply",  NULL);
r3_tree_insert_path(n, "/corge/quux/foo",  NULL);
r3_tree_insert_path(n, "/corge/quux/bar",  NULL);
r3_tree_insert_path(n, "/corge/quux/baz",  NULL);
r3_tree_insert_path(n, "/corge/quux/qux",  NULL);
r3_tree_insert_path(n, "/corge/quux/grault",  NULL);
r3_tree_insert_path(n, "/corge/quux/garply",  NULL);
r3_tree_insert_path(n, "/corge/grault/foo",  NULL);
r3_tree_insert_path(n, "/corge/grault/bar",  NULL);
r3_tree_insert_path(n, "/corge/grault/baz",  NULL);
r3_tree_insert_path(n, "/corge/grault/qux",  NULL);
r3_tree_insert_path(n, "/corge/grault/quux",  NULL);
r3_tree_insert_path(n, "/corge/grault/garply",  NULL);
r3_tree_insert_path(n, "/corge/garply/foo",  NULL);
r3_tree_insert_path(n, "/corge/garply/bar",  NULL);
r3_tree_insert_path(n, "/corge/garply/baz",  NULL);
r3_tree_insert_path(n, "/corge/garply/qux",  NULL);
r3_tree_insert_path(n, "/corge/garply/quux",  NULL);
r3_tree_insert_path(n, "/corge/garply/grault",  NULL);
r3_tree_insert_path(n, "/grault/foo/bar",  NULL);
r3_tree_insert_path(n, "/grault/foo/baz",  NULL);
r3_tree_insert_path(n, "/grault/foo/qux",  NULL);
r3_tree_insert_path(n, "/grault/foo/quux",  NULL);
r3_tree_insert_path(n, "/grault/foo/corge",  NULL);
r3_tree_insert_path(n, "/grault/foo/garply",  NULL);
r3_tree_insert_path(n, "/grault/bar/foo",  NULL);
r3_tree_insert_path(n, "/grault/bar/baz",  NULL);
r3_tree_insert_path(n, "/grault/bar/qux",  NULL);
r3_tree_insert_path(n, "/grault/bar/quux",  NULL);
r3_tree_insert_path(n, "/grault/bar/corge",  NULL);
r3_tree_insert_path(n, "/grault/bar/garply",  NULL);
r3_tree_insert_path(n, "/grault/baz/foo",  NULL);
r3_tree_insert_path(n, "/grault/baz/bar",  NULL);
r3_tree_insert_path(n, "/grault/baz/qux",  NULL);
r3_tree_insert_path(n, "/grault/baz/quux",  NULL);
r3_tree_insert_path(n, "/grault/baz/corge",  NULL);
r3_tree_insert_path(n, "/grault/baz/garply",  NULL);
r3_tree_insert_path(n, "/grault/qux/foo",  NULL);
r3_tree_insert_path(n, "/grault/qux/bar",  NULL);
r3_tree_insert_path(n, "/grault/qux/baz",  NULL);
r3_tree_insert_path(n, "/grault/qux/quux",  NULL);
r3_tree_insert_path(n, "/grault/qux/corge",  NULL);
r3_tree_insert_path(n, "/grault/qux/garply",  NULL);
r3_tree_insert_path(n, "/grault/quux/foo",  NULL);
r3_tree_insert_path(n, "/grault/quux/bar",  NULL);
r3_tree_insert_path(n, "/grault/quux/baz",  NULL);
r3_tree_insert_path(n, "/grault/quux/qux",  NULL);
r3_tree_insert_path(n, "/grault/quux/corge",  NULL);
r3_tree_insert_path(n, "/grault/quux/garply",  NULL);
r3_tree_insert_path(n, "/grault/corge/foo",  NULL);
r3_tree_insert_path(n, "/grault/corge/bar",  NULL);
r3_tree_insert_path(n, "/grault/corge/baz",  NULL);
r3_tree_insert_path(n, "/grault/corge/qux",  NULL);
r3_tree_insert_path(n, "/grault/corge/quux",  NULL);
r3_tree_insert_path(n, "/grault/corge/garply",  NULL);
r3_tree_insert_path(n, "/grault/garply/foo",  NULL);
r3_tree_insert_path(n, "/grault/garply/bar",  NULL);
r3_tree_insert_path(n, "/grault/garply/baz",  NULL);
r3_tree_insert_path(n, "/grault/garply/qux",  NULL);
r3_tree_insert_path(n, "/grault/garply/quux",  NULL);
r3_tree_insert_path(n, "/grault/garply/corge",  NULL);
r3_tree_insert_path(n, "/garply/foo/bar",  NULL);
r3_tree_insert_path(n, "/garply/foo/baz",  NULL);
r3_tree_insert_path(n, "/garply/foo/qux",  NULL);
r3_tree_insert_path(n, "/garply/foo/quux",  NULL);
r3_tree_insert_path(n, "/garply/foo/corge",  NULL);
r3_tree_insert_path(n, "/garply/foo/grault",  NULL);
r3_tree_insert_path(n, "/garply/bar/foo",  NULL);
r3_tree_insert_path(n, "/garply/bar/baz",  NULL);
r3_tree_insert_path(n, "/garply/bar/qux",  NULL);
r3_tree_insert_path(n, "/garply/bar/quux",  NULL);
r3_tree_insert_path(n, "/garply/bar/corge",  NULL);
r3_tree_insert_path(n, "/garply/bar/grault",  NULL);
r3_tree_insert_path(n, "/garply/baz/foo",  NULL);
r3_tree_insert_path(n, "/garply/baz/bar",  NULL);
r3_tree_insert_path(n, "/garply/baz/qux",  NULL);
r3_tree_insert_path(n, "/garply/baz/quux",  NULL);
r3_tree_insert_path(n, "/garply/baz/corge",  NULL);
r3_tree_insert_path(n, "/garply/baz/grault",  NULL);
r3_tree_insert_path(n, "/garply/qux/foo",  NULL);
r3_tree_insert_path(n, "/garply/qux/bar",  NULL);
r3_tree_insert_path(n, "/garply/qux/baz",  NULL);
r3_tree_insert_path(n, "/garply/qux/quux",  NULL);
r3_tree_insert_path(n, "/garply/qux/corge",  NULL);
r3_tree_insert_path(n, "/garply/qux/grault",  NULL);
r3_tree_insert_path(n, "/garply/quux/foo",  NULL);
r3_tree_insert_path(n, "/garply/quux/bar",  NULL);
r3_tree_insert_path(n, "/garply/quux/baz",  NULL);
r3_tree_insert_path(n, "/garply/quux/qux",  NULL);
r3_tree_insert_path(n, "/garply/quux/corge",  NULL);
r3_tree_insert_path(n, "/garply/quux/grault",  NULL);
r3_tree_insert_path(n, "/garply/corge/foo",  NULL);
r3_tree_insert_path(n, "/garply/corge/bar",  NULL);
r3_tree_insert_path(n, "/garply/corge/baz",  NULL);
r3_tree_insert_path(n, "/garply/corge/qux",  NULL);
r3_tree_insert_path(n, "/garply/corge/quux",  NULL);
r3_tree_insert_path(n, "/garply/corge/grault",  NULL);
r3_tree_insert_path(n, "/garply/grault/foo",  NULL);
r3_tree_insert_path(n, "/garply/grault/bar",  NULL);
r3_tree_insert_path(n, "/garply/grault/baz",  NULL);
r3_tree_insert_path(n, "/garply/grault/qux",  NULL);
r3_tree_insert_path(n, "/garply/grault/quux",  NULL);
r3_tree_insert_path(n, "/garply/grault/corge",  NULL);

    MEASURE(tree_compile)
    r3_tree_compile(n, NULL);
    END_MEASURE(tree_compile)

    node *m;
    m = r3_tree_match(n , "/qux/bar/corge", NULL);
    assert(m != NULL);
    assert( *((int*) m->data) == 999 );



    BENCHMARK(str_dispatch)
    r3_tree_matchl(n , "/qux/bar/corge", strlen("/qux/bar/corge"), NULL);
    END_BENCHMARK(str_dispatch)
    BENCHMARK_SUMMARY(str_dispatch);

    BENCHMARK(str_match_entry)
    match_entry * e = match_entry_createl("/qux/bar/corge", strlen("/qux/bar/corge") );
    r3_tree_match_entry(n , e);
    match_entry_free(e);
    END_BENCHMARK(str_match_entry)
    BENCHMARK_SUMMARY(str_match_entry);


    node * tree2 = r3_tree_create(1);
    r3_tree_insert_path(tree2, "/post/{year}/{month}",  NULL);
    r3_tree_compile(tree2, NULL);

    BENCHMARK(pcre_dispatch)
    r3_tree_matchl(tree2, "/post/2014/12", strlen("/post/2014/12"), NULL);
    END_BENCHMARK(pcre_dispatch)
    BENCHMARK_SUMMARY(pcre_dispatch);


    BENCHMARK_RECORD_CSV("bench_str.csv", 4, BR(str_dispatch), BR(pcre_dispatch), BR(tree_compile), BR(str_match_entry) );

    r3_tree_free(tree2);
    r3_tree_free(n);
    return 0;
}
