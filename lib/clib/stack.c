/*
 * struct stack.c
 * 
 * Copyright (C) 2021 Ligang Wang <ligangwangs@gmail.com>
 *
 * dynamic struct stack in C
 */
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include "clib/array.h"
#include "clib/stack.h"
#include "clib/string.h"

void stack_init(struct stack* s, size_t element_size)
{
    array_init(&s->items, element_size);
    s->top = 0;
}

void stack_push(struct stack *s, void *element)
{
    if(s->top < array_size(&s->items))
        array_set(&s->items, s->top, element);
    else
        array_push(&s->items, element);
    s->top++;
}

void *stack_pop(struct stack *s)
{
    if (s->top > 0) {
        return array_get(&s->items, --s->top);
    }
    return 0;
}

void stack_deinit(struct stack *q)
{
    array_deinit(&q->items);
}

size_t stack_size(struct stack *s)
{
    return s->top;
}
