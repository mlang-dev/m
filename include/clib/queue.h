/*
 * array.h
 * 
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * dynamic array c header file
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
    size_t front;
    size_t back;
} queue;

void queue_init(queue *q, size_t element_size);
void queue_deinit(queue *q);
void queue_push(queue *q, void* element);
void queue_pop(queue *q);
void* queue_front(queue *q);
void* queue_back(queue *q);


#ifdef __cplusplus
}
#endif

#endif