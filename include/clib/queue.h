/*
 * struct queue.h
 *
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * dynamic struct queue c header file
 */
#ifndef __CLIB_QUEUE_H__
#define __CLIB_QUEUE_H__

#include <stddef.h>

#include "array.h"

#ifdef __cplusplus
extern "C" {
#endif

struct queue {
    struct array items;
    size_t front; // pointing to the place where element is to be read from
    size_t back; // pointing to the place where element is to be put next
};

void queue_init(struct queue *q, size_t element_size);
void queue_deinit(struct queue *q);
void queue_push(struct queue *q, void *element);
void *queue_pop(struct queue *q);
void *queue_front(struct queue *q);
void *queue_back(struct queue *q);
size_t queue_size(struct queue *q);

#ifdef __cplusplus
}
#endif

#endif
