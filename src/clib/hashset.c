/*
 * hashset.c
 * 
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * dynamic hashset in C
 */
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "clib/array.h"
#include "clib/string.h"
#include "clib/hashset.h"
#include "clib/hash.h"

void hashset_init(hashset *hs, size_t key_object_size)
{
    hashtable_init_free(hs, key_object_size, 0, generic_free, NULL);
}

void hashset_init_free(hashset *hs, size_t key_object_size, free_fun key_free)
{
    hashtable_init_free(hs, key_object_size, 0, key_free, NULL);
}

void hashset_add(hashset *hs, void *key_data)
{
    hashtable_add(hs, key_data, NULL);
}

size_t hashset_size(hashset *hs)
{
    return hashtable_size(hs);
}

bool hashset_in(hashset *hs, void* key_data)
{
    return hashtable_in(hs, key_data);
}

void hashset_deinit(hashset *hs)
{
    hashtable_deinit(hs);
}
