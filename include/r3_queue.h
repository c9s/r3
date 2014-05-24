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

struct _queue_node {
	void *data;
	struct _queue_node * next;
};
typedef struct _queue_node queue_node;
  
typedef struct {
	queue_node * first;
	queue_node * last;
} queue;

// create and return the queue
queue * queue_factory(void);

// destory the queue (free all the memory associate with the que even the data)
void queue_destroy(queue * que);


// enque the data into queue
// data is expected to a pointer to a heap allocated memory
int enque(queue * que, void * data);

// return the data from the que (FIFO)
// and free up all the internally allocated memory
// but the user have to free the returning data pointer
void * deque(queue * que);

#endif /* !R3_QUEUE_H */
