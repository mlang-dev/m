/*
 * struct byte_array.c
 * 
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * dynamic struct byte array in C
 */
#include <assert.h>
#include <stddef.h>
#include <stdio.h>

#include "clib/byte_array.h"
#include "clib/util.h"

void ba_init(struct byte_array *ba, u32 init_size)
{
    ba->cap = init_size;
    ba->size = 0;
    MALLOC(ba->data, init_size);
}

void _ba_grow(struct byte_array *ba)
{
    ba->cap *= 1.5;
    void* data;
    REALLOC(data, ba->data, ba->cap);
    ba->data = data;
}

void ba_add(struct byte_array *ba, u8 byte)
{
    if (ba->size == ba->cap) {
        _ba_grow(ba);
    }
    ba->data[ba->size++] = byte;
}


void ba_set(struct byte_array *ba, u32 index, u8 byte)
{
    if (index > ba->size - 1)
        return;
    ba->data[index] = byte;
}


void ba_deinit(struct byte_array *ba)
{
    FREE(ba->data);
    ba->size = ba->cap = 0;
    ba->data = 0;
}
