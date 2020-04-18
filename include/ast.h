/*
 * ast.h
 * 
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * ast header file
 */
#ifndef __MLANG_AST_H__
#define __MLANG_AST_H__

#include <string>
#include <vector>

#include "lexer.h"
#include "type.h"
#include "util.h"
#include "clib/string.h"

#define FOREACH_NODETYPE(ENUM_ITEM) \
    ENUM_ITEM(UNK_NODE)             \
    ENUM_ITEM(NUMBER_NODE)          \
    ENUM_ITEM(IDENT_NODE)           \
    ENUM_ITEM(VAR_NODE)             \
    ENUM_ITEM(UNARY_NODE)           \
    ENUM_ITEM(BINARY_NODE)          \
    ENUM_ITEM(CONDITION_NODE)       \
    ENUM_ITEM(FOR_NODE)             \
    ENUM_ITEM(CALL_NODE)            \
    ENUM_ITEM(PROTOTYPE_NODE)       \
    ENUM_ITEM(FUNCTION_NODE)        \
    ENUM_ITEM(BLOCK_NODE)

enum NodeType { FOREACH_NODETYPE(GENERATE_ENUM) };

static const char* NodeTypeString[] = {
    FOREACH_NODETYPE(GENERATE_ENUM_STRING)
};

struct exp_node {
    NodeType node_type;
    type_exp type; //type annotation
    source_loc loc;
    exp_node* parent;
};

struct block_node {
    exp_node base;
    std::vector<exp_node*> nodes;
};

struct module {
    string name;
    block_node* block;
    file_tokenizer* tokenizer;
};

struct ast {
    std::vector<exp_node*> builtins;
    std::vector<module*> modules;
};

struct num_node {
    exp_node base;
    union {
        double double_val;
        int int_val;
    };
};

struct ident_node {
    exp_node base;
    string name;
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
    string var_name;
    exp_node *start, *end, *step, *body;
};

struct call_node {
    exp_node base;
    std::string callee;
    std::vector<exp_node*> args;
};

struct prototype_node {
    exp_node base;
    std::string name;
    std::string op;
    std::vector<std::string> args;
    char is_operator;
    unsigned precedence;
};

struct function_node {
    exp_node base;
    prototype_node* prototype;
    block_node* body;
};

function_node* create_function_node(prototype_node* prototype,
    block_node* body);
ident_node* create_ident_node(exp_node* parent, source_loc loc, std::string& name);
num_node* create_num_node(exp_node* parent, source_loc loc, double val);
num_node* create_num_node(exp_node* parent, source_loc loc, int val);
var_node* create_var_node(exp_node* parent, source_loc loc, std::string var_name, exp_node* init_value);
call_node* create_call_node(exp_node* parent, source_loc loc, const std::string& callee,
    std::vector<exp_node*>& args);
prototype_node* create_prototype_node(exp_node* parent, source_loc loc,
    const std::string& name,
    std::vector<std::string>& args,
    bool is_operator = false,
    unsigned precedence = 0,
    std::string op = "");

condition_node* create_if_node(exp_node* parent, source_loc loc, exp_node* condition, exp_node* then_node,
    exp_node* else_node);
unary_node* create_unary_node(exp_node* parent, source_loc loc, std::string op, exp_node* operand);
binary_node* create_binary_node(exp_node* parent, source_loc loc, std::string op, exp_node* lhs, exp_node* rhs);
for_node* create_for_node(exp_node* parent, source_loc loc, const std::string& var_name, exp_node* start,
    exp_node* end, exp_node* step, exp_node* body);
block_node* create_block_node(exp_node* parent, std::vector<exp_node*>& nodes);
module* create_module(const char* mod_name, FILE* file);



#endif