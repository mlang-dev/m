/*
 * hashset.h
 * 
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * dynamic hash set c header file
 */
#ifndef __CLIB_QUEUE_H__
#define __CLIB_QUEUE_H__

#include <stddef.h>

#include "array.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    array items;
} hashset;

void hashset_init(hashset *hs, size_t element_size);
void hashset_deinit(hashset *hs);
void hashset_add(hashset *hs, void* element);
size_t hashset_size(hashset *hs);
bool hashset_in(hashset* hs, void* element);

#ifdef __cplusplus
}
#endif

#endif