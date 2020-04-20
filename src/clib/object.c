/*
 * object.c
 * 
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * a generic variant object type implementation
 */
#include <string.h>
#include "clib/object.h"

bool is_eq(object *dest, object *src)
{
    if (dest->type != src->type||dest->size != src->size)
        return false;
    if (dest->type>REFERENCE_TYPE)
        return memcmp(dest->p_data, src->p_data, dest->size) == 0;
    return memcmp(dest, src, dest->size) == 0;
}

object box_int(int value)
{
    object o;
    o.i_data = value;
    o.size = sizeof(value);
    o.type = INT;
    return o;
}