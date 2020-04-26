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
#include "generic.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct hashtable hashset;

void hashset_init(hashset *hs);
void hashset_deinit(hashset *hs);
void hashset_set(hashset *hs, void *key_data);
size_t hashset_size(hashset *hs);
bool hashset_in(hashset* hs, void *key_data);

#ifdef __cplusplus
}
#endif

#endif
