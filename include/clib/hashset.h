/*
 * hashset.h
 * 
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * dynamic hash set c header file
 */
#ifndef __CLIB_HASHSET_H__
#define __CLIB_HASHSET_H__

#include <stddef.h>

#include "object.h"
#include "hashtable.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef hashtable hashset;

void hashset_init(hashset *hs, size_t key_object_size);
void hashset_deinit(hashset *hs);
void hashset_add(hashset *hs, object* key_data);
size_t hashset_size(hashset *hs);
bool hashset_in(hashset* hs, object* key_data);

#ifdef __cplusplus
}
#endif

#endif