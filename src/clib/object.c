/*
 * object.c
 * 
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * a generic variant object type implementation
 */
#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "clib/object.h"

bool obj_eq(object* dest, object* src);
void obj_init(object* dest, object* src);
void obj_deinit(object* obj);
void* obj_data(object* obj);

object_interface default_object_interface = { obj_eq, obj_init, obj_deinit, obj_data };
object_interface object_interfaces[ALL];

void obj_init(object* dest, object* src)
{
    *dest = *src;
}

void obj_deinit(object* obj)
{
    if (!obj)
        return;
}

void* obj_data(object* obj)
{
    return &obj->c_data;
}

bool obj_eq(object* dest, object* src)
{
    if (dest->type != src->type || dest->size != src->size)
        return false;
    if (dest->type > REFERENCE_TYPE)
        return memcmp(dest->p_data, src->p_data, dest->size) == 0;
    return memcmp(dest, src, dest->size) == 0;
}

void register_object_interface(enum ctype type, object_interface interface)
{
    object_interfaces[type] = interface;
}

object_eq get_eq(enum ctype type)
{
    return object_interfaces[type].eq ? object_interfaces[type].eq : default_object_interface.eq;
}

object_init get_init(enum ctype type)
{
    return object_interfaces[type].init ? object_interfaces[type].init : default_object_interface.init;
}

object_deinit get_deinit(enum ctype type)
{
    return object_interfaces[type].deinit ? object_interfaces[type].deinit : default_object_interface.deinit;
}

object_data get_data(enum ctype type)
{
    return object_interfaces[type].data ? object_interfaces[type].data : default_object_interface.data;
}
