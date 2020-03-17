#pragma once
#include <string>
#include <vector>
#include "lexer.h"
#include "util.h"
#include "value_type.h"

using namespace std;

#define FOREACH_NODETYPE(ENUM_ITEM) \
  ENUM_ITEM(UNK_NODE)               \
  ENUM_ITEM(NUMBER_NODE)            \
  ENUM_ITEM(IDENT_NODE)             \
  ENUM_ITEM(VAR_NODE)               \
  ENUM_ITEM(UNARY_NODE)             \
  ENUM_ITEM(BINARY_NODE)            \
  ENUM_ITEM(CONDITION_NODE)         \
  ENUM_ITEM(FOR_NODE)               \
  ENUM_ITEM(CALL_NODE)              \
  ENUM_ITEM(PROTOTYPE_NODE)         \
  ENUM_ITEM(FUNCTION_NODE)          \
  ENUM_ITEM(BLOCK_NODE)             \

enum NodeType { FOREACH_NODETYPE(GENERATE_ENUM) };

static const char* NodeTypeString[] = {
    FOREACH_NODETYPE(GENERATE_ENUM_STRING)
};

struct exp_node {
  NodeType node_type;
  ValueType value_type;
  source_loc loc;
  exp_node* parent;
};

struct block_node {
  exp_node base;
  vector<exp_node*> nodes;
};

struct module {
  std::string name;
  block_node* block;
  file_tokenizer* tokenizer;
};

struct ast {
  std::vector<exp_node*> builtins;
  std::vector<module*> modules;
};

struct num_node {
  exp_node base;
  double num_val;
};

struct ident_node {
  exp_node base;
  std::string name;
};

struct var_node {
  exp_node base;
  string var_name;
  exp_node* init_value;
};

struct unary_node {
  exp_node base;
  string op;
  exp_node* operand;
};

struct binary_node {
  exp_node base;
  string op;
  exp_node *lhs, *rhs;
};

struct condition_node {
  exp_node base;
  exp_node *condition_node, *then_node, *else_node;
};

struct for_node {
  exp_node base;
  std::string var_name;
  exp_node *start, *end, *step, *body;
};

struct call_node {
  exp_node base;
  std::string callee;
  std::vector<exp_node*> args;
};

struct prototype_node {
  exp_node base;
  string name;
  string op;
  vector<string> args;
  char is_operator;
  unsigned precedence;
};

struct function_node {
  exp_node base;
  prototype_node* prototype;
  block_node* body;
};

prototype_node* create_prototype_node(exp_node* parent, source_loc loc,
                                      const std::string& name,
                                      std::vector<std::string>& args,
                                      bool is_operator = false,
                                      unsigned precedence = 0,
                                      string op = "");