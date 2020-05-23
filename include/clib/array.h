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

#include "clib/generic.h"
#include "clib/object.h"
#include <stddef.h>

struct array {
    object base;
    size_t cap;
    size_t _element_size; //element size of the array
    free_fun free;
};

struct array* array_new(size_t element_size);
void array_init(struct array* arr, size_t element_size);
void array_init_free(struct array* arr, size_t element_size, free_fun free);
void array_grow(struct array* arr);
void array_deinit(struct array* arr);
void array_push(struct array* arr, void* element);
void array_set(struct array* arr, size_t index, void* element);
void array_copy(struct array* dest, struct array* src);
void array_copy_size(struct array* dest, struct array* src, size_t size);
void* array_get(struct array* arr, size_t index);
void* array_data(struct array* arr);
void* array_back(struct array* arr);
void* array_front(struct array* arr);
size_t array_size(struct array* arr);
void array_free(struct array* arr);
void array_add(struct array* dest, struct array* src);

#define ARRAY(var, elem_type, elem_deinit_fun) struct array var;     \
        array_init_free(&var, sizeof(elem_type), elem_deinit_fun)

#define ARRAY_VALUE(var, elem_type) struct array var;     \
        array_init_free(&var, sizeof(elem_type), 0)

#ifdef __cplusplus
}
#endif

#endif
