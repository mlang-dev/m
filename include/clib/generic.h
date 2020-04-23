
/*
 * generic.h
 * 
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * generic pointer value header file
 */
#ifndef __CLIB_GENERIC_H__
#define __CLIB_GENERIC_H__

#ifdef __cplusplus
extern "C"{
#endif 

typedef struct _value_ref{
    void* data;
    size_t size;
}value_ref;

typedef void (*free_fun)(void *);
void generic_free(void *p);

#ifdef __cplusplus
}
#endif

#endif