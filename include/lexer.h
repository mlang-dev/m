#pragma once
#include <map>
#include <string>
#include "util.h"

#define FOREACH_TOKENTYPE(ENUM_ITEM) \
  ENUM_ITEM(TOKEN_UNK)               \
  ENUM_ITEM(TOKEN_EOF)               \
  ENUM_ITEM(TOKEN_IDENT)             \
  ENUM_ITEM(TOKEN_NUM)               \
  ENUM_ITEM(TOKEN_LET)               \
  ENUM_ITEM(TOKEN_IMPORT)            \
  ENUM_ITEM(TOKEN_IF)                \
  ENUM_ITEM(TOKEN_THEN)              \
  ENUM_ITEM(TOKEN_ELSE)              \
  ENUM_ITEM(TOKEN_FOR)               \
  ENUM_ITEM(TOKEN_IN)                \
  ENUM_ITEM(TOKEN_RANGE)             \
  ENUM_ITEM(TOKEN_UNARY)             \
  ENUM_ITEM(TOKEN_BINARY)            \
  ENUM_ITEM(TOKEN_VAR)               \
  ENUM_ITEM(TOKEN_OP)                \
  ENUM_ITEM(TOKEN_EOS)               \

enum TokenType { FOREACH_TOKENTYPE(GENERATE_ENUM) };

static const char* TokenTypeString[] = {
    FOREACH_TOKENTYPE(GENERATE_ENUM_STRING)};

struct source_loc {
  int line;
  int col;
};

struct token {
  TokenType type;
  source_loc loc;
  union {
    std::string* ident_str;
    double num_val;
    int op_val;
  };
};

token& get_token(FILE* file);
