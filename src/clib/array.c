/*
 * array
 * 
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * dynamic array in C
 */
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "clib/array.h"

array* array_new(size_t element_size)
{
    array* arr = (array*)malloc(sizeof(array));
    array_init(arr, element_size);
    return arr;
}

void array_init(array *arr, size_t element_size)
{
    arr->_element_size = element_size;
    size_t init_size = 7;
    arr->data = (unsigned char*)malloc(init_size * element_size);
    arr->cap = init_size;
    arr->size = 0;
}

void array_append(array* a, void* element)
{
    if (a->size == a->cap) {
        a->cap *= 2;
        a->data = realloc(a->data, a->cap * a->_element_size);
    }
    memcpy(a->data + a->size * a->_element_size, (unsigned char*)element, a->_element_size);
    a->size ++;
}

void* array_get(array *arr, size_t index)
{
    return (void*)(arr->data + index * arr->_element_size);
}

void array_deinit(array *arr)
{
    free(arr->data);
}

void array_free(array* arr)
{
    array_deinit(arr);
    free(arr);
}