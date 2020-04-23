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


typedef struct {
    object base;
    //bytes array
    size_t cap;
    size_t _element_size; //element size of the array
} array;

array* array_new(size_t element_size);
void array_init(array* a, size_t element_size);
void array_init_size(array *a, size_t element_size, size_t init_size);
void array_grow(array *a);
void array_deinit(array *a);
void array_push(array* a, object* element);
void array_push_g(array* a, void* element);
void array_set(array *a, size_t index, object *element);
void array_copy(array *dest, array *src);
object* array_get(array* a, size_t index);
void* array_get_g(array *a, size_t index);
void* array_data(array *a);
object* array_back(array *a);
size_t array_size(array *a);
void array_free(array* a);

#ifdef __cplusplus
}
#endif

#endif