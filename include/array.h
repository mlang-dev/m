/*
 * array.h
 * 
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * array c header file
 */
#ifndef __LIB_ARRAY_H__
#define __LIB_ARRAY_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    void** data;
    size_t used;
    size_t size;
} Array;

Array* array_init();
void array_append(Array* a, void* element);
void array_free(Array* a);

#ifdef __cplusplus
}
#endif

#endif