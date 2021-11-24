#include <stdint.h>
#include "wasm/wasm.h"
extern size_t __heap_base;
//extern void log(char *str);

static uintptr_t hp = (uintptr_t)&__heap_base;

uintptr_t *get_ptr()
{
    if(hp==0){
        hp = (uintptr_t)&__heap_base;
    }
    return &hp;
}

void *sbrk(intptr_t inc)
{
    uintptr_t increment = (uintptr_t)inc;
    increment = (increment + 3) & ~3;
    uintptr_t *hp_ptr = get_ptr();
    uintptr_t heap_ptr = *hp_ptr;
    uintptr_t new_heap_ptr = heap_ptr + increment;
    uintptr_t current_size = get_mem_size();
    if(new_heap_ptr > current_size){
        grow_mem(new_heap_ptr);
    }
    *hp_ptr = new_heap_ptr;
    return (void*)heap_ptr;
}