/*
 * byte_array.h
 * 
 * Copyright (C) 2021 Ligang Wang <ligangwangs@gmail.com>
 *
 * dynamic raw byte array c header file
 */
#ifndef __CLIB_BYTE_ARRAY_H__
#define __CLIB_BYTE_ARRAY_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include "clib/typedef.h"

struct byte_array {
    u32 size; //size of the array used
    u8 *data;
    u32 cap;
};

void ba_init(struct byte_array *ba, u32 init_size);
void ba_deinit(struct byte_array *ba);
void ba_reset(struct byte_array *ba);
void ba_add(struct byte_array *ba, u8 byte);
void ba_add_array(struct byte_array *ba, u8 *byte, u32 bytes);
void ba_set(struct byte_array *ba, u32 index, u8 byte);
void ba_add2(struct byte_array *dst, struct byte_array *src);

#ifdef __cplusplus
}
#endif

#endif
