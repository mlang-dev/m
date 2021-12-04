/*
 * Copyright (C) 2021 Ligang Wang <ligangwangs@gmail.com>
 *
 * This is to wrap up clang's wasm builtin functions
 * 
 */

#include "wasm/sys.h"
#include <assert.h>

#define PAGE_SIZE (1<<16)

size_t get_mem_size() 
{
  return __builtin_wasm_memory_size(0) * PAGE_SIZE;
}

int grow_mem(size_t size)
{
  size_t old_size = get_mem_size();
  assert(old_size < size);
  size_t diff = (size - old_size + PAGE_SIZE - 1) / PAGE_SIZE;
  size_t result = __builtin_wasm_memory_grow(0, diff);
  if (result != (size_t)-1) {
    return 1;
  }
  return 0;
}
