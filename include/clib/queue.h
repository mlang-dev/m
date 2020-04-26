/*
 * queue.h
 * 
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * dynamic queue c header file
 */
#ifndef __CLIB_QUEUE_H__
#define __CLIB_QUEUE_H__

#include <stddef.h>

#include "array.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    array items;
    size_t front; //pointing to the place where element is to be read from
    size_t back; //pointing to the place where element is to be put next
} queue;

void queue_init(queue *q, size_t element_size);
void queue_deinit(queue *q);
void queue_push(queue *q, void* element);
void* queue_pop(queue *q);
void* queue_front(queue *q);
void* queue_back(queue *q);
size_t queue_size(queue *q);


#ifdef __cplusplus
}
#endif

#endif
