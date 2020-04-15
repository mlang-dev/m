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

typedef struct {
    void** data;
    size_t used_size;
    size_t total_size;
} array;

array* array_init();
void array_append(array* a, void* element);
void array_free(array* a);

#ifdef __cplusplus
}
#endif

#endif