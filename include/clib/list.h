/*
 * list.h
 * 
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * dynamic singly-linked list c header file
 */

#ifndef __CLIB_LIST_H__
#define __CLIB_LIST_H__

#ifdef __cplusplus
extern "C"{
#endif

#define list_head(name, type)                                   \
struct name {                                                   \
    struct type *first;                                         \
}

#define list_entry(type)                                        \
struct {                                                        \
    struct type *next;                                          \
}

#define list_insert_head(head, element, field) do {             \
(element)->field.next = (head)->first;                          \
(head)->first = (element);                                      \
} while (0)

#define list_remove_head(head, field) do {                      \
(head)->first = (head)->first->field.next;                      \
} while (0)

#define	list_first(head)	((head)->first)
#define list_end(head)  NULL
#define list_empty(head) (list_first(head) == list_end(head))
#define list_remove_next(elem, field) do {                      \
    (elem)->field.next = (elem)->field.next->field.next;        \
} while (0)
#define list_next(element, field) ((element)->field.next)

#define	list_foreach(var, head, field)					        \
	for((var) = list_first(head);					            \
	    (var) != list_end(head);					            \
	    (var) = list_next(var, field))


#ifdef __cplusplus
}
#endif


#endif
