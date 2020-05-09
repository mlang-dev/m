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

struct array{
    object base;
    //bytes array
    size_t cap;
    size_t _element_size; //element size of the array
    free_fun free;
};

struct array* array_new(size_t element_size);
void array_string_init(struct array* arr);
void array_init(struct array* arr, size_t element_size);
void array_init_fun(struct array* arr, size_t element_size, free_fun free);
void array_init_size(struct array *arr, size_t element_size, size_t init_size, free_fun free);
void array_grow(struct array *arr);
void array_deinit(struct array *arr);
void array_push(struct array* arr, void* element);
void array_set(struct array *arr, size_t index, void *element);
void array_copy(struct array *dest, struct array *src);
void array_copy_size(struct array *dest, struct array *src, size_t size);
void* array_get(struct array* arr, size_t index);
void* array_data(struct array *arr);
void* array_back(struct array *arr);
void* array_front(struct array *arr);
size_t array_size(struct array *arr);
void array_free(struct array* arr);

#ifdef __cplusplus
}
#endif

#endif
