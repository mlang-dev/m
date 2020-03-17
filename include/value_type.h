#pragma once
#include <map>
#include "util.h"

using namespace std;

#define FOREACH_TYPE(ENUM_ITEM) \
  ENUM_ITEM(TYPE_UNK)           \
  ENUM_ITEM(TYPE_BOOL)          \
  ENUM_ITEM(TYPE_CHAR)          \
  ENUM_ITEM(TYPE_INT)           \
  ENUM_ITEM(TYPE_DOUBLE)        \

enum ValueType { FOREACH_TYPE(GENERATE_ENUM) };

static const char* TypeString[] = {
    FOREACH_TYPE(GENERATE_ENUM_STRING)};

map<string, ValueType> ValueTypes = {
  {"bool", TYPE_BOOL},
  {"char", TYPE_CHAR},
  {"int", TYPE_INT},
  {"double", TYPE_DOUBLE}
};