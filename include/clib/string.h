/*
 * string.h
 * 
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * dynamic string c header file
 */
#ifndef __CLIB_STRING_H__
#define __CLIB_STRING_H__

#include <stddef.h>
#include <stdbool.h>

#include "clib/array.h"
#include "clib/object.h"

#ifdef __cplusplus
extern "C" {
#endif

#define SSO_LENGTH 19

typedef struct{
    object base;
    size_t cap;
    char _reserved[SSO_LENGTH]; //reserved for short string optimization
}string;

#define STRING_POINTER(vp) ((string*)vp)

string* string_new(const char *chars);
string* string_new_len(const char *chars, size_t len);
void string_init(string *str);
void string_init_chars(string *str, const char *chars);
void string_copy_chars(string* str, const char* chars);
void string_copy_with_len(string* str, const char* data, size_t len);
void string_copy(string* dest, string *src);
bool string_eq_chars(string *str1, const char *chars);
bool string_eq(string *str1, string *str2);
void string_add(string *str1, string *str2);
void string_add_chars(string *str1, const char *chars);
string string_join(array* arr, char sep);
array string_split(string* str, char sep);
string* string_substr(string *str, char match);
void string_deinit(string *str);
void string_free(string *str);
char string_back(string *str);
char string_pop(string *str); // from back
void string_push(string *str, char ch); //push to back
char * string_get(string *str);
size_t string_size(string *str);
//generic interface
void string_init_generic(void *dest, void *src);
void string_deinit_generic(void *dest);


#ifdef __cplusplus
}
#endif

#endif