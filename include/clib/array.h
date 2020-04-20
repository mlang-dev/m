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

typedef struct {
    size_t size;
    size_t cap;
    size_t _element_size; //element size of the array
    unsigned char *data; //bytes array
    void (*element_init)(void* dest, void* src); //element fields copy override for deep copy 
    void (*element_deinit)(void* dest); //element fields deep copy free
} array;

array* array_new(size_t element_size);
void array_init(array* a, size_t element_size);
void array_init_size(array *a, size_t element_size, size_t init_size);
void array_grow(array *a);
void array_deinit(array *arr);
void array_push(array* a, void* element);
void array_set(array *a, size_t index, void *element);
void* array_get(array* a, size_t index);
void array_free(array* a);
void string_array_init(array* a);

#ifdef __cplusplus
}
#endif

#endif