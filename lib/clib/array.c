/*
 * struct array.c
 * 
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * dynamic struct array in C
 */
#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include "clib/array.h"
#include "clib/generic.h"
#include "clib/util.h"

struct array *array_new(size_t element_size)
{
    struct array *arr;
    MALLOC(arr, sizeof(*arr));
    array_init(arr, element_size);
    return arr;
}

void array_init_size(struct array *arr, size_t element_size, size_t init_size, free_fun free)
{
    void *p_data;
    CALLOC(p_data, init_size, element_size);
    arr->_element_size = element_size;
    arr->base.data.p_data = p_data;
    arr->cap = init_size;
    arr->base.size = 0;
    arr->free = free;
}

void array_init(struct array *arr, size_t element_size)
{
    array_init_free(arr, element_size, 0);
}

void array_init_free(struct array *arr, size_t element_size, free_fun free)
{
    array_init_size(arr, element_size, 7, free);
}

void array_copy(struct array *dest, struct array *src)
{
    array_copy_size(dest, src, src->base.size);
}

void array_copy_size(struct array *dest, struct array *src, size_t size)
{
    array_init_size(dest, src->_element_size, size, src->free);
    memcpy(dest->base.data.p_data, src->base.data.p_data, size * src->_element_size);
    dest->base.size = size;
}

void _copy_element_to_array(struct array *arr, size_t index, void *element)
{
    memcpy((unsigned char *)arr->base.data.p_data + (index * arr->_element_size), element, arr->_element_size);
}

void array_push(struct array *arr, void *element)
{
    if (arr->base.size == arr->cap) {
        array_grow(arr);
    }
    _copy_element_to_array(arr, arr->base.size, element);
    arr->base.size++;
}

void array_grow(struct array *arr)
{
    arr->cap *= 2;
    arr->base.data.p_data = realloc(arr->base.data.p_data, arr->cap * arr->_element_size);
}

void array_set(struct array *arr, size_t index, void *element)
{
    if (index > arr->cap - 1)
        return;
    _copy_element_to_array(arr, index, element);
}

void *array_get(struct array *arr, size_t index)
{
    return (unsigned char *)arr->base.data.p_data + (index * arr->_element_size);
}

void *array_data(struct array *arr)
{
    return arr->base.data.p_data;
}

void *array_back(struct array *arr)
{
    return array_get(arr, arr->base.size - 1);
}

void *array_front(struct array *arr)
{
    return array_get(arr, 0);
}

size_t array_size(struct array *arr)
{
    return arr->base.size;
}

void array_free(struct array *arr)
{
    array_deinit(arr);
    FREE(arr);
}

void array_deinit(struct array *arr)
{
    void *elem;
    for (size_t i = 0; i < arr->base.size; i++) {
        void *data = array_get(arr, i);
        if (sizeof(void*) == arr->_element_size) //This is just heuristic guess
            elem = *(void**)data;
        else
            elem = data;
        if (arr->free && elem)
            arr->free(elem);
    }
    FREE(arr->base.data.p_data);
}

void array_add(struct array *dest, struct array *src)
{
    for (size_t i = 0; i < array_size(src); i++) {
        array_push(dest, array_get(src, i));
    }
}

void array_clear(struct array *arr)
{
    array_deinit(arr);
    array_init_free(arr, arr->_element_size, arr->free);
}
