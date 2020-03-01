#pragma once
#include <map>
#include <string>
#include "util.h"

using namespace std;
#define FOREACH_TOKENTYPE(ENUM_ITEM) \
  ENUM_ITEM(TOKEN_UNK)               \
  ENUM_ITEM(TOKEN_EOF)               \
  ENUM_ITEM(TOKEN_IDENT)             \
  ENUM_ITEM(TOKEN_NUM)               \
  ENUM_ITEM(TOKEN_IMPORT)            \
  ENUM_ITEM(TOKEN_IF)                \
  ENUM_ITEM(TOKEN_THEN)              \
  ENUM_ITEM(TOKEN_ELSE)              \
  ENUM_ITEM(TOKEN_FOR)               \
  ENUM_ITEM(TOKEN_IN)                \
  ENUM_ITEM(TOKEN_RANGE)             \
  ENUM_ITEM(TOKEN_UNARY)             \
  ENUM_ITEM(TOKEN_BINARY)            \
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
    string* ident_str;
    double num_val;
    int op_val;
  };
};

struct file_tokenizer{
  FILE* file;
  source_loc loc = {1, 0};
  source_loc tok_loc;
  token _token;
  token _next_token = {.type = TOKEN_UNK};
  int curr_char = ' ';
  string ident_str;
};

file_tokenizer* create_tokenizer(FILE* file);
void destroy_tokenizer(file_tokenizer* tokenizer);
token& get_token(file_tokenizer* tokenizer);
void repeat_token(file_tokenizer* tokenizer);
