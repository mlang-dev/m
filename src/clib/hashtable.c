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
    ht->size = 0;
    ht->key_object_size = key_object_size;
    ht->value_object_size = value_object_size;
    array_init_size(&ht->buckets, sizeof(hashable), 19);
}

size_t _get_index(array *buckets, object *key_data)
{
    return hash(get_data(key_data->type)(key_data), key_data->size) % buckets->cap;
}

typedef bool (*match_predicate) (hashable* box, object *key_data);

bool match_empty(hashable* box, object *key_data)
{
    return box->status == HASH_EMPTY || box->status == HASH_DELETED;
}

bool match_found(hashable* box, object *key_data)
{
    return box->status == HASH_EXIST && get_eq(box->key_data->type)(box->key_data, key_data);
}

bool match_search(hashable* box, object *key_data)
{
    return match_empty(box, key_data) || match_found(box, key_data);
}

hashable *_find_from_to(array *buckets, object *key_data, size_t from, size_t to, match_predicate match)
{
    size_t index = from;
    while(index>=to){
        hashable *box = (hashable*)array_get(buckets, index);
        if (match(box, key_data))
            return box;
        index--;
    }
    return NULL;
}

hashable *_find(array *buckets, object *key_data, match_predicate match)
{
    size_t index = _get_index(buckets, key_data);
    hashable *box = _find_from_to(buckets, key_data, index, 0, match);  
    if (box)
        return box;
    return _find_from_to(buckets, key_data, buckets->cap - 1, index + 1, match);    
}

void _add_to_buckets(hashtable *ht, object *key_data, object *value_data)
{
    hashable *box = _find(&ht->buckets, key_data, match_empty);
    assert(box && box->key_data == NULL);
    box->key_data = malloc(ht->key_object_size);
    get_init(key_data->type)(box->key_data, key_data);
    if (value_data){
        box->value_data = malloc(ht->value_object_size);
        get_init(value_data->type)(box->value_data, value_data);
    }
    box->status = HASH_EXIST;
}

void _hashtable_grow(hashtable* ht)
{
    array new_buckets;
    array_init_size(&new_buckets, ht->buckets._element_size, ht->buckets.cap * 2);
    for(size_t i = 0; i<ht->buckets.cap; i++){
        hashable* h = (hashable*)array_get(&ht->buckets, i);
        if (h->status != HASH_EMPTY)
            _add_to_buckets(ht, h->key_data, h->value_data);
    }
    ht->buckets = new_buckets;
}

void hashtable_add(hashtable *ht, object *key_data, object *value_data)
{
    size_t size_cap = ht->buckets.cap / 2;
    if (ht->size >= size_cap){
        _hashtable_grow(ht);
    }
    _add_to_buckets(ht, key_data, value_data);
    ht->size ++;
}

object* hashtable_get(hashtable *ht, object *key_data)
{
    hashable *box = _find(&ht->buckets, key_data, match_search);
    if (box->status==HASH_EXIST)
        return box->value_data;
    return NULL;
}

size_t hashtable_size(hashtable *ht)
{
    return ht->size;
}

bool hashtable_in(hashtable *ht, object* key_data)
{
    hashable *box = _find(&ht->buckets, key_data, match_search);
    return (box && box->status == HASH_EXIST);
}

void hashtable_deinit(hashtable *hs)
{
    array_deinit(&hs->buckets);
}
