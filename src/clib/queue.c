/*
 * queue.c
 * 
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * dynamic queue in C
 */
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "clib/array.h"
#include "clib/string.h"
#include "clib/queue.h"

void queue_init(queue *q, size_t element_size)
{
    array_init(&q->items, element_size);
    q->front = q->back = 0;
}

void queue_push(queue *q, void* element)
{
    array_set(&q->items, q->back, element);
    q->back++;
}

size_t q_get_index(queue *q, size_t pos)
{
    return pos % q->items.cap;
}

void* queue_pop(queue *q)
{
    if (q->front < q->back){
        void* element = array_get(&q->items, q_get_index(q, q->front));
        q->front++;
        return element;
    }
    return NULL;
}

void* queue_front(queue *q)
{
    return q->back > q->front ? array_get(&q->items, q_get_index(q, q->front)) : NULL;
}

void* queue_back(queue *q)
{
    return  q->back > q->front ? array_get(&q->items, q_get_index(q, q->back - 1)) : NULL;
}

void queue_deinit(queue *q)
{
    array_deinit(&q->items);
}

size_t queue_size(queue *q)
{
    return q->back - q->front;
}