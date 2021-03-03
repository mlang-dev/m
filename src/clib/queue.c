/*
 * struct queue.c
 * 
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * dynamic struct queue in C
 */
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "clib/array.h"
#include "clib/queue.h"
#include "clib/string.h"

void queue_init(struct queue *q, size_t element_size)
{
    array_init(&q->items, element_size);
    q->front = q->back = 0;
}

void queue_push(struct queue *q, void *element)
{
    array_set(&q->items, q->back, element);
    q->back++;
}

size_t q_get_index(struct queue *q, size_t pos)
{
    return pos % q->items.cap;
}

void *queue_pop(struct queue *q)
{
    if (q->front < q->back) {
        void *element = array_get(&q->items, q_get_index(q, q->front));
        q->front++;
        return element;
    }
    return 0;
}

void *queue_front(struct queue *q)
{
    return q->back > q->front ? array_get(&q->items, q_get_index(q, q->front)) : 0;
}

void *queue_back(struct queue *q)
{
    return q->back > q->front ? array_get(&q->items, q_get_index(q, q->back - 1)) : 0;
}

void queue_deinit(struct queue *q)
{
    array_deinit(&q->items);
}

size_t queue_size(struct queue *q)
{
    return q->back - q->front;
}
