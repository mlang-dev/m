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
    a->base.p_data = malloc(init_size * element_size);
    memset(a->base.p_data, 0, init_size * element_size);
    a->cap = init_size;
    a->base.size = 0;
}

void array_copy(array *dest, array *src)
{
    array_init_size(dest, src->_element_size, src->base.size);
    memcpy(dest->base.p_data, src->base.p_data, src->base.size * src->_element_size);
}

void _copy_element_to_array(array *a, size_t index, object *element)
{
    get_init(element->type)(a->base.p_data + (index * a->_element_size), element);
}

void array_push(array* a, object* element)
{
    if (a->base.size == a->cap) {
        array_grow(a);
    }
    _copy_element_to_array(a, a->base.size, element);
    a->base.size ++;
}

void array_push_g(array* a, void* element)
{
    if (a->base.size == a->cap) {
        array_grow(a);
    }
    memcpy(a->base.p_data + (a->base.size * a->_element_size), element, a->_element_size);
    a->base.size ++;
}

void array_grow(array *a)
{
    a->cap *= 2;
    a->base.p_data = realloc(a->base.p_data, a->cap * a->_element_size);
}

void array_set(array *a, size_t index, object *element)
{
    if(index>a->cap - 1)
        return;
    _copy_element_to_array(a, index, element);
    if (a->base.size < index + 1)
        a->base.size = index + 1;
}

object* array_get(array *a, size_t index)
{
    return (object*)array_get_g(a, index);
}

void* array_get_g(array *a, size_t index)
{
    return a->base.p_data + (index * a->_element_size);
}

void* array_data(array* a)
{
    return a->base.p_data;
}

object* array_back(array *a)
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

void array_deinit(array *a)
{
    for(size_t i=0;i<a->base.size;i++){
        object *obj = array_get(a, i);
        get_deinit(obj->type)(obj);
    }
    free(a->base.p_data);
}
