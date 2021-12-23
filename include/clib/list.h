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
extern "C" {
#endif

#define list_head(name, type) \
    struct name {             \
        struct type *first;   \
    }

#define list_entry(type)   \
    struct {               \
        struct type *next; \
    }

#define list_insert_head(head, element, field) \
    do {                                       \
        (element)->field.next = (head)->first; \
        (head)->first = (element);             \
    } while (0)

#define list_remove_head(head, field)              \
    do {                                           \
        (head)->first = (head)->first->field.next; \
    } while (0)

#define list_first(head) ((head)->first)
#define list_end(head) 0
#define list_empty(head) (list_first(head) == list_end(head))
#define list_remove_next(elem, field)                        \
    do {                                                     \
        (elem)->field.next = (elem)->field.next->field.next; \
    } while (0)
#define list_next(element, field) ((element)->field.next)

#define list_foreach(var, head, field) \
    for ((var) = list_first(head);     \
         (var) != list_end(head);      \
         (var) = list_next(var, field))

/*link list interfaces*/
#define link_list(list_struct_name, entry_struct_name, data_type) \
    struct entry_struct_name {\
        list_entry(entry_struct_name) list;\
        data_type data;\
    };\
    list_head(list_struct_name, entry_struct_name);

#define link_list_insert_head(head, element) list_insert_head(head, element, list)

#define link_list_add_data_fn(ll_struct_name, entry_struct_name) \
    void ll_struct_name##_add_data_to_head(struct ll_struct_name *ll, void *data)\
    {\
        struct entry_struct_name *entry;\
        MALLOC(entry, sizeof(*entry));\
        entry->data = data;\
        entry->list.next = 0;\
        link_list_insert_head(ll, entry);\
    }

#define link_list_remove_data_fn(ll_struct_name, entry_struct_name, data_type)\
    data_type ll_struct_name##_remove_data_from_head(struct ll_struct_name *ll)\
    {\
        data_type data = 0;\
        if (!ll) {\
            return 0;\
        }\
        if (ll->first) {\
            struct entry_struct_name *first = ll->first;\
            data = first->data;\
            list_remove_head(ll, list);\
            FREE(first);\
        }\
        return data;\
    }

#ifdef __cplusplus
}
#endif

#endif
