
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
typedef void (*copy_fun)(void *dest, void *src, size_t size);

typedef struct _fun{
    copy_fun copy;
    free_fun free;
}fun;

void generic_free(void *p);
void generic_copy(void *dest, void *src, size_t size);

extern fun default_fun;
extern fun value_fun;

#ifdef __cplusplus
}
#endif

#endif