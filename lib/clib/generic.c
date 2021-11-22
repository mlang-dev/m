/*
 * generic.c
 * 
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * generic functions in C
 */

#include <stddef.h>
#include <string.h>

#include "clib/generic.h"
#include "clib/util.h"

void generic_free(void *p)
{
    FREE(p);
}

void generic_copy(void *dest, void *src, size_t size)
{
    memcpy(dest, src, size);
}
