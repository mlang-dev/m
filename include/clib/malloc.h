
/*
 * malloc.h
 * 
 * Copyright (C) 2021 Ligang Wang <ligangwangs@gmail.com>
 *
 * custom made malloc with c header file
 */
#ifndef __CLIB_MALLOC_H__
#define __CLIB_MALLOC_H__

#ifdef __cplusplus
extern "C" {
#endif

void* malloc(size_t bytes);
void free(void* mem);
void* realloc(void* oldmem, size_t bytes);
void* calloc(size_t n_elements, size_t elem_size);

#ifdef __cplusplus
}
#endif

#endif
