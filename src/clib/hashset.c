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

void hashset_init(hashset *hs, size_t element_size)
{
    array_init(&hs->items, element_size);
}

void hashset_add(hashset *hs, void* element)
{
    array_push(&hs->items, element);
}

size_t hashset_size(hashset *hs)
{
    return 0;
}

bool hashset_in(hashset *hs, void* element)
{
    return false;
}

void hashset_deinit(hashset *hs)
{
    array_deinit(&hs->items);
}
