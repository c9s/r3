/*
 * r3_workers.h
 * Copyright (C) 2014 c9s <c9s@c9smba.local>
 *
 * Distributed under terms of the MIT license.
 */

#ifndef R3_WORKERS_H
#define R3_WORKERS_H

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

typedef struct {
    int     thread_id;
    node  * matched_node;
} feedback_payload;

void r3_feedback_worker_init(pthread_t * t, feedback_payload * data);

void r3_worker_cancel(pthread_t t);

void r3_worker_exit();

#endif /* !R3_WORKERS_H */
