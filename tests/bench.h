/*
 * bench.h
 * Copyright (C) 2014 c9s <c9s@c9smba.local>
 *
 * Distributed under terms of the MIT license.
 */

#ifndef BENCH_H
#define BENCH_H

#include <stdlib.h>

#define MICRO_IN_SEC 1000000.00
#define SEC_IN_MIN 60
#define NUL  '\0'


typedef struct {
    long N; // N for each run
    long R; // runs
    double secs;
    double start;
    double end;
} bench;


#define BENCHMARK(name) \
    bench B; B.N = 5000000; B.R = 3; \
    bench_start(&B); \
    for (int _r = 0; _r < B.R ; _r++ ) { \
        for (int _i = 0; _i < B.N ; _i++ ) {

#define END_BENCHMARK() \
        } \
    } \
    bench_stop(&B);


long unixtime();

double microtime();

void bench_start(bench *b);

void bench_stop(bench *b);

double bench_iteration_speed(bench *b);

void bench_summary(bench *b);

#endif /* !BENCH_H */
