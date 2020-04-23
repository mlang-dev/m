/*
 * hashtable.h
 * 
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * dynamic hash table c header file
 */
#ifndef __CLIB_HASHTABLE_H__
#define __CLIB_HASHTABLE_H__

#include <stddef.h>

#include "array.h"
#include "object.h"
#include "generic.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef struct {
    array buckets;
    size_t key_object_size;
    size_t value_object_size;
    size_t size;
    free_fun key_free;
    free_fun value_free;
} hashtable;

void hashtable_init(hashtable *ht, size_t key_object_size, size_t value_object_size);
void hashtable_init_free(hashtable *ht, size_t key_object_size, size_t value_object_size, free_fun key_free, free_fun value_free);
void hashtable_deinit(hashtable *ht);
void hashtable_add(hashtable *ht, void *key, void *value);
void* hashtable_get(hashtable *ht, void *key);
size_t hashtable_size(hashtable *ht);
bool hashtable_in(hashtable* hs, void *key);

#ifdef __cplusplus
}
#endif

#endif