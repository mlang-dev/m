#include <stdlib.h>
#include <string.h>

#include "clib/string.h"

string* string_new(const char* chars)
{
    string* str = (string*)malloc(sizeof(string));
    string_init(str, chars);
    return str;
}

string* string_new_len(const char* chars, size_t len)
{
    string* str = (string*)malloc(sizeof(string));
    string_init_len(str, chars, len);
    return str;
}

void string_init(string* str, const char* chars)
{
    size_t len = strlen(chars);
    string_init_len(str, chars, len);
    str->data[len] = '\0';
}

void string_init_len(string *str, const char* chars, size_t len)
{
    if (len < SSO_LENGTH) {
        str->data = str->reserved;
        str->cap = SSO_LENGTH;
    } else {
        str->data = (char*)malloc(len + 1);
        str->cap = len + 1;
    }
    memcpy(str->data, chars, len);
    str->size = len;
}

void string_copy(string* str, const char* chars)
{
    size_t len = strlen(chars);
    if (len >= str->cap) {
        str->data = (char*)malloc(len + 1);
        str->cap = len + 1;
    }
    memcpy(str->data, chars, len + 1);
    str->size = len;
}

void string_append(string *str1, string *str2)
{
    //
    size_t len = str1->size + str2->size;
    if (len>SSO_LENGTH-1){
        //allocate
        char* data;
        if (str1->size > SSO_LENGTH - 1){
            //allocated in heap already
            data = (char *)realloc(str1->data, len + 1);
            memcpy(data + str1->size, str2->data, str2->size+1);
        }else{
            //previously in reserved
            data = (char *)malloc(len+1);
            memcpy(data, str1->data, str1->size + 1);
            memcpy(data + str1->size, str2->data, str2->size + 1);
        }
        str1->data = data;
    }else{
        //still in reserved area
        memcpy(str1->data + str1->size, str2->data, str2->size + 1);
    }
    str1->size = len;
}

void string_add(string *str1, const char *chars)
{
    string str2;
    string_init(&str2, chars);
    string_append(str1, &str2);
    string_deinit(&str2);
}

bool string_eqs(string *str1, string *str2)
{
    if (str1->size != str2->size){
        return false;
    }
    return strcmp(str1->data, str2->data) == 0;
}

bool string_eq(string* str1, const char* chars)
{
    string str2;
    string_init(&str2, chars);
    bool result = string_eqs(str1, &str2);
    string_deinit(&str2);
    return result;
}

void string_deinit(string* str)
{
    if (str->data != str->reserved)
        free(str->data);
}

string* substr_until(string *str, char match)
{
    for (int i=str->size-1; i>=0; i--){
        if(str->data[i] == match){
            str->data[i] = '\0';
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
    string strsep;
    string_init(&strsep, separator);
    string str;
    string_init(&str, "");
    for (int i=0; i<arr->size; i++){
        string_append(&str, (string*)&arr->data[i]);
        if (i < arr->size - 1)
            string_append(&str, &strsep);
    }
    string_deinit(&strsep);
    return str;
}

array string_split(string* str, char sep)
{
    array arr;
    array_init(&arr, sizeof(string));
    int collect_start = 0;
    for(int i=0;i<str->size;i++){
        if(str->data[i] == sep){
            string sub_str;
            string_init_len(&sub_str, &str->data[collect_start], i-collect_start);
            collect_start = i+1;
            array_append(&arr, &sub_str);
        }
    }
    return arr;
}


void string_free(string* str)
{
    string_deinit(str);
    free(str);
}
