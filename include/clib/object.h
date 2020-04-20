/*
 * object.h
 * 
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * a variant object c header file
 */
#ifndef __CLIB_OBJECT_H__
#define __CLIB_OBJECT_H__

#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

enum ctype
{
    //value type
    VALUE_TYPE,
    CHAR,
    INT,
    FLOAT,
    DOUBLE,

    REFERENCE_TYPE,
    //reference type or value type (depending on how long of the string)
    STRING,
};

typedef struct {
    enum ctype type;
    size_t size; //size of data, not the struct size
    union{
        char c_data;
        int i_data;
        float f_data;
        double d_data;
        void *p_data;
    };
}object;

bool is_eq(object *dest, object *src);
object box_int(int value);

#ifdef __cplusplus
}
#endif

#endif