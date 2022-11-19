/*
 * hashtable.h
 * 
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * dynamic hash table c header file
 */
#ifndef __CLIB_HASHTABLE_H__
#define __CLIB_HASHTABLE_H__

#include <stdbool.h>
#include <stddef.h>

#include "hash.h"
#include "list.h"

#ifdef __cplusplus
extern "C" {
#endif
/*defined two data structure hash_head, and hash entry to contain existing struct hashbox data*/
link_list(hash_head, hash_entry, struct hashbox)

struct hashtable {
    struct hash_head *heads;
    size_t size;
    size_t cap;
    size_t value_size;
    size_t key_size;
    free_fun free_element;
    bool key_is_c_str;
};

//default hashtable without value size will store object pointer
void hashtable_init(struct hashtable *ht);
void hashtable_c_str_key_init(struct hashtable *ht, free_fun free_element);

void hashtable_init_with_value_size(struct hashtable *ht, size_t value_size, free_fun free_element);
void hashtable_init_with_size(struct hashtable *ht, size_t key_size, size_t value_size);
void hashtable_deinit(struct hashtable *ht);
size_t hashtable_size(struct hashtable *ht);
void hashtable_set(struct hashtable *ht, const char *key, void *value);
void hashtable_set2(struct hashtable *ht, const char *key, size_t key_size, void *value);
void hashtable_set_g(struct hashtable *ht, void *key, size_t key_size, void *value, size_t value_size);
void hashtable_set_p(struct hashtable *ht, void *key, void *value);
void *hashtable_get(struct hashtable *ht, const char *key);
void *hashtable_get2(struct hashtable *ht, const char *key, size_t key_size);
void *hashtable_get_g(struct hashtable *ht, void *key, size_t key_size);
//hashtable with key as pointer
void *hashtable_get_p(struct hashtable *ht, void *key);
void hashtable_set_v(struct hashtable *ht, void *key, void *value);
void *hashtable_get_v(struct hashtable *ht, void *key);

int hashtable_get_int(struct hashtable *ht, void *key);
void hashtable_set_int(struct hashtable *ht, void *key, int value);
bool hashtable_in(struct hashtable *ht, const char *key);
bool hashtable_in_g(struct hashtable *ht, void *key, size_t key_size);
bool hashtable_in_p(struct hashtable *ht, void *key);
bool hashtable_in_v(struct hashtable *ht, void *key);
void hashtable_clear(struct hashtable *ht);
void hashtable_remove(struct hashtable *ht, const char *key);
void hashtable_remove_p(struct hashtable *ht, void *key);
void hashtable_remove_g(struct hashtable *ht, void *key, size_t key_size);

#ifdef __cplusplus
}
#endif

#endif
