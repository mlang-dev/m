/*
 * ast.h
 * 
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * ast header file
 */
#ifndef __MLANG_AST_H__
#define __MLANG_AST_H__

#include <stdio.h>

#include "lexer.h"
#include "type.h"
#include "clib/util.h"
#include "clib/string.h"

#ifdef __cplusplus
extern "C"{
#endif

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

enum _NodeType { FOREACH_NODETYPE(GENERATE_ENUM) };
typedef enum _NodeType NodeType;

static const char* NodeTypeString[] = {
    FOREACH_NODETYPE(GENERATE_ENUM_STRING)
};

typedef struct _exp_node {
    NodeType node_type;
    type_exp type; //type annotation
    source_loc loc;
    struct _exp_node* parent;
}exp_node;

typedef struct _block_node {
    exp_node base;
    array nodes; //array of exp_node*
}block_node;

typedef struct _module {
    string name;
    block_node* block;
    file_tokenizer* tokenizer;
}module;

typedef struct _ast {
    array builtins; //array of exp_node*
    array modules; //array of module*
}ast;

typedef struct _num_node {
    exp_node base;
    union {
        double double_val;
        int int_val;
    };
}num_node;

typedef struct _ident_node {
    exp_node base;
    string name;
}ident_node;

typedef struct _var_node {
    exp_node base;
    string var_name;
    exp_node* init_value;
}var_node;

typedef struct _unary_node {
    exp_node base;
    string op;
    exp_node* operand;
}unary_node;

typedef struct _binary_node {
    exp_node base;
    string op;
    exp_node *lhs, *rhs;
}binary_node;

typedef struct _condition_node {
    exp_node base;
    exp_node *condition_node, *then_node, *else_node;
}condition_node;

typedef struct _for_node {
    exp_node base;
    string var_name;
    exp_node *start, *end, *step, *body;
}for_node;

typedef struct _call_node {
    exp_node base;
    string callee;
    array args; //args: array of exp_node*
}call_node;

typedef struct _prototype_node {
    exp_node base;
    string name;
    string op;
    array args; /*array of string*/
    char is_operator;
    unsigned precedence;
}prototype_node;

typedef struct _function_node {
    exp_node base;
    prototype_node* prototype;
    block_node* body;
}function_node;

function_node* create_function_node(prototype_node* prototype,
    block_node* body);
ident_node* create_ident_node(exp_node* parent, source_loc loc, const char *name);
num_node* create_double_node(exp_node* parent, source_loc loc, double val);
num_node* create_int_node(exp_node* parent, source_loc loc, int val);
var_node* create_var_node(exp_node* parent, source_loc loc, const char *var_name, exp_node* init_value);
call_node* create_call_node(exp_node* parent, source_loc loc, const char *callee,
    array *args);
prototype_node* create_prototype_node(exp_node* parent, source_loc loc,
    const char *name,
    array* args,
    bool is_operator,
    unsigned precedence,
    const char *op);
prototype_node* create_prototype_node_default(exp_node* parent, source_loc loc,
    const char *name,
    array* args);

condition_node* create_if_node(exp_node* parent, source_loc loc, exp_node* condition, exp_node* then_node,
    exp_node* else_node);
unary_node* create_unary_node(exp_node* parent, source_loc loc, const char *op, exp_node* operand);
binary_node* create_binary_node(exp_node* parent, source_loc loc, const char *op, exp_node* lhs, exp_node* rhs);
for_node* create_for_node(exp_node* parent, source_loc loc, const char *var_name, exp_node* start,
    exp_node* end, exp_node* step, exp_node* body);
block_node* create_block_node(exp_node* parent, array *nodes);
module* create_module(const char* mod_name, FILE* file);

#ifdef __cplusplus
}
#endif

#endif
