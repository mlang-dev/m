/*
 * array
 * 
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * dynamic array in C
 */
#include <stddef.h>
#include <stdlib.h>

#include "array.h"

array* array_init()
{
    size_t init_size = 7;
    array* arr = (array*)malloc(sizeof(array));
    arr->data = malloc(init_size * sizeof(void*));
    arr->used_size = 0;
    arr->total_size = init_size;
    return arr;
}

void array_append(array* a, void* element)
{
    if (a->used_size == a->total_size) {
        a->total_size *= 2;
        a->data = realloc(a->data, a->total_size * sizeof(void*));
    }
    a->data[a->used_size++] = element;
}

void array_free(array* a)
{
    free(a->data);
    a->data = NULL;
    a->used_size = a->total_size = 0;
    free(a);
}