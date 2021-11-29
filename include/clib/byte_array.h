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

struct byte_array {
    size_t cap;
    size_t size; //size of the array used
    unsigned char *data;
};

void ba_init(struct byte_array *ba, size_t init_size);
void ba_deinit(struct byte_array *ba);
void ba_add(struct byte_array *ba, unsigned char byte);
void ba_set(struct byte_array *ba, size_t index, unsigned char byte);


#ifdef __cplusplus
}
#endif

#endif
