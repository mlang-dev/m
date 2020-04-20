/*
 * object.c
 * 
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * a generic variant object type implementation
 */
#include <string.h>
#include "clib/object.h"

bool is_eq(object *dest, object *src);

eq_predicate eq_predicates[ALL] = {
    is_eq,
    is_eq,
    is_eq,
    is_eq,
    is_eq,
    is_eq,
    is_eq,
};

bool is_eq(object *dest, object *src)
{
    if (dest->type != src->type||dest->size != src->size)
        return false;
    if (dest->type>REFERENCE_TYPE)
        return memcmp(dest->p_data, src->p_data, dest->size) == 0;
    return memcmp(dest, src, dest->size) == 0;
}

void register_eq_predicate(enum ctype type, eq_predicate eq)
{
    eq_predicates[type] = eq;
}

eq_predicate get_eq(enum ctype type)
{
    return eq_predicates[type];
}

object make_int(int value)
{
    object o;
    o.i_data = value;
    o.size = sizeof(value);
    o.type = INT;
    return o;
}