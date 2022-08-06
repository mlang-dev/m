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

void _ba_grow_to(struct byte_array *ba, u32 new_cap)
{
    ba->cap = new_cap;
    void *data;
    REALLOC(data, ba->data, ba->cap);
    ba->data = data;
}

void _ba_grow(struct byte_array *ba)
{
    _ba_grow_to(ba, ba->cap * 1.5);
}

void ba_add(struct byte_array *ba, u8 byte)
{
    if (ba->size == ba->cap) {
        _ba_grow(ba);
    }
    ba->data[ba->size++] = byte;
}

void ba_add2(struct byte_array *dst, struct byte_array *src)
{
    if (dst->cap < dst->size + src->size) {
        _ba_grow_to(dst, dst->size + src->size);
    }
    memcpy(&dst->data[dst->size], src->data, src->size);
    dst->size += src->size;
}

void ba_set(struct byte_array *ba, u32 index, u8 byte)
{
    if (index > ba->size - 1)
        return;
    ba->data[index] = byte;
}

void ba_reset(struct byte_array *ba)
{
    ba->size = 0;
}

void ba_deinit(struct byte_array *ba)
{
    FREE(ba->data);
    ba->size = ba->cap = 0;
    ba->data = 0;
}
