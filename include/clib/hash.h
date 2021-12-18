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

unsigned int hash(unsigned char *str, size_t len);

#define HASH_EMPTY 0
#define HASH_EXIST 1
#define HASH_DELETED 2

struct hashbox {
    unsigned int status : 2; //00: empty  01: occupied  10: deleted with open addressing hash
    unsigned int key_store_size : 15;  //key store size could be 1 more byte than key_size for null terminated string as key
    unsigned int value_size : 15;
    unsigned char *key_value_pair;
};

void *hashbox_get_key(struct hashbox *box);
void *hashbox_get_value(struct hashbox *box);

#ifdef __cplusplus
}
#endif

#endif
