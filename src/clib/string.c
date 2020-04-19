/*
 * string.c
 * 
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * dynamic string in C
 */
#include <stdlib.h>
#include <string.h>

#include "clib/string.h"
#include "clib/array.h"

void _init_str(string *str)  
{
    str->_dyn_data = NULL;
    str->size = 0;
    str->cap = SSO_LENGTH;
}

string* string_new(const char* chars)
{
    string* str = (string*)malloc(sizeof(string));
    string_init_chars(str, chars);
    return str;
}

string* string_new_len(const char* chars, size_t len)
{
    string* str = (string*)malloc(sizeof(string));
    string_copy_with_len(str, chars, len);
    return str;
}

char * string_get(string *str){
    return str->cap <= SSO_LENGTH ? str->_reserved : str->_dyn_data; 
}

void string_init(string* str)
{
    string_init_chars(str, "");
}

void string_init_chars(string* str, const char* chars)
{
    _init_str(str);
    string_copy_chars(str, chars);
}

void string_copy_with_len(string* dest, const char* data, size_t len)
{
    if (len >= dest->cap) {
        dest->_dyn_data = (char*)malloc(len + 1);
        dest->cap = len + 1;
    }
    char *dest_data = string_get(dest);
    memcpy(dest_data, data, len);
    dest_data[len] = '\0';
    dest->size = len;
}

void string_copy_chars(string* dest, const char* chars)
{
    string_copy_with_len(dest, chars, strlen(chars));
}

void string_copy(string* dest, string* src)
{
    string_init(dest);
    string_copy_with_len(dest, string_get(src), src->size);
}

void string_add(string *str1, string *str2)
{
    //
    size_t len = str1->size + str2->size;
    if (len>SSO_LENGTH-1){
        //allocate in dynamic array
        char* data;
        if (str1->cap > SSO_LENGTH){
            //allocated in heap already
            data = (char *)realloc(str1->_dyn_data, len + 1);
            memcpy(data + str1->size, string_get(str2), str2->size+1);
        }else{
            //previously in reserved
            data = (char *)malloc(len+1);
            memcpy(data, str1->_reserved, str1->size + 1);
            memcpy(data + str1->size, string_get(str2), str2->size + 1);
        }
        str1->_dyn_data = data;
        str1->cap = len + 1;
    }else{
        //still in reserved area
        memcpy(str1->_reserved + str1->size, string_get(str2), str2->size + 1);
    }
    str1->size = len;
}

void string_add_chars(string *str1, const char *chars)
{
    string str2;
    string_init_chars(&str2, chars);
    string_add(str1, &str2);
    string_deinit(&str2);
}

bool string_eq(string *str1, string *str2)
{
    if (str1->size != str2->size){
        return false;
    }
    return strcmp(string_get(str1), string_get(str2)) == 0;
}

bool string_eq_chars(string* str1, const char* chars)
{
    string str2;
    string_init_chars(&str2, chars);
    bool result = string_eq(str1, &str2);
    string_deinit(&str2);
    return result;
}

void string_deinit(string* str)
{
    if (str->_dyn_data)
        free(str->_dyn_data);
}

string* string_substr(string *str, char match)
{
    char* data = string_get(str);
    for (int i=str->size-1; i>=0; i--){
        if(data[i] == match){
            data[i] = '\0';
            str->size = i;
            return str;
        }
    }
    return 0;
}

string string_join(array* arr, char sep)
{
    char separator[2];
    separator[0] = sep;
    separator[1] = '\0';
    string str_sepa;
    string_init_chars(&str_sepa, separator);
    string str;
    string_init(&str);
    for (int i=0; i<arr->size; i++){
        string_add(&str, (string*)array_get(arr, i));
        if (i < arr->size - 1)
            string_add(&str, &str_sepa);
    }
    string_deinit(&str_sepa);
    return str;
}

array string_split(string* str, char sep)
{
    array arr;
    string_array_init(&arr);
    string sub_str;
    string_init(&sub_str);
    int collect_start = 0;
    char *data = string_get(str);
    for(int i=0;i<str->size;i++){
        if(data[i] == sep||i==str->size-1){
            size_t sub_str_len = data[i] == sep? i-collect_start: i-collect_start + 1;
            string_copy_with_len(&sub_str, &data[collect_start], sub_str_len);
            array_push(&arr, &sub_str);
            collect_start = i+1;
        }
    }
    string_deinit(&sub_str);
    return arr;
}


void string_free(string* str)
{
    string_deinit(str);
    free(str);
}

//generic interfaces
void string_init_generic(void *dest, void *src)
{
  string_copy((string*)dest, (string*)src);
}

void string_deinit_generic(void *dest)
{
  string_deinit((string*)dest);
}

char string_back(string* str)
{
    if (str->size == 0)
        return '\0';
    char *data = string_get(str);
    return data[str->size-1];
}

char string_pop(string* str)
{
    if (str->size == 0)
        return '\0';
    char *data = string_get(str);
    char back = data[str->size-1];
    data[str->size-1] = '\0';
    str->size--;
    return back;
}

void string_push(string *str, char ch)
{
    char temp[2];
    temp[0] = ch;
    temp[1] = '\0';
    string_add_chars(str, temp);
}
