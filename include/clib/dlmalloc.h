
/*
 * dlmalloc.h
 * 
 * Copyright (C) 2021 Ligang Wang <ligangwangs@gmail.com>
 *
 * This is header file wrapper for dlmalloc implemented by 
 */
#ifndef __CLIB_MALLOC_H__
#define __CLIB_MALLOC_H__

#ifdef __cplusplus
extern "C" {
#endif

void* dlmalloc(size_t bytes);
void dlfree(void* mem);
void* dlrealloc(void* oldmem, size_t bytes);
void* dlcalloc(size_t n_elements, size_t elem_size);

#ifdef __cplusplus
}
#endif

#endif
