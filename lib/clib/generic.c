/*
 * generic.c
 * 
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * generic functions in C
 */

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "clib/generic.h"

void generic_free(void *p)
{
    free(p);
}

void generic_copy(void *dest, void *src, size_t size)
{
    memcpy(dest, src, size);
}
