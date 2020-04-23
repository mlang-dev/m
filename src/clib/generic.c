/*
 * generic.c
 * 
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * generic functions in C
 */

#include <stddef.h>
#include <stdlib.h>

void generic_free(void *p)
{
    free(p);
}
