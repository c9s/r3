/*
 * workers.c
 * Copyright (C) 2014 c9s <c9s@c9smba.local>
 *
 * Distributed under terms of the MIT license.
 */
#include "r3.h"
#include "r3_workers.h"

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>


void *PrintHello(void *threadid)
{
   long tid;
   tid = (long)threadid;
   printf("Hello World! It's me, thread #%ld!\n", tid);
   pthread_exit(NULL);
}

typedef struct {
    int     thread_id;
    node  * matched_node;
} feedback_payload;

void *r3_feedback_worker(void * data) {
    feedback_payload * payload = (feedback_payload*) data; // pointer cast

    return NULL;
}

void r3_launch_feedback_worker(feedback_payload * data) {
    int rc;
    pthread_t worker_thread;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    rc = pthread_create(&worker_thread, &attr, r3_feedback_worker, (void*) data);
}


void r3_worker_stop() {
   pthread_exit(NULL);
}

