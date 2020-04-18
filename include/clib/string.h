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

#ifdef __cplusplus
extern "C" {
#endif

#define SSO_LENGTH 19

typedef struct{
  size_t size;
  char reserved[SSO_LENGTH]; //reserved for short string optimization
  char* data;
}string;

string* string_new(const char *chars);
void string_init(string *str, const char *chars);
void string_append(string *str1, string *str2);
void string_add(string *str1, const char *chars);
void string_deinit(string *str);
void string_free(string *str);

#ifdef __cplusplus
}
#endif

#endif