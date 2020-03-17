#pragma once
#include <map>
#include "util.h"

using namespace std;

#define FOREACH_TYPE(ENUM_ITEM) \
  ENUM_ITEM(TYPE_UNK)           \
  ENUM_ITEM(TYPE_UNIT)          \
  ENUM_ITEM(TYPE_BOOL)          \
  ENUM_ITEM(TYPE_CHAR)          \
  ENUM_ITEM(TYPE_INT)           \
  ENUM_ITEM(TYPE_DOUBLE)        \
  ENUM_ITEM(TYPE_FUNCTION)      \

enum ValueType { FOREACH_TYPE(GENERATE_ENUM) };

static const char* TypeString[] = {
    FOREACH_TYPE(GENERATE_ENUM_STRING)};
