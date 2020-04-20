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

#include "clib/array.h"
#include "clib/string.h"

array* array_new(size_t element_size)
{
    array* a = (array*)malloc(sizeof(array));
    array_init(a, element_size);
    return a;
}

void array_init(array *a, size_t element_size)
{
    array_init_size(a, element_size, 7);
}

void array_init_size(array *a, size_t element_size, size_t init_size)
{
    a->_element_size = element_size;
    a->base.p_data = (unsigned char*)malloc(init_size * element_size);
    memset(a->base.p_data, 0, init_size * element_size);
    a->cap = init_size;
    a->base.size = 0;
    a->element_init = NULL;
    a->element_deinit = NULL;
}

void _copy_element_to_array(array *a, size_t index, void *element)
{
    if (a->element_init)
        a->element_init(a->base.p_data + (index * a->_element_size), element);
    else
        memcpy(a->base.p_data + (index * a->_element_size), (unsigned char*)element, a->_element_size);
}

void array_push(array* a, void* element)
{
    if (a->base.size == a->cap) {
        array_grow(a);
    }
    _copy_element_to_array(a, a->base.size, element);
    a->base.size ++;
}

void array_grow(array *a)
{
    a->cap *= 2;
    a->base.p_data = realloc(a->base.p_data, a->cap * a->_element_size);
}

void array_set(array *a, size_t index, void *element)
{
    if(index>a->cap - 1)
        return;
    _copy_element_to_array(a, index, element);
    if (a->base.size < index + 1)
        a->base.size = index + 1;
}

void* array_get(array *a, size_t index)
{
    return (void*)(a->base.p_data + (index * a->_element_size));
}

void* array_data(array* a)
{
    return a->base.p_data;
}

void* array_back(array *a)
{
    return array_get(a, a->base.size - 1);
}

size_t array_size(array *a)
{
    return a->base.size;
}

void array_free(array* a)
{
    array_deinit(a);
    free(a);
}

void string_array_init(array *a)
{
    array_init(a, sizeof(string));
    a->element_init = string_init_generic;
    a->element_deinit = string_deinit_generic;
}

void array_deinit(array *a)
{
    if (a->element_deinit){
        for(size_t i=0;i<a->base.size;i++){
            a->element_deinit(array_get(a, i));
        }
    }
    free(a->base.p_data);
}
