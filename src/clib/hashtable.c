/*
 * hashtable.c
 * 
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * dynamic hashtable implemented using open addressing with linear probing
 */
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>

#include "clib/array.h"
#include "clib/string.h"
#include "clib/hashtable.h"
#include "clib/hash.h"

void hashtable_init(hashtable *ht, size_t key_object_size, size_t value_object_size)
{
    hashtable_init_free(ht, key_object_size, value_object_size, generic_free, generic_free);
}

void hashtable_init_free(hashtable *ht, size_t key_object_size, size_t value_object_size, free_fun key_free, free_fun value_free)
{
    ht->size = 0;
    ht->key_object_size = key_object_size;
    ht->value_object_size = value_object_size;
    ht->key_free = key_free;
    ht->value_free = value_free;
    array_init_size(&ht->buckets, sizeof(hash_box), 19);
}

size_t _get_index(array *buckets, void *key, size_t key_size)
{
    return hash(key, key_size) % buckets->cap;
}

typedef bool (*match_predicate) (hash_box* box, void *key, size_t key_size);

bool match_empty(hash_box* box, void *key, size_t key_size)
{
    return box->status == HASH_EMPTY || box->status == HASH_DELETED;
}

bool match_found(hash_box* box, void *key, size_t key_size)
{
    return box->status == HASH_EXIST && memcmp(box->key, key, key_size) == 0;
}

bool match_search(hash_box* box, void *key, size_t key_size)
{
    return match_empty(box, key, key_size) || match_found(box, key, key_size);
}

hash_box* _find_from_to(array *buckets, void *key, size_t key_size, size_t from, size_t to, match_predicate match)
{
    size_t index = from;
    while(index>=to){
        hash_box *box = (hash_box*)array_get(buckets, index);
        if (match(box, key, key_size))
            return box;
        index--;
    }
    return NULL;
}

hash_box *_find(array *buckets, void *key, size_t key_size, match_predicate match)
{
    size_t index = _get_index(buckets, key, key_size);
    hash_box *box = _find_from_to(buckets, key, key_size, index, 0, match);  
    if (box)
        return box;
    return _find_from_to(buckets, key, key_size, buckets->cap - 1, index + 1, match);    
}

void _add_to_buckets(array *buckets, void *key, size_t key_size, void *value, size_t value_size)
{
    hash_box *box = _find(buckets, key, key_size, match_empty);
    assert(box && box->key == NULL);
    box->key = malloc(key_size);
    memcpy(box->key, key, key_size);
    //get_init(key_data->type)(box->key_data, key_data);
    if (value){
        box->value = malloc(value_size);
        memcpy(box->value, value, value_size);//malloc(ht->value_object_size);
        //get_init(value_data->type)(box->value_data, value_data);
    }
    box->status = HASH_EXIST;
}

void _hashtable_grow(hashtable* ht)
{
    array new_buckets;
    array_init_size(&new_buckets, ht->buckets._element_size, ht->buckets.cap * 2);
    for(size_t i = 0; i<ht->buckets.cap; i++){
        hash_box* h = (hash_box*)array_get(&ht->buckets, i);
        if (h->status != HASH_EMPTY)
            _add_to_buckets(&new_buckets, h->key, ht->key_object_size, h->value, ht->value_object_size);
    }
    ht->buckets = new_buckets;
}

void* hashtable_get(hashtable *ht, void *key)
{
    hash_box *box = _find(&ht->buckets, key, ht->key_object_size, match_search);
    if (box->status==HASH_EXIST)
        return box->value;
    return NULL;
}

void hashtable_add(hashtable *ht, void *key, void *value)
{
    size_t size_cap = ht->buckets.cap / 2;
    if (ht->size >= size_cap){
        _hashtable_grow(ht);
    }
    _add_to_buckets(&ht->buckets, key, ht->key_object_size, value, ht->value_object_size);
    ht->size ++;
}

size_t hashtable_size(hashtable *ht)
{
    return ht->size;
}

bool hashtable_in(hashtable *ht, void *key)
{
    hash_box *box = _find(&ht->buckets, key, ht->key_object_size, match_search);
    return (box && box->status == HASH_EXIST);
}

void hashtable_deinit(hashtable *ht)
{
    for(size_t i = 0; i<ht->buckets.cap; i++){
        hash_box* h = (hash_box*)array_get(&ht->buckets, i);
        if (h->status != HASH_EMPTY){
            ht->key_free(h->key);
            if(ht->value_free){
                ht->value_free(h->value);
            }
        }
    }
    array_deinit(&ht->buckets);
}
