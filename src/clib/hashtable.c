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

void hashtable_init_ref(hashtable *ht)
{
    hashtable_init_fun(ht, 0, 0, default_fun, default_fun);
}

void hashtable_init(hashtable *ht, size_t key_object_size, size_t value_object_size)
{
    hashtable_init_fun(ht, key_object_size, value_object_size, default_fun, default_fun);
}

void hashtable_init_fun(hashtable *ht, size_t key_object_size, size_t value_object_size, fun key_f, fun value_f)
{
    ht->size = 0;
    ht->key_object_size = key_object_size;
    ht->value_object_size = value_object_size;
    ht->key_f = key_f;
    ht->value_f = value_f;
    array_init_size(&ht->buckets, sizeof(hash_box), 19, default_fun);
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

void _add_to_buckets(array *buckets, void *key, size_t key_size, void *value, size_t value_size, fun key_f, fun value_f)
{
    hash_box *box = _find(buckets, key, key_size, match_empty);
    assert(box && box->key == NULL);
    box->key = malloc(key_size);
    key_f.copy(box->key, key, key_size);
    //get_init(key_data->type)(box->key_data, key_data);
    if (value){
        box->value = malloc(value_size);
        value_f.copy(box->value, value, value_size);//malloc(ht->value_object_size);
        //get_init(value_data->type)(box->value_data, value_data);
    }
    box->status = HASH_EXIST;
}

void _hashtable_grow(hashtable* ht)
{
    array new_buckets;
    array_init_size(&new_buckets, ht->buckets._element_size, ht->buckets.cap * 2, default_fun);
    for(size_t i = 0; i<ht->buckets.cap; i++){
        hash_box* h = (hash_box*)array_get(&ht->buckets, i);
        if (h->status != HASH_EMPTY)
            _add_to_buckets(&new_buckets, h->key, ht->key_object_size, h->value, ht->value_object_size, 
            ht->key_f, ht->value_f);
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

void* hashtable_get_ref(hashtable *ht, value_ref key)
{
    hash_box *box = _find(&ht->buckets, key.data, key.size, match_search);
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
    _add_to_buckets(&ht->buckets, key, ht->key_object_size, value, ht->value_object_size, ht->key_f, ht->value_f);
    ht->size ++;
}

void hashtable_add_ref(hashtable *ht, value_ref key, value_ref value)
{
    size_t size_cap = ht->buckets.cap / 2;
    if (ht->size >= size_cap){
        _hashtable_grow(ht);
    }
    _add_to_buckets(&ht->buckets, key.data, key.size, value.data, value.size, ht->key_f, ht->value_f);
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

bool hashtable_in_ref(hashtable *ht, value_ref key)
{
    hash_box *box = _find(&ht->buckets, key.data, key.size, match_search);
    return (box && box->status == HASH_EXIST);
}

void hashtable_deinit(hashtable *ht)
{
    for(size_t i = 0; i<ht->buckets.cap; i++){
        hash_box* h = (hash_box*)array_get(&ht->buckets, i);
        if (h->status != HASH_EMPTY){
            ht->key_f.free(h->key);
            if(h->value){
                ht->value_f.free(h->value);
            }
        }
    }
    array_deinit(&ht->buckets);
}
