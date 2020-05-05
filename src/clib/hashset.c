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

void hashset_init(hashset *hs)
{
    hashtable_init(hs);
}

void hashset_set(hashset *hs, const char *key_data)
{
    hashtable_set(hs, key_data, 0);
}

size_t hashset_size(hashset *hs)
{
    return hashtable_size(hs);
}

bool hashset_in(hashset *hs, const char *key_data)
{
    return hashtable_in(hs, key_data);
}

void hashset_deinit(hashset *hs)
{
    hashtable_deinit(hs);
}
