/*
 * object.h
 * 
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * a variant object c header file
 */
#ifndef __CLIB_OBJECT_H__
#define __CLIB_OBJECT_H__

#include <stdbool.h>
#include <stddef.h>
#include "clib/typedef.h"

#ifdef __cplusplus
extern "C" {
#endif

enum ctype {
    //value type
    VALUE_TYPE = 0,
    // CHAR,
    // INT,
    // FLOAT,
    // DOUBLE,

    REFERENCE_TYPE = 64, //5
    //reference type or value type (depending on how long of the string)
    STRING,
    POINTER,
    ALL
};

typedef struct _object {
    enum ctype type;
    size_t size; //size of data, not the struct size
    union {
        char c_data;
        int i_data;
        f32 f_data;
        f64 d_data;
        void *p_data;
    } data;
} object;

typedef bool (*object_eq)(object *dest, object *src);
typedef void (*object_init)(object *dest, object *src);
typedef void (*object_deinit)(object *obj);
typedef void *(*object_data)(object *obj);

typedef struct _object_interface {
    object_eq eq;
    object_init init;
    object_deinit deinit;
    object_data data;
} object_interface;

void register_object_interface(enum ctype type, object_interface interface);
object_eq get_eq(enum ctype type);
object_init get_init(enum ctype type);
object_deinit get_deinit(enum ctype type);
object_data get_data(enum ctype type);

#ifdef __cplusplus
}
#endif

#endif
