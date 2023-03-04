/*
 * array.h
 * 
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * dynamic array c header file
 */
#ifndef __CLIB_ARRAY_H__
#define __CLIB_ARRAY_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "clib/generic.h"
#include "clib/object.h"
#include <stddef.h>

struct array {
    object base;
    size_t cap;
    size_t _element_size; //element size of the array
    free_fun fun_free;
};

typedef int (*cmp_fn_t) (const void *, const void *);

struct array *array_new(size_t element_size);
void array_init(struct array *arr, size_t element_size);
void array_init_free(struct array *arr, size_t element_size, free_fun free_fun);
void array_grow(struct array *arr);
void array_deinit(struct array *arr);
void array_push(struct array *arr, void *element);
void array_insert_at(struct array *arr, void *element, size_t index);
void *array_pop(struct array *arr);
void array_set(struct array *arr, size_t index, void *element);
void array_copy(struct array *dest, struct array *src);
void array_copy_size(struct array *dest, struct array *src, size_t size);
void *array_get(struct array *arr, size_t index);
void *array_data(struct array *arr);
void *array_back(struct array *arr);
void *array_front(struct array *arr);
size_t array_size(struct array *arr);
void array_free(struct array *arr);
void array_add(struct array *dest, struct array *src);
void array_clear(struct array *arr);
/*reset the size to zero, without freeing memory*/
void array_reset(struct array *a);
void array_sort(struct array *a, cmp_fn_t compare);

void array_insert_ptr_at(struct array *arr, void *element, size_t index);
void array_push_ptr(struct array *arr, void *element);
void *array_get_ptr(struct array *arr, size_t index);
void *array_back_ptr(struct array *arr);
void *array_front_ptr(struct array *arr);

#define ARRAY(var, elem_type, elem_deinit_fun) \
    struct array var;                          \
    array_init_free(&var, sizeof(elem_type), elem_deinit_fun)

#define ARRAY_VALUE(var, elem_type) \
    struct array var;               \
    array_init_free(&var, sizeof(elem_type), 0)

#define array_t(type_name, postfix) \
type_name array_get_##postfix(struct array *arr, size_t index); \
type_name array_back_##postfix(struct array *arr);\
type_name array_front_##postfix(struct array *arr);\
type_name array_pop_##postfix(struct array *arr);\
void array_push_##postfix(struct array *arr, type_name element);\

array_t(u32, u32)
array_t(void *, p)


#ifdef __cplusplus
}
#endif

#endif
