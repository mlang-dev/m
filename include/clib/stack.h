/*
 * struct stack.h
 *
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * dynamic struct stack c header file
 */
#ifndef __CLIB_STACK_H__
#define __CLIB_STACK_H__

#include <stddef.h>

#include "array.h"

#ifdef __cplusplus
extern "C" {
#endif

struct stack {
    struct array items;
    size_t top; // pointing to the place where element is to be read from
};

void stack_init(struct stack *s, size_t element_size);
void stack_deinit(struct stack *s);
void stack_push(struct stack *s, void *element);
void *stack_pop(struct stack *s);
void *stack_top(struct stack *s);
void *stack_pop_ptr(struct stack *s);
void *stack_top_ptr(struct stack *s);
size_t stack_size(struct stack *s);

#ifdef __cplusplus
}
#endif

#endif
