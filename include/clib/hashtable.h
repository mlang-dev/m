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
#include <stdbool.h>

#include "hash.h"
#include "list.h"

#ifdef __cplusplus
extern "C" {
#endif

struct hash_entry
{
    list_entry(hash_entry) list;
    struct hashbox data;
};

list_head(hash_head, hash_entry);

struct hashtable{
    struct hash_head *heads;
    size_t size;
    size_t cap;
};

void hashtable_init(struct hashtable *ht);
// void hashtable_init(hashtable *ht, size_t key_object_size, size_t value_object_size);
// void hashtable_init_fun(hashtable *ht, size_t key_object_size, size_t value_object_size, 
// fun key_f, fun value_f);
void hashtable_deinit(struct hashtable *ht);
// void hashtable_add(hashtable *ht, void *key, void *value);
// void hashtable_add_ref(hashtable *ht, value_ref key, value_ref value);
// void* hashtable_get(hashtable *ht, void *key);
// void* hashtable_get_ref(hashtable *ht, value_ref key);
size_t hashtable_size(struct hashtable *ht);
// bool hashtable_in(hashtable* hs, void *key);
// bool hashtable_in_ref(hashtable *ht, value_ref key);
void hashtable_set(struct hashtable *ht, const char *key, void *value);
void* hashtable_get(struct hashtable *ht, const char *key);
bool hashtable_in(struct hashtable *ht, const char *key);
void hashtable_clear(struct hashtable* ht);
void hashtable_remove(struct hashtable* ht, const char* key);

#ifdef __cplusplus
}
#endif

#endif