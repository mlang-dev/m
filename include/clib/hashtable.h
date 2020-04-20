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

#ifdef __cplusplus
extern "C" {
#endif


typedef struct {
    array buckets;
    size_t size;
} hashtable;

void hashtable_init(hashtable *ht);
void hashtable_deinit(hashtable *ht);
void hashtable_add(hashtable *ht, object *key_data, object *value_data);
object* hashtable_get(hashtable *ht, object *key_data);
size_t hashtable_size(hashtable *ht);
bool hashtable_in(hashtable* hs, object *key_data);

#ifdef __cplusplus
}
#endif

#endif