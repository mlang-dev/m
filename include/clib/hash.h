/*
 * hash.h
 * 
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * hash function c header file
 */
#ifndef __CLIB_HASH_H__
#define __CLIB_HASH_H__

#include "generic.h"

#ifdef __cplusplus
extern "C" {
#endif


unsigned int hash(void* str, size_t len);

#define HASH_EMPTY      0
#define HASH_EXIST      1
#define HASH_DELETED    2

typedef struct _hash_box
{
    unsigned int status;    //00: empty  01: occupied  10: deleted with open addressing hash 
    void *key;
    void *value;
}hash_box;

#ifdef __cplusplus
}
#endif

#endif