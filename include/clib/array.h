/*
 * array.h
 * 
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * dynamic array c header file
 */
#ifndef __CLIB_ARRAY_H__
#define __CLIB_ARRAY_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include "clib/object.h"
#include "clib/generic.h"

typedef struct _array{
    object base;
    //bytes array
    size_t cap;
    size_t _element_size; //element size of the array
    fun f;
} array;

array* array_new(size_t element_size);
void array_string_init(array* a);
void array_init(array* a, size_t element_size);
void array_init_fun(array* a, size_t element_size, fun f);
void array_init_size(array *a, size_t element_size, size_t init_size, fun f);
void array_grow(array *a);
void array_deinit(array *a);
void array_push(array* a, void* element);
void array_set(array *a, size_t index, void *element);
void array_copy(array *dest, array *src);
void* array_get(array* a, size_t index);
void* array_data(array *a);
void* array_back(array *a);
void* array_front(array *a);
size_t array_size(array *a);
void array_free(array* a);

#ifdef __cplusplus
}
#endif

#endif