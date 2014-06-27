/*
 * list.c Copyright (C) 2014 c9s <yoanlin93@gmail.com>
 * 
 * Distributed under terms of the MIT license.
 */
#include <stdlib.h>
#include "r3_list.h"
#include "zmalloc.h"

/* Naive linked list implementation */

list           *
list_create()
{
    list           *l = (list *) zmalloc(sizeof(list));
    l->count = 0;
    l->head = NULL;
    l->tail = NULL;
    pthread_mutex_init(&(l->mutex), NULL);
    return l;
}

void
list_free(l)
    list           *l;
{
    if (l) {
        list_item      *li, *tmp;

        pthread_mutex_lock(&(l->mutex));

        if (l != NULL) {
            li = l->head;
            while (li != NULL) {
                tmp = li->next;
                li = tmp;
            }
        }
        pthread_mutex_unlock(&(l->mutex));
        pthread_mutex_destroy(&(l->mutex));
        zfree(l);
    }
}

list_item * list_add_element(list * l, void * ptr) 
{
    list_item      *li;

    pthread_mutex_lock(&(l->mutex));

    li = (list_item *) zmalloc(sizeof(list_item));
    li->value = ptr;
    li->next = NULL;
    li->prev = l->tail;

    if (l->tail == NULL) {
        l->head = l->tail = li;
    } else {
        l->tail = li;
    }
    l->count++;

    pthread_mutex_unlock(&(l->mutex));

    return li;
}

int
list_remove_element(l, ptr)
    list           *l;
    void           *ptr;
{
    int     result = 0;
    list_item      *li = l->head;

    pthread_mutex_lock(&(l->mutex));

    while (li != NULL) {
        if (li->value == ptr) {
            if (li->prev == NULL) {
                l->head = li->next;
            } else {
                li->prev->next = li->next;
            }

            if (li->next == NULL) {
                l->tail = li->prev;
            } else {
                li->next->prev = li->prev;
            }
            l->count--;
            zfree(li);
            result = 1;
            break;
        }
        li = li->next;
    }

    pthread_mutex_unlock(&(l->mutex));

    return result;
}

void
list_each_element(l, func)
    list           *l;
    int             (*func) (list_item *);
{
    list_item      *li;

    pthread_mutex_lock(&(l->mutex));

    li = l->head;
    while (li != NULL) {
        if (func(li) == 1) {
            break;
        }
        li = li->next;
    }

    pthread_mutex_unlock(&(l->mutex));
}
