/*
 * hash.c
 * 
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * hash functions in C
 */

#include <stddef.h>
#include "clib/hash.h"
/*
 * the hash function from Donald E. Knuth, The Art of Computer Proramming Vol 3.
 * 
 */

unsigned int hash(void* data, size_t len)
{
    const char *str = data;
   unsigned int hash = len;
   unsigned int i    = 0;
   for (i = 0; i < len; ++str, ++i){
      hash = ((hash << 5) ^ (hash >> 27)) ^ (*str);
   }
   return hash;
}

void *hashbox_get_key(hashbox* box)
{
    return box->key_value_pair;
}

void *hashbox_get_value(hashbox *box)
{
    return box->key_value_pair + box->key_size;
}

