/*
 * string.h
 * 
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * string c header file
 */
#ifndef __CLIB_STRING_H__
#define __CLIB_STRING_H__

#include <stddef.h>
#include <stdbool.h>

#include "clib/array.h"

#ifdef __cplusplus
extern "C" {
#endif

#define SSO_LENGTH 19

typedef struct{
  size_t size;
  size_t cap;
  char reserved[SSO_LENGTH]; //reserved for short string optimization
  char* data;
}string;

string* string_new(const char *chars);
string* string_new_len(const char *chars, size_t len);
void string_init(string *str, const char *chars);
void string_init_len(string *str, const char *chars, size_t len);
void string_copy(string* str, const char* chars);
bool string_eq(string *str1, const char *chars);
bool string_eqs(string *str1, string *str2);
void string_append(string *str1, string *str2);
void string_add(string *str1, const char *chars);
string string_join(array* arr, char sep);
array string_split(string* str, char sep);
string* substr_until(string *str, char match);
void string_deinit(string *str);
void string_free(string *str);

#ifdef __cplusplus
}
#endif

#endif