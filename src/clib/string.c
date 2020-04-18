#include <stdlib.h>
#include <string.h>

#include "clib/string.h"

string* string_new(const char* chars)
{
    string* str = (string*)malloc(sizeof(string));
    string_init(str, chars);
    return str;
}

void string_init(string* str, const char* chars)
{
    size_t len = strlen(chars);
    if (len < SSO_LENGTH) {
        str->data = str->reserved;
    } else {
        str->data = (char*)malloc(len + 1);
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
    if (str1->size != str2->size)
        return false;
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

void string_free(string* str)
{
    string_deinit(str);
    free(str);
}
