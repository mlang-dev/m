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

#include "clib/string.h"
#include "clib/util.h"
#include "lexer.h"
#include "type.h"

#ifdef __cplusplus
extern "C" {
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

enum node_type { FOREACH_NODETYPE(GENERATE_ENUM) };

static const char* node_type_strings[] = {
    FOREACH_NODETYPE(GENERATE_ENUM_STRING)
};

struct exp_node {
    enum node_type node_type;
    struct type_exp* annotated_type;
    struct type_exp* type; //type inferred
    struct source_loc loc;
    struct exp_node* parent;
};

struct block_node {
    struct exp_node base;
    struct array nodes; //struct array of exp_node*
};

struct module {
    string name;
    struct block_node* block;
    struct file_tokenizer* tokenizer;
};

struct ast {
    struct array modules; //struct array of module*
};

struct num_node {
    struct exp_node base;
    union {
        double double_val;
        int int_val;
    };
};

struct ident_node {
    struct exp_node base;
    string name;
};

struct var_node {
    struct exp_node base;
    string var_name;
    struct exp_node* init_value;
};

struct unary_node {
    struct exp_node base;
    string op;
    struct exp_node* operand;
};

struct binary_node {
    struct exp_node base;
    string op;
    struct exp_node *lhs, *rhs;
};

struct condition_node {
    struct exp_node base;
    struct exp_node *condition_node, *then_node, *else_node;
};

struct for_node {
    struct exp_node base;
    string var_name;
    struct exp_node *start, *end, *step, *body;
};

struct call_node {
    struct exp_node base;
    string callee;
    struct array args; //args: struct array of exp_node*
};

#define ARRAY_FUN_PARAM(var)  ARRAY(var, struct var_node, 0)

struct prototype_node {
    struct exp_node base;
    string name;
    string op;
    struct array fun_params; /*struct array of var_node*/
    char is_operator;
    unsigned precedence;
    bool is_variadic;
};

#define UNARY_PARAM_SIZE 1
#define UNARY_SIG_SIZE  (UNARY_PARAM_SIZE + 1)

#define BINARY_PARAM_SIZE 2
#define BINARY_SIG_SIZE (BINARY_PARAM_SIZE + 1)

struct function_node {
    struct exp_node base;
    struct prototype_node* prototype;
    struct block_node* body;
};
struct type_exp* get_ret_type(struct function_node* fun_node);

struct function_node* create_function_node(struct prototype_node* prototype,
    struct block_node* body);
struct ident_node* create_ident_node(struct exp_node* parent, struct source_loc loc, const char* name);
struct num_node* create_double_node(struct exp_node* parent, struct source_loc loc, double val);
struct num_node* create_int_node(struct exp_node* parent, struct source_loc loc, int val);
struct num_node* create_bool_node(struct exp_node* parent, struct source_loc loc, int val);
struct var_node* create_var_node(struct exp_node* parent, struct source_loc loc, const char* var_name, struct exp_node* init_value);
struct call_node* create_call_node(struct exp_node* parent, struct source_loc loc, const char* callee,
    struct array* args);
struct prototype_node* create_prototype_node(struct exp_node* parent, struct source_loc loc,
    const char* name,
    struct array* params,
    struct type_exp* ret_type,
    bool is_operator,
    unsigned precedence,
    const char* op,
    bool is_variadic);
struct prototype_node* create_prototype_node_default(struct exp_node* parent, struct source_loc loc,
    const char* name,
    struct array* args, struct type_exp* ret_type, bool is_variadic);

struct condition_node* create_if_node(struct exp_node* parent, struct source_loc loc, struct exp_node* condition, struct exp_node* then_node,
    struct exp_node* else_node);
struct unary_node* create_unary_node(struct exp_node* parent, struct source_loc loc, const char* op, struct exp_node* operand);
struct binary_node* create_binary_node(struct exp_node* parent, struct source_loc loc, const char* op, struct exp_node* lhs, struct exp_node* rhs);
struct for_node* create_for_node(struct exp_node* parent, struct source_loc loc, const char* var_name, struct exp_node* start,
    struct exp_node* end, struct exp_node* step, struct exp_node* body);
struct block_node* create_block_node(struct exp_node* parent, struct array* nodes);
struct module* create_module(const char* mod_name, FILE* file);

bool is_unary_op(struct prototype_node* pnode);
bool is_binary_op(struct prototype_node* pnode);
char get_op_name(struct prototype_node* pnode);

#ifdef __cplusplus
}
#endif

#endif
