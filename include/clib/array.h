/*
 * array.h
 * 
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * array c header file
 */
#ifndef __CLIB_ARRAY_H__
#define __CLIB_ARRAY_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>

typedef struct {
    size_t size;
    size_t cap;
    size_t _element_size; //element size of the array
    unsigned char *data; //bytes array
} array;

array* array_new(size_t element_size);
void array_init(array* arr, size_t element_size);
void array_deinit(array *arr);
void array_append(array* a, void* element);
void* array_get(array* arr, size_t index);
void array_free(array* a);

#ifdef __cplusplus
}
#endif

#endif