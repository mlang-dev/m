/*
 * array
 * 
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * dynamic array in C
 */
#include <stddef.h>

#include "array.h"

Array* array_init()
{
    size_t init_size = 7;
    Array* a = (Array*)malloc(sizeof(Array));
    a->data = malloc(init_size * sizeof(void*));
    a->used = 0;
    a->size = init_size;
    return a;
}

void array_append(Array* a, void* element)
{
    if (a->used == a->size) {
        a->size *= 2;
        a->data = (int*)realloc(a->data, a->size * sizeof(void*));
    }
    a->data[a->used++] = element;
}

void array_free(Array* a)
{
    free(a->data);
    a->data = NULL;
    a->used = a->size = 0;
    free(a);
}