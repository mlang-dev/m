/*
 * hash.h
 * 
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * hash function c header file
 */
#ifndef __CLIB_HASH_H__
#define __CLIB_HASH_H__

#include "clib/object.h"

#ifdef __cplusplus
extern "C" {
#endif

unsigned int hash(const char* str, unsigned int len);

#define HASH_EMPTY      0
#define HASH_EXIST      1
#define HASH_DELETED    2

typedef struct
{
    object* key_data;
    object* value_data;
    unsigned int status;    //00: empty  01: occupied  10: deleted with open addressing hash 
}hashable;


#ifdef __cplusplus
}
#endif

#endif