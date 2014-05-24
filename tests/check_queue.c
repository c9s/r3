/*
 * check_queue.c
 * Copyright (C) 2014 c9s <c9s@c9smba.local>
 *
 * Distributed under terms of the MIT license.
 */
#include "r3_queue.h"

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <check.h>

#include "r3.h"
#include "zmalloc.h"


#define number_of_threads	6 
#define number_of_threads_d 5
#define number_of_iters		1000
 
struct _stru {
    int number;
    int thread_no;
    queue * q;
};
 
void * producer_thread(void * arg)
{
    struct _stru * args = (struct _stru *) arg;
    int number = args->number;
    //int th_number = args->thread_no;
    queue * q = args->q;

    int i;
    srand(time(NULL));
    for(i = 0; i < number; i++) {
        char * message = zmalloc(16);
        snprintf(message, 15, "rand: %d", rand());
        queue_push(q, (void *)message);
    }

    return NULL;
}


void * consumer_thread(void * args)
{
    queue * q = (queue *) args;
    void * data;
    while((data = queue_pop(q)) != NULL) {
        char * string = (char *)data;
        zfree(data);
    }
    return NULL;
}

START_TEST (test_queue)
{
    queue * q = queue_new();
    queue_push(q, (void*) 1);
    int i = (int) queue_pop(q);
    ck_assert_int_eq(i, 1);
}
END_TEST

START_TEST (test_queue_threads)
{
    queue * q = queue_new();
    pthread_t threads[number_of_threads];
    pthread_t thread_d[number_of_threads_d];
    
    int i;
    struct _stru arg[number_of_threads];
    for(i = 0; i < number_of_threads; i++) {
        arg[i].number = number_of_iters;
        arg[i].thread_no = i;
        arg[i].q = q;
        pthread_create(threads+i, NULL, producer_thread, (void *)&arg[i]); 
    }

    for(i = 0; i < number_of_threads_d; i++) {
        pthread_create(thread_d+i, NULL, consumer_thread, (void *)q);
    }

    for(i = 0; i < number_of_threads; i++) {
        pthread_join(*(threads+i), NULL);
    }
    
    for(i = 0; i < number_of_threads_d; i++) {
        pthread_join(*(thread_d+i), NULL);   
    }
    queue_free(q);
}
END_TEST





Suite* r3_suite (void) {
    Suite *suite = suite_create("queue");
    TCase *tcase = tcase_create("queue_test");
    tcase_add_test(tcase, test_queue_threads);
    tcase_add_test(tcase, test_queue);
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
