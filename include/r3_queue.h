/*
 * r3_queue.h
 * Copyright (C) 2014 c9s <c9s@c9smba.local>
 *
 * Distributed under terms of the MIT license.
 */

#ifndef R3_QUEUE_H
#define R3_QUEUE_H

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>

struct _queue;
struct _queue_node;
typedef struct _queue_node queue_node;
typedef struct _queue queue;

struct _queue_node {
	void *data;
	queue * next;
};
// typedef struct _queue_node queue_node;
  
struct _queue {
	queue_node * first;
	queue_node * last;
};

// create and return the queue
queue * queue_new(void);

// destory the queue (free all the memory associate with the que even the data)
void queue_free(queue * que);


// queue_push the data into queue
// data is expected to a pointer to a heap allocated memory
int queue_push(queue * que, void * data);

// return the data from the que (FIFO)
// and free up all the internally allocated memory
// but the user have to free the returning data pointer
void * queue_pop(queue * que);

#endif /* !R3_QUEUE_H */
