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


typedef struct _hashtable{
    array buckets;
    size_t key_object_size;
    size_t value_object_size;
    size_t size;
    fun key_f;
    fun value_f;
} hashtable;

void hashtable_init_ref(hashtable *ht);
void hashtable_init(hashtable *ht, size_t key_object_size, size_t value_object_size);
void hashtable_init_fun(hashtable *ht, size_t key_object_size, size_t value_object_size, 
fun key_f, fun value_f);
void hashtable_deinit(hashtable *ht);
void hashtable_add(hashtable *ht, void *key, void *value);
void hashtable_add_ref(hashtable *ht, value_ref key, value_ref value);
void* hashtable_get(hashtable *ht, void *key);
void* hashtable_get_ref(hashtable *ht, value_ref key);
size_t hashtable_size(hashtable *ht);
bool hashtable_in(hashtable* hs, void *key);
bool hashtable_in_ref(hashtable *ht, value_ref key);
void hashtable_set_p(hashtable* ht, const char* key_p, void* value_p);
void* hashtable_get_p(hashtable* ht, const char* key_p);
bool hashtable_in_p(hashtable* ht, const char* key_p);
void hashtable_clear(hashtable* ht);
void hashtable_remove(hashtable* ht, const char* key_p);

#ifdef __cplusplus
}
#endif

#endif