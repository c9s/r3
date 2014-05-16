/*
 * bench.c
 * Copyright (C) 2014 c9s <c9s@c9smba.local>
 *
 * Distributed under terms of the MIT license.
 */

#include "bench.h"
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>

long unixtime() {
    struct timeval tp;
    long sec = 0L;
    if (gettimeofday((struct timeval *) &tp, (NUL)) == 0) {
        return tp.tv_sec;
    }
    return 0;
}

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


void bench_start(bench *b) {
    b->start = microtime();
}

void bench_stop(bench *b) {
    b->end = microtime();
}

double bench_iteration_speed(bench *b) {
    return b->N / (b->end - b->start);
}

void bench_summary(bench *b) {
    printf("%ld runs, ", b->R);
    printf("%ld iterations, ", b->N);
    printf("finished in %lf seconds\n", b->end - b->start );
    printf("%.2f i/sec\n", b->N / (b->end - b->start) );
}
