
/*
 * generic.h
 * 
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * generic pointer value header file
 */
#ifndef __CLIB_GENERIC_H__
#define __CLIB_GENERIC_H__

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*free_fun)(void *);
typedef void (*copy_fun)(void *dest, void *src, size_t size);

void generic_free(void *p);
void generic_copy(void *dest, void *src, size_t size);

#ifdef __cplusplus
}
#endif

#endif
