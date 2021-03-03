/*
 * hashtable.c
 * 
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * dynamic hashtable with collision resolution implemented using chaining (singly-linked list)
 */
#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "clib/hash.h"
#include "clib/hashtable.h"

bool match_entry(struct hash_entry *entry, const char *key, size_t key_size)
{
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

void _hashtable_grow(struct hashtable *ht)
{
    struct hash_head *head, *heads, *new_head;
    struct hash_entry *entry, *next;
    size_t cap, index;

    if (ht->size > 0.75 * ht->cap) {
        cap = ht->cap;
        heads = ht->heads;
        ht->cap *= 2;
        ht->heads = calloc(ht->cap, sizeof(struct hash_head));
        for (size_t i = 0; i < cap; i++) {
            head = &heads[i];
            entry = head->first;
            while (entry) {
                next = entry->list.next;
                index = _get_index(ht, (unsigned char *)entry->data.key_value_pair, entry->data.key_size);
                new_head = &ht->heads[index];
                list_insert_head(new_head, entry, list);
                entry = next;
            }
        }
        free(heads);
    }
}

struct hashbox *_get_hashbox(struct hash_head *head, const char *key, size_t key_size)
{
    struct hash_entry *entry;
    list_foreach(entry, head, list)
    {
        if (match_entry(entry, key, key_size)) {
            return &entry->data;
        }
    }
    return 0;
}

struct hashbox *_hashtable_get_hashbox(struct hashtable *ht, void *key, size_t key_size)
{
    struct hash_head *head;
    head = &ht->heads[_get_index(ht, (unsigned char *)key, key_size)];
    return _get_hashbox(head, key, key_size);
}

struct hash_entry *_hash_entry_new(size_t key_size, size_t value_size)
{
    struct hash_entry *entry = (struct hash_entry *)calloc(1, sizeof(struct hash_entry));
    entry->data.status = HASH_EXIST;
    entry->data.key_size = key_size;
    entry->data.value_size = value_size;
    entry->data.key_value_pair = malloc(key_size + value_size);
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
    for (size_t i = 0; i < ht->cap; i++) {
        head = &ht->heads[i];
        entry = head->first;
        while (entry) {
            next = entry->list.next;
            _hash_entry_free(entry);
            entry = next;
        }
        head->first = 0;
    }
}

void hashtable_remove(struct hashtable *ht, const char *key)
{
    struct hash_head *head;
    struct hash_entry *entry;
    struct hash_entry *prev = 0;
    size_t key_size = strlen(key) + 1;
    head = &ht->heads[_get_index(ht, (unsigned char *)key, key_size)];
    list_foreach(entry, head, list)
    {
        if (match_entry(entry, key, key_size)) {
            break;
        }
        prev = entry;
    }
    if (entry) {
        if (prev)
            list_remove_next(prev, list);
        else
            list_remove_head(head, list);
        _hash_entry_free(entry);
        ht->size--;
    }
}

void hashtable_set(struct hashtable *ht, const char *key, void *value)
{
    size_t key_size = strlen(key) + 1;
    hashtable_set_g(ht, (void *)key, key_size, value, 0);
}

void hashtable_set_int(struct hashtable *ht, const char *key, int value)
{
    size_t key_size = strlen(key) + 1;
    hashtable_set_g(ht, (void *)key, key_size, &value, sizeof(value));
}

void hashtable_set_g(struct hashtable *ht, void *key, size_t key_size, void *value, size_t value_size)
{
    bool copy_value = (bool)value_size;
    if (!value_size)
        value_size = sizeof(value);
    _hashtable_grow(ht);
    size_t index = _get_index(ht, (unsigned char *)key, key_size);
    struct hash_head *head = &ht->heads[index];
    struct hashbox *box = _get_hashbox(head, key, key_size);
    if (!box) {
        struct hash_entry *entry = _hash_entry_new(key_size, value_size);
        memcpy(entry->data.key_value_pair, key, key_size);
        list_insert_head(head, entry, list);
        ht->size++;
        box = &entry->data;
    }
    if (copy_value)
        memcpy(box->key_value_pair + key_size, value, value_size);
    else
        memcpy(box->key_value_pair + key_size, &value, value_size);
}

void hashtable_set_p(struct hashtable *ht, void *key, void *value)
{
    hashtable_set_g(ht, (void *)&key, sizeof(void *), value, 0);
}

void *hashtable_get_p(struct hashtable *ht, void *key)
{
    return hashtable_get_g(ht, (void *)&key, sizeof(void *));
}

void *hashtable_get(struct hashtable *ht, const char *key)
{
    size_t key_size = strlen(key) + 1;
    return hashtable_get_g(ht, (void *)key, key_size);
}

void *hashtable_get_g(struct hashtable *ht, void *key, size_t key_size)
{
    void **data;
    struct hashbox *box = _hashtable_get_hashbox(ht, key, key_size);
    if (box) {
        data = (void **)(box->key_value_pair + key_size);
        return data ? *data : 0;
    }
    return 0;
}

int hashtable_get_int(struct hashtable *ht, const char *key)
{
    size_t key_size = strlen(key) + 1;
    int *data = 0;
    struct hashbox *box = _hashtable_get_hashbox(ht, (void *)key, key_size);
    if (box) {
        data = (int *)(box->key_value_pair + key_size);
        return data ? *data : 0;
    }
    return 0;
}

bool hashtable_in_g(struct hashtable *ht, void *key, size_t key_size)
{
    struct hashbox *box = _hashtable_get_hashbox(ht, key, key_size);
    return box != 0;
}

bool hashtable_in_p(struct hashtable *ht, void *key)
{
    return hashtable_in_g(ht, (void *)&key, sizeof(void *));
}

bool hashtable_in(struct hashtable *ht, const char *key)
{
    size_t key_size = strlen(key) + 1;
    return hashtable_in_g(ht, (void *)key, key_size);
}

size_t hashtable_size(struct hashtable *ht)
{
    return ht->size;
}

void hashtable_deinit(struct hashtable *ht)
{
    hashtable_clear(ht);
    free(ht->heads);
}
