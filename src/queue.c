/*
 * queue.c
 * Copyright (C) 2014 c9s <c9s@c9smba.local>
 *
 * Distributed under terms of the MIT license.
 */
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "r3_queue.h"
#include "zmalloc.h"


pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

/**
 * Create and return a new queue
 **/
queue * queue_new()
{
   queue * q = zmalloc(sizeof(queue));
   if(q == NULL) {
       fprintf(stderr, "Malloc failed creating the que\n");
       return NULL;
   }
   q->first = NULL;
   q->last = NULL;
   return q;
}

void queue_free(queue * que)
{
    if(que == NULL) {
        return;
    }

    pthread_mutex_lock(&mutex);
    if(que->first == NULL) {
        // ("que->first == NULL .... \n");
        zfree(que);
        pthread_mutex_unlock(&mutex);
        return;
    }

    // ("que is there lets try to free it...\n");

    queue_node * qn = que->first;

    while(qn != NULL) {
        // freeing the data coz it's on the heap and no one to free it
        // except for this one
        // ("freeing : %s\n", (char *)qn->data);
        zfree(qn->data);
        queue_node *tmp = qn->next;
        zfree(qn);
        qn = tmp;
    }

    zfree(que);

    pthread_mutex_unlock(&mutex);
}

/**
 * que is a queue pointer
 * data is a heap allocated memory pointer
 */
int queue_push(queue * que, void * data)
{
    queue_node * new_node = zmalloc(sizeof(queue_node));
    if (new_node == NULL) {
        fprintf(stderr, "Malloc failed creating a queue_node\n");
        return -1;
    }
    // assumming data is in heap
    new_node->data = data;
    new_node->next = NULL;

    pthread_mutex_lock(&mutex);
    if (que->first == NULL) {
        // new que
        que->first = new_node;
        que->last = new_node;
    } else {
        que->last->next = new_node;
        que->last = new_node;
    }
    pthread_mutex_unlock(&mutex);
    return 0;
}

void * queue_pop(queue * que) 
{
    // print("Entered to queue_pop\n");
    if (que == NULL) {
        // print("que is null exiting...\n");
        return NULL;
    }


    pthread_mutex_lock(&mutex);
    if (que->first == NULL) {
        pthread_mutex_unlock(&mutex);
        // print("que->first is null exiting...\n");
        return NULL;
    }

    void * data;
    queue_node * qn = que->first;
    if (que->first == que->last) {
        que->first = NULL;
        que->last = NULL;
    } else {
        que->first = qn->next;
    }

    data = qn->data;

    // print("Freeing qn@ %p", qn);
    zfree(qn);
    pthread_mutex_unlock(&mutex);
    // print("Exiting queue_pop\n");
    return data;
}



