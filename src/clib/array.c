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
#include <stdio.h>

#include "clib/array.h"
#include "clib/string.h"

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
    arr->element_init = NULL;
    arr->element_deinit = NULL;
}

void array_insert(array* a, void* element)
{
    if (a->size == a->cap) {
        a->cap *= 2;
        a->data = realloc(a->data, a->cap * a->_element_size);
    }
    if (a->element_init)
        a->element_init(a->data + (a->size * a->_element_size), element);
    else
        memcpy(a->data + (a->size * a->_element_size), (unsigned char*)element, a->_element_size);
    a->size ++;
}

void* array_get(array *arr, size_t index)
{
    return (void*)(arr->data + (index * arr->_element_size));
}

void array_free(array* arr)
{
    array_deinit(arr);
    free(arr);
}

void string_array_init(array *arr)
{
    array_init(arr, sizeof(string));
    arr->element_init = string_init_generic;
    arr->element_deinit = string_deinit_generic;
}

void array_deinit(array *arr)
{
    if (arr->element_deinit){
        for(size_t i=0;i<arr->size;i++){
            arr->element_deinit(array_get(arr, i));
        }
    }
    free(arr->data);
}
