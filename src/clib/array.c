/*
 * array.c
 * 
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * dynamic array in C
 */
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>

#include "clib/array.h"
#include "clib/string.h"
#include "clib/generic.h"


array* array_new(size_t element_size)
{
    array* arr = malloc(sizeof(*arr));
    array_init(arr, element_size);
    return arr;
}

void array_string_init(array *arr)
{
    array_init_fun(arr, sizeof(string), string_free_generic);
}

void array_init(array *arr, size_t element_size)
{
    array_init_fun(arr, element_size, 0);
}

void array_init_fun(array *arr, size_t element_size, free_fun free)
{
    array_init_size(arr, element_size, 7, free);
}

void array_init_size(array *arr, size_t element_size, size_t init_size, free_fun free)
{
    arr->_element_size = element_size;
    arr->base.p_data = malloc(init_size * element_size);
    memset(arr->base.p_data, 0, init_size * element_size);
    arr->cap = init_size;
    arr->base.size = 0;
    arr->free = free;
}

void array_copy(array *dest, array *src)
{
    array_copy_size(dest, src, src->base.size);
}

void array_copy_size(array *dest, array *src, size_t size)
{
    array_init_size(dest, src->_element_size, size, src->free);
    memcpy(dest->base.p_data, src->base.p_data, size * src->_element_size);
    dest->base.size = size;
}

void _copy_element_to_array(array *arr, size_t index, void *element)
{
    memcpy((unsigned char*)arr->base.p_data + (index * arr->_element_size), element, arr->_element_size);
}

void array_push(array* arr, void *element)
{
    if (arr->base.size == arr->cap) {
        array_grow(arr);
    }
    _copy_element_to_array(arr, arr->base.size, element);
    arr->base.size ++;
}

void array_grow(array *arr)
{
    arr->cap *= 2;
    arr->base.p_data = realloc(arr->base.p_data, arr->cap * arr->_element_size);
}

void array_set(array *arr, size_t index, void *element)
{
    if(index>arr->cap - 1)
        return;
    _copy_element_to_array(arr, index, element);
}

void* array_get(array *arr, size_t index)
{
    return (unsigned char*)arr->base.p_data + (index * arr->_element_size);
}

void* array_data(array* arr)
{
    return arr->base.p_data;
}

void* array_back(array *arr)
{
    return array_get(arr, arr->base.size - 1);
}

void* array_front(array *arr)
{
    return array_get(arr, 0);
}

size_t array_size(array *arr)
{
    return arr->base.size;
}

void array_free(array* arr)
{
    array_deinit(arr);
    free(arr);
}

void array_deinit(array *arr)
{
    for(size_t i=0;i<arr->base.size;i++){
        void *elem = array_get(arr, i);
        if (arr->free && elem)
            arr->free(elem);
    }
    free(arr->base.p_data);
}
