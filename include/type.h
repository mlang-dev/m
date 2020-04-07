#pragma once
#include <map>
#include <vector>
#include "util.h"


#define FOREACH_TYPE(ENUM_ITEM) \
  ENUM_ITEM(TYPE_UNK)           \
  ENUM_ITEM(TYPE_ANY)           \
  ENUM_ITEM(TYPE_UNIT)          \
  ENUM_ITEM(TYPE_BOOL)          \
  ENUM_ITEM(TYPE_CHAR)          \
  ENUM_ITEM(TYPE_INT)           \
  ENUM_ITEM(TYPE_DOUBLE)        \
  ENUM_ITEM(TYPE_FUNCTION)      \

enum Type { FOREACH_TYPE(GENERATE_ENUM) };

static const char* const TypeString[] = {
  "unkown",
  "any",
  "()",
  "bool",
  "char",
  "int",
  "double",
  "->"
};

#define FOREACH_KIND(ENUM_ITEM) \
  ENUM_ITEM(KIND_UNK)           \
  ENUM_ITEM(KIND_VAR)           \
  ENUM_ITEM(KIND_OPER)          \

enum Kind { FOREACH_KIND(GENERATE_ENUM) };

static const char* KindString[] = {
  FOREACH_KIND(GENERATE_ENUM_STRING)
};


//type variable or operator
struct type_exp{
  Kind kind; //type variable or type operator
  string name;  //name of type exp: like "->" for function, "bool", "int", "double" for type variable
};

struct type_var{
  type_exp base;
  type_exp* instance;
};

struct type_oper{
  type_exp base;
  vector<type_exp*> args; //type composition like function or tuple, record
};

type_var* create_type_var();
type_oper* create_type_oper(string name, vector<type_exp*>& args);
type_oper* create_type_fun(vector<type_exp*>& args, type_exp* ret);
void destroy_type_exp(type_exp* type);
bool occurs_in_type(type_exp* type1, type_exp* type2);
type_exp* retrieve(string name, vector<type_exp*>& nogen, map<string, type_exp*>& env);
string format_type(type_exp* exp);
bool unify(type_exp* type1, type_exp* type2, vector<type_exp*>& nogens);