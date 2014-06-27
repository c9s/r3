/*
 * r3_list.h
 * Copyright (C) 2014 c9s <yoanlin93@gmail.com>
 *
 * Distributed under terms of the MIT license.
 */

#ifndef R3_LIST_H
#define R3_LIST_H

#include <pthread.h>
 
typedef struct _list_item {
  void *value;
  struct _list_item *prev;
  struct _list_item *next;
} list_item;
 
typedef struct {
  int count;
  list_item *head;
  list_item *tail;
  pthread_mutex_t mutex;
} list;
 
list *list_create();
void list_free(list *l);
 
list_item *list_add_element(list *l, void *ptr);
int list_remove_element(list *l, void *ptr);
void list_each_element(list *l, int (*func)(list_item *));
 


#endif /* !R3_LIST_H */
