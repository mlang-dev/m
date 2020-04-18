#include <string.h>
#include <stdlib.h>

#include "clib/string.h"

string *string_new(const char *chars)
{
  string *str = (string *)malloc(sizeof(string));
  string_init(str, chars);
  return str;
}

void string_init(string *str, const char *chars)
{
  size_t len = strlen(chars);
  if (len<SSO_LENGTH){
    memcpy(str->reserved, chars, len+1);
    str->data = str->reserved;
  }else{
    str->data = (char*)malloc(len+1);
  }
  str->size = len;
}

void string_deinit(string *str)
{
  if(str->data!=str->reserved)
    free(str->data);
}

void string_free(string *str)
{
  string_deinit(str);
  free(str);
}
