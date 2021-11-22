/*
 * string.c
 * 
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * dynamic string in C
 */
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "clib/array.h"
#include "clib/object.h"
#include "clib/string.h"
#include "clib/util.h"

void _init_str(string *str)
{
    object_interface string_interface = {
        string_eq_generic, string_init_generic,
        string_deinit_generic, string_data_generic
    };
    register_object_interface(STRING, string_interface);
    str->base.type = STRING;
    str->base.data.p_data = 0;
    str->base.size = 0;
    str->cap = SSO_LENGTH;
}

string *string_new(const char *chars)
{
    string *str;
    MALLOC(str, sizeof(*str));
    string_init_chars(str, chars);
    return str;
}


string *string_new2(const char *chars, size_t str_len)
{
    string *str;
    MALLOC(str, sizeof(*str));
    string_init_chars2(str, chars, str_len);
    return str;
}

string make_string(const char *chars)
{
    string str;
    string_init_chars(&str, chars);
    return str;
}

char *string_get(string *str)
{
    return str->cap <= SSO_LENGTH ? str->_reserved : str->base.data.p_data;
}

char *to_c_str(string *str)
{
    size_t str_len = string_size(str);
    char *p;
    MALLOC(p, (sizeof(*p) * (str_len + 1)));
    p[str_len] = 0;
    strncpy(p, str->cap <= SSO_LENGTH ? str->_reserved : str->base.data.p_data, str_len);
    return p;
}

size_t string_size(string *str)
{
    return str->base.size;
}

void string_init(string *str)
{
    string_init_chars(str, "");
}

void string_init_chars(string *str, const char *chars)
{
    _init_str(str);
    string_copy_chars(str, chars);
}

void string_init_chars2(string *str, const char *chars, size_t str_len)
{
    _init_str(str);
    string_copy_chars2(str, chars, str_len);
}

void string_copy_with_len(string *dest, const char *data, size_t len)
{
    if (len >= dest->cap) {
        void* p;
        MALLOC(p, len+1);
        dest->base.data.p_data = p;
        dest->cap = len + 1;
    }
    char *dest_data = string_get(dest);
    memcpy(dest_data, data, len);
    dest_data[len] = '\0';
    dest->base.size = len;
}

void string_copy_with_len2(string *dest, const char *data, size_t len)
{
    if (len >= dest->cap) {
        void* p;
        MALLOC(p, len);
        dest->base.data.p_data = p;
        dest->cap = len;
    }
    char *dest_data = string_get(dest);
    memcpy(dest_data, data, len);
    dest->base.size = len;
}

void string_copy_chars(string *dest, const char *chars)
{
    string_copy_with_len(dest, chars, strlen(chars));
}

void string_copy_chars2(string *dest, const char *chars, size_t str_len)
{
    string_copy_with_len2(dest, chars, str_len);
}

void string_copy(string *dest, string *src)
{
    string_init(dest);
    string_copy_with_len(dest, string_get(src), src->base.size);
}

void string_add(string *str1, string *str2)
{
    if (!str2 || !string_size(str2))
        return;
    //printf("string sizes: %zu, %zu\n", string_size(str1), string_size(str2));
    assert(string_size(str1) < 1000 && string_size(str2) < 1000);
    size_t len = str1->base.size + str2->base.size;
    if (len > SSO_LENGTH - 1) {
        //allocate in dynamic struct array
        char *data;
        if (str1->cap > SSO_LENGTH) {
            //allocated in heap already
            REALLOC(data, str1->base.data.p_data, len + 1);
            memcpy(data + str1->base.size, string_get(str2), str2->base.size + 1);
        } else {
            //previously in reserved
            MALLOC(data, len+1);
            memcpy(data, str1->_reserved, str1->base.size + 1);
            memcpy(data + str1->base.size, string_get(str2), str2->base.size + 1);
        }
        str1->base.data.p_data = data;
        str1->cap = len + 1;
    } else {
        char *dst = (str1->cap > SSO_LENGTH) ? (char *)str1->base.data.p_data : str1->_reserved;
        memcpy(dst + str1->base.size, string_get(str2), str2->base.size + 1);
    }
    str1->base.size = len;
}

void string_add2(string *str1, string *str2)
{
    if (!str2 || !string_size(str2))
        return;
    //printf("string sizes: %zu, %zu\n", string_size(str1), string_size(str2));
    assert(string_size(str1) < 1000 && string_size(str2) < 1000);
    size_t len = str1->base.size + str2->base.size;
    if (len > SSO_LENGTH) {
        //allocate in dynamic struct array
        char *data;
        if (str1->cap > SSO_LENGTH) {
            //allocated in heap already
            REALLOC(data, str1->base.data.p_data, len);
            memcpy(data + str1->base.size, string_get(str2), str2->base.size);
        } else {
            //previously in reserved
            MALLOC(data, len);
            memcpy(data, str1->_reserved, str1->base.size);
            memcpy(data + str1->base.size, string_get(str2), str2->base.size);
        }
        str1->base.data.p_data = data;
        str1->cap = len;
    } else {
        char *dst = (str1->cap > SSO_LENGTH) ? (char *)str1->base.data.p_data : str1->_reserved;
        memcpy(dst + str1->base.size, string_get(str2), str2->base.size);
    }
    str1->base.size = len;
}

void string_add_chars(string *str1, const char *chars)
{
    string str2;
    string_init_chars(&str2, chars);
    string_add(str1, &str2);
    string_deinit(&str2);
}

void string_add_chars2(string *str1, const char *str, size_t str_len)
{
    string str2;
    string_init_chars2(&str2, str, str_len);
    string_add2(str1, &str2);
    string_deinit(&str2);
}

void string_append(string *str1, const char *str)
{
    string str2;
    string_init_chars2(&str2, str, strlen(str));
    string_add2(str1, &str2);
    string_deinit(&str2);
}

bool string_eq(string *str1, string *str2)
{
    if (str1->base.size != str2->base.size) {
        return false;
    }
    return strcmp(string_get(str1), string_get(str2)) == 0;
}

bool string_eq_chars(string *str1, const char *chars)
{
    string str2;
    string_init_chars(&str2, chars);
    bool result = string_eq(str1, &str2);
    string_deinit(&str2);
    return result;
}

void string_deinit(string *str)
{
    if (str->base.data.p_data)
        FREE(str->base.data.p_data);
}

string *string_substr(string *str, char match)
{
    char *data = string_get(str);
    for (int i = (int)str->base.size - 1; i >= 0; i--) {
        if (data[i] == match) {
            data[i] = '\0';
            str->base.size = i;
            return str;
        }
    }
    return 0;
}

string string_join(struct array *arr, const char *sep)
{
    string str_sepa;
    string_init_chars(&str_sepa, sep);
    string str;
    string_init(&str);
    for (size_t i = 0; i < array_size(arr); i++) {
        string_add(&str, (string *)array_get(arr, i));
        if (i < array_size(arr) - 1)
            string_add(&str, &str_sepa);
    }
    string_deinit(&str_sepa);
    return str;
}

struct array string_split(string *str, char sep)
{
    ARRAY_STRING(arr);
    string sub_str;
    string_init(&sub_str);
    size_t collect_start = 0;
    char *data = string_get(str);
    for (size_t i = 0; i < str->base.size; i++) {
        if (data[i] == sep || i == str->base.size - 1) {
            size_t sub_str_len = data[i] == sep ? i - collect_start : i - collect_start + 1;
            string_copy_with_len(&sub_str, &data[collect_start], sub_str_len);
            array_push(&arr, &sub_str); /*sub_str allocated the heap will be owned by struct array*/
            collect_start = i + 1;
            string_init(&sub_str);
        }
    }
    return arr;
}

void string_free(string *str)
{
    string_deinit(str);
    FREE(str);
}

char string_back(string *str)
{
    if (str->base.size == 0)
        return '\0';
    char *data = string_get(str);
    return data[str->base.size - 1];
}

char string_pop(string *str)
{
    if (str->base.size == 0)
        return '\0';
    char *data = string_get(str);
    char back = data[str->base.size - 1];
    data[str->base.size - 1] = '\0';
    str->base.size--;
    return back;
}

void string_push(string *str, char ch)
{
    char temp[2];
    temp[0] = ch;
    temp[1] = '\0';
    string_add_chars(str, temp);
}

//generic interfaces
void string_init_generic(object *dest, object *src)
{
    string_copy((string *)dest, (string *)src);
}

void string_copy_generic(void *dest, void *src, size_t size)
{
    if (!size)
        return;
    string_copy((string *)dest, (string *)src);
}

void string_deinit_generic(object *dest)
{
    string_deinit((string *)dest);
}

void string_free_generic(void *dest)
{
    string_deinit((string *)dest);
}

bool string_eq_generic(object *str1, object *str2)
{
    assert(str1->type == STRING && str2->type == STRING);
    return string_eq((string *)str1, (string *)str2);
}

void *string_data_generic(object *obj)
{
    return string_get((string *)obj);
}

bool is_upper(string *str)
{
    const char *pstr = string_get(str);
    for(size_t i=0; i<string_size(str); i++){
        if(!isupper(pstr[i]))
            return false;
    }
    return true;
}