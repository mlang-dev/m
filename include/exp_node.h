#pragma once
#include <string>
#include <vector>
#include "lexer.h"
#include "util.h"

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

typedef struct exp_node {
  NodeType type;
  source_loc loc;
  exp_node* parent;
} exp_node;

typedef struct block_node {
  exp_node base;
  vector<exp_node*> nodes;
} block_node;

typedef struct module {
  std::string name;
  block_node* block;
} module;

typedef struct ast {
  std::vector<exp_node*> builtins;
  std::vector<module*> modules;
  module* entry_module;
} ast;

typedef struct num_node {
  exp_node base;
  double num_val;
} num_node;

typedef struct ident_node {
  exp_node base;
  std::string name;
} ident_node;

typedef struct var_node {
  exp_node base;
  std::vector<std::pair<std::string, exp_node*> > var_names;
  exp_node* body;
} var_node;

typedef struct unary_node {
  exp_node base;
  char op;
  exp_node* operand;
} unary_node;

typedef struct binary_node {
  exp_node base;
  char op;
  exp_node *lhs, *rhs;
} binary_node;

typedef struct condition_node {
  exp_node base;
  exp_node *condition_node, *then_node, *else_node;
} condition_node;

typedef struct for_node {
  exp_node base;
  std::string var_name;
  exp_node *start, *end, *step, *body;
} for_node;

typedef struct call_node {
  exp_node base;
  std::string callee;
  std::vector<exp_node*> args;
} call_node;

typedef struct prototype_node {
  exp_node base;
  std::string name;
  std::vector<std::string> args;
  bool is_operator;
  unsigned precedence;
} prototype_node;

typedef struct function_node {
  exp_node base;
  prototype_node* prototype;
  block_node* body;
} function_node;

prototype_node* create_prototype_node(exp_node* parent, source_loc loc,
                                      const std::string& name,
                                      std::vector<std::string>& args,
                                      bool is_operator = false,
                                      unsigned precedence = 0);