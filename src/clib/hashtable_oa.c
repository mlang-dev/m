// /*
//  * hashtable.c
//  * 
//  * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
//  *
//  * dynamic hashtable implemented using open addressing with linear probing
//  */
// #include <stddef.h>
// #include <stdlib.h>
// #include <string.h>
// #include <stdio.h>
// #include <assert.h>

// #include "clib/array.h"
// #include "clib/string.h"
// #include "clib/hashtable.h"
// #include "clib/hash.h"

// void hashtable_init_ref(hashtable *ht)
// {
//     hashtable_init_fun(ht, 0, 0, default_fun, default_fun);
// }

// void hashtable_init(hashtable *ht, size_t key_object_size, size_t value_object_size)
// {
//     hashtable_init_fun(ht, key_object_size, value_object_size, default_fun, default_fun);
// }

// void hashtable_init_fun(hashtable *ht, size_t key_object_size, size_t value_object_size, fun key_f, fun value_f)
// {
//     ht->size = 0;
//     ht->key_object_size = key_object_size;
//     ht->value_object_size = value_object_size;
//     ht->key_f = key_f;
//     ht->value_f = value_f;
//     array_init_size(&ht->buckets, sizeof(hashbox), 19, default_fun);
// }

// size_t _get_index(array *buckets, void *key, size_t key_size)
// {
//     return hash(key, key_size) % buckets->cap;
// }

// typedef bool (*match_predicate) (hashbox* box, void *key, size_t key_size);

// bool match_empty(hashbox* box, void *key, size_t key_size)
// {
//     return box->status == HASH_EMPTY;
// }

// bool match_found(hashbox* box, void *key, size_t key_size)
// {
//     return box->status == HASH_EXIST && memcmp(hashbox_get_key(box), key, key_size) == 0;
// }

// bool match_search(hashbox* box, void *key, size_t key_size)
// {
//     return match_empty(box, key, key_size) || match_found(box, key, key_size);
// }

// hashbox* _find_from_to(array *buckets, void *key, size_t key_size, size_t from, size_t to, match_predicate match)
// {
//     size_t index = from;
//     while(index>=to){
//         hashbox *box = (hashbox*)array_get(buckets, index);
//         if (match(box, key, key_size))
//             return box;
//         index--;
//     }
//     return NULL;
// }

// hashbox *_find(array *buckets, void *key, size_t key_size, match_predicate match)
// {
//     size_t index = _get_index(buckets, key, key_size);
//     hashbox *box = _find_from_to(buckets, key, key_size, index, 0, match);  
//     if (box)
//         return box;
//     return _find_from_to(buckets, key, key_size, buckets->cap - 1, index + 1, match);    
// }

// void _add_to_buckets(array *buckets, void *key, size_t key_size, void *value, size_t value_size, fun key_f, fun value_f)
// {
//     hashbox *box = _find(buckets, key, key_size, match_empty);
//     if(box->key_value_pair){
//         printf("oops ! something wrong: %p, status: %d\n", box->key_value_pair, box->status);
//     }
//     assert(box->key_value_pair == NULL);
//     assert(value || !value_size);
//     box->key_value_pair = malloc(key_size + value_size);
//     key_f.copy(hashbox_get_key(box), key, key_size);
//     box->key_size = key_size;
//     box->value_size = value_size;
//     //get_init(key_data->type)(box->key_data, key_data);
//     if (value){
//         value_f.copy(hashbox_get_value(box), value, value_size);//malloc(ht->value_object_size);
//         //get_init(value_data->type)(box->value_data, value_data);
//     }
//     box->status = HASH_EXIST;
// }

// void _hashtable_grow(hashtable* ht)
// {
//     array new_buckets;
//     array_init_size(&new_buckets, ht->buckets._element_size, ht->buckets.cap * 2, default_fun);
//     for(size_t i = 0; i<ht->buckets.cap; i++){
//         hashbox* h = (hashbox*)array_get(&ht->buckets, i);
//         if (h->status == HASH_EXIST){
//             size_t key_size = ht->key_object_size? ht->key_object_size : h->key_size;
//             size_t value_size = ht->value_object_size? ht->value_object_size : h->value_size;
//             _add_to_buckets(&new_buckets, hashbox_get_key(h), key_size, hashbox_get_value(h), value_size, ht->key_f, ht->value_f);
//         }
//     }
//     ht->buckets = new_buckets;
// }

// void* hashtable_get(hashtable *ht, void *key)
// {
//     hashbox *box = _find(&ht->buckets, key, ht->key_object_size, match_search);
//     if (box->status==HASH_EXIST)
//         return hashbox_get_value(box);
//     return NULL;
// }

// void* hashtable_get_ref(hashtable *ht, value_ref key)
// {
//     hashbox *box = _find(&ht->buckets, key.data, key.size, match_search);
//     if (box->status==HASH_EXIST)
//         return hashbox_get_value(box);
//     return NULL;
// }

// void hashtable_add(hashtable *ht, void *key, void *value)
// {
//     size_t size_cap = ht->buckets.cap / 2;
//     if (ht->size >= size_cap){
//         _hashtable_grow(ht);
//     }
//     _add_to_buckets(&ht->buckets, key, ht->key_object_size, value, ht->value_object_size, ht->key_f, ht->value_f);
//     ht->size ++;
// }

// void hashtable_add_ref(hashtable *ht, value_ref key, value_ref value)
// {
//     size_t size_cap = ht->buckets.cap / 2;
//     if (ht->size >= size_cap){
//         _hashtable_grow(ht);
//     }
//     _add_to_buckets(&ht->buckets, key.data, key.size, value.data, value.size, ht->key_f, ht->value_f);
//     ht->size ++;
// }

// size_t hashtable_size(hashtable *ht)
// {
//     return ht->size;
// }

// bool hashtable_in(hashtable *ht, void *key)
// {
//     hashbox *box = _find(&ht->buckets, key, ht->key_object_size, match_search);
//     return (box && box->status == HASH_EXIST);
// }

// bool hashtable_in_ref(hashtable *ht, value_ref key)
// {
//     hashbox *box = _find(&ht->buckets, key.data, key.size, match_search);
//     return (box && box->status == HASH_EXIST);
// }

// void hashtable_deinit(hashtable *ht)
// {
//     for(size_t i = 0; i<ht->buckets.cap; i++){
//         hashbox* h = (hashbox*)array_get(&ht->buckets, i);
//         if (h->status == HASH_EXIST){
//             ht->key_f.free(hashbox_get_key(h));
//         }
//     }
//     array_deinit(&ht->buckets);
// }

// void hashtable_clear(hashtable *ht)
// {
//     for(size_t i = 0; i<ht->buckets.cap; i++){
//         hashbox* h = (hashbox*)array_get(&ht->buckets, i);
//         if (h->status == HASH_EXIST){
//             ht->key_f.free(hashbox_get_key(h));
//         if (h->status != HASH_EMPTY)
//             h->status = HASH_EMPTY;
//             h->key_value_pair = NULL;
//         }
//     }
// }

// void hashtable_remove(hashtable *ht, const char *key_p)
// {
//     hashbox *box = _find(&ht->buckets, (void*)key_p, strlen(key_p) + 1, match_search);
//     if(box){
//         if(box->status == HASH_EXIST){
//             ht->key_f.free(hashbox_get_key(box));
//             box->status = HASH_DELETED;
//         }
//     }
// }

// void hashtable_set_p(hashtable* ht, const char* key_p, void* value_p)
// {
//     value_ref key = {(void*)key_p, strlen(key_p) + 1};
//     value_ref value = {(void*)&value_p, sizeof(value_p)};
//     hashtable_add_ref(ht, key, value);
// }

// void* hashtable_get_p(hashtable* ht, const char* key_p)
// {
//     value_ref key = {(void*)key_p, strlen(key_p) + 1};
//     void** p = (void**)hashtable_get_ref(ht, key);
//     return p? *p : NULL;
// }

// bool hashtable_in_p(hashtable* ht, const char* key_p)
// {
//     value_ref key = {(void*)key_p, strlen(key_p) + 1};
//     return hashtable_in_ref(ht, key);
// }
