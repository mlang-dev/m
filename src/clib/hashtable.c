/*
 * hashtable.c
 * 
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * dynamic hashtable with collision resolution implemented using chaining (singly-linked list)
 */
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>

#include "clib/hashtable.h"
#include "clib/hash.h"

bool match_entry (struct hash_entry *entry, const char *key, size_t key_size){
    return entry->data.key_size == key_size && !memcmp(entry->data.key_value_pair, key, key_size);
} 

void hashtable_init(struct hashtable *ht)
{
    ht->cap = 19;
    ht->size = 0;
    ht->heads = calloc(ht->cap, sizeof(struct hash_head));
}

size_t _get_index(struct hashtable *ht, unsigned char *key, size_t key_size)
{
    return hash(key, key_size) % ht->cap;
}

void _hashtable_grow(struct hashtable* ht)
{
    struct hash_head *head, *heads, *new_head;
    struct hash_entry *entry, *next;
    size_t cap, index;

    if(ht->size > 0.75 * ht->cap){
        cap = ht->cap;
        heads = ht->heads;
        ht->cap *= 2;
        ht->heads = calloc(ht->cap, sizeof(struct hash_head));
        for(size_t i = 0; i < cap; i++){
            head = &heads[i];
            entry = head->first;
            while(entry){
                next = entry->list.next;
                index = _get_index(ht, (unsigned char*)entry->data.key_value_pair, entry->data.key_size);
                new_head = &ht->heads[index];
                list_insert_head(new_head, entry, list);
                entry = next;
            }
        }
        free(heads);
    }
}

struct hashbox* _get_hashbox(struct hash_head *head, const char *key, size_t key_size)
{
    struct hash_entry *entry;
    list_foreach(entry, head, list){
        if(match_entry(entry, key, key_size)){
            return &entry->data;
        }
    }    
    return NULL;
}

struct hash_entry* _hash_entry_new(size_t key_size, size_t value_size)
{
    struct hash_entry *entry = (struct hash_entry*)calloc(1, sizeof(struct hash_entry));
    entry->data.status = HASH_EXIST;
    entry->data.key_size = key_size;
    entry->data.value_size = value_size;
    entry->data.key_value_pair = (unsigned char*)malloc(key_size + value_size);
    return entry;
}

void _hash_entry_free(struct hash_entry *entry)
{   
    free(entry->data.key_value_pair);
    free(entry);
}

void hashtable_clear(struct hashtable *ht)
{
    struct hash_head *head;
    struct hash_entry *entry, *next;
    for(size_t i = 0; i<ht->cap; i++){
        head = &ht->heads[i];
        entry = head->first;
        while(entry){
            next = entry->list.next;
            _hash_entry_free(entry);
            entry = next;
        }
        head->first = NULL;
    }
}

void hashtable_remove(struct hashtable *ht, const char *key)
{
	struct hash_head *head;
	struct hash_entry *entry;
    struct hash_entry *prev = NULL;
    size_t key_size = strlen(key) + 1;
    head = &ht->heads[_get_index(ht, (unsigned char*)key, key_size)];
    list_foreach(entry, head, list){
        if(match_entry(entry, key, key_size)){
            break;
        }
        prev = entry;
    }    
    if (entry){
        if(prev)
            list_remove_next(prev, list);
        else
            list_remove_head(head, list);
        _hash_entry_free(entry);
        ht->size --;
    }
}

void hashtable_set(struct hashtable* ht, const char* key, void* value)
{
    size_t key_size = strlen(key) + 1;
    size_t value_size = sizeof(value);
    _hashtable_grow(ht);
    struct hash_head *head = &ht->heads[_get_index(ht, (unsigned char*)key, key_size)];
    struct hashbox *box = _get_hashbox(head, key, key_size);
    if(!box){
        struct hash_entry *entry = _hash_entry_new(key_size, value_size);
        memcpy(entry->data.key_value_pair, key, key_size);
        list_insert_head(head, entry, list);
        ht->size ++;
        box = &entry->data;
    }
    memcpy(box->key_value_pair + key_size, &value, value_size);
}

void* hashtable_get(struct hashtable* ht, const char* key)
{
	struct hash_head *head;
    void **data = NULL;
    struct hashbox *box;
    size_t key_size = strlen(key) + 1;
    head = &ht->heads[_get_index(ht, (unsigned char*)key, key_size)];
    box = _get_hashbox(head, key, key_size);
    if (box){
        data = (void**)(box->key_value_pair + key_size);
        return data ? *data : NULL;
    }    
    return NULL;
}

bool hashtable_in(struct hashtable* ht, const char* key)
{
    void *data = hashtable_get(ht, key);
    return data != NULL;
}

size_t hashtable_size(struct hashtable* ht)
{
    return ht->size;
}

void hashtable_deinit(struct hashtable *ht)
{
    hashtable_clear(ht);
    free(ht->heads);
}
