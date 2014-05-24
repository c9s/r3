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

void *r3_feedback_worker(void * data) {
    feedback_payload * payload = (feedback_payload*) data; // pointer cast

   pthread_exit(NULL);
}

void r3_feedback_worker_init(pthread_t * t, feedback_payload * data) {
    int rc;
    // pthread_t worker_thread;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    rc = pthread_create(t, &attr, r3_feedback_worker, (void*) data);

    // if the pthread is created, we may free the attr
    pthread_attr_destroy(&attr);

    // rc = pthread_join(t, &status);
}

void r3_worker_cancel(pthread_t t) {
    pthread_cancel(t);
}

void r3_worker_exit() {
   pthread_exit(NULL);
}

