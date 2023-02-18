/*
 * string.h
 * 
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * dynamic string c header file
 */
#ifndef __CLIB_STRING_H__
#define __CLIB_STRING_H__

#include <stdbool.h>
#include <stddef.h>

#include "clib/array.h"
#include "clib/object.h"

#ifdef __cplusplus
extern "C" {
#endif

#define SSO_LENGTH 19

#ifndef _WIN32
#define sprintf_s(dst, dst_size, format, ...)  sprintf(dst, format, __VA_ARGS__)
#define vsprintf_s(dst, dst_size, format, ...) vsprintf(dst, format, __VA_ARGS__)
#define strcpy_s(dst, dst_size, src) strcpy(dst, src)
#define strcat_s(dst, dst_size, src) strcat(dst, src)
#define strerror_s(errmsg, errmsg_size, errnum) strerror(errnum)
#define _strdup(src) strdup(src)
#endif

typedef struct {
    object base;
    size_t cap;
    char _reserved[SSO_LENGTH]; //reserved for short string optimization
} string;

#define STRING_POINTER(vp) ((string *)vp)

string *string_new(const char *chars);
string *string_new2(const char *chars, size_t len);
string make_string(const char *chars);
void string_init(string *str);
void string_init_chars(string *str, const char *chars);
void string_init_chars2(string *str, const char *chars, size_t str_len);
void string_copy_chars(string *str, const char *chars);
void string_copy_with_len(string *str, const char *data, size_t len);
void string_copy(string *dest, string *src);
bool string_eq_chars(string *str1, const char *chars);
bool string_eq(string *str1, string *str2);
void string_add(string *str1, string *str2);
void string_add_chars(string *str1, const char *chars);
void string_append(string *str1, const char *chars);
void string_add_chars2(string *str1, const char *chars, size_t str_len);
string string_join(struct array *arr, const char *sep);
struct array string_split(string *str, char sep);
string *string_substr(string *str, char match);
void string_deinit(string *str);
void string_free(string *str);
char string_back(string *str);
char string_pop(string *str); // from back
void string_push(string *str, char ch); //push to back
char *string_get(string *str);
char *string_get_owned(string *str);
const char *string_cstr(string *str);
size_t string_size(string *str);
//generic interface
void string_init_generic(object *dest, object *src);
void string_deinit_generic(object *dest);
bool string_eq_generic(object *str1, object *str2);
void *string_data_generic(object *obj);

void string_copy_generic(void *dest, void *src, size_t size);
void string_free_generic(void *dest);
bool is_upper(string *str);
char *str_clone (const char *s);
string* string_replace(const char *src, size_t src_size, char match, const char *replaced_with);

#define ARRAY_STRING(var) ARRAY(var, string, string_free_generic)

#ifdef __cplusplus
}
#endif

#endif
