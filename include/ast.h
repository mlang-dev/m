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
    ENUM_ITEM(LITERAL_NODE)          \
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

extern const char* node_type_strings[];

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

struct literal_node {
    struct exp_node base;
    union {
        double double_val;
        int int_val;
        bool bool_val;
        char char_val;
        const char* str_val;
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
    string specialized_callee;
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

struct function_node* function_node_new(struct prototype_node* prototype,
    struct block_node* body);
struct function_node* copy_function_node(struct function_node* fun, const char* name);
struct ident_node* ident_node_new(struct exp_node* parent, struct source_loc loc, const char* name);
struct literal_node* double_node_new(struct exp_node* parent, struct source_loc loc, double val);
struct literal_node* int_node_new(struct exp_node* parent, struct source_loc loc, int val);
struct literal_node* bool_node_new(struct exp_node* parent, struct source_loc loc, bool val);
struct literal_node* char_node_new(struct exp_node* parent, struct source_loc loc, char val);
struct literal_node* string_node_new(struct exp_node* parent, struct source_loc loc, const char* val);
struct var_node* var_node_new(struct exp_node* parent, struct source_loc loc, const char* var_name, enum type type, struct exp_node* init_value);
struct call_node* call_node_new(struct exp_node* parent, struct source_loc loc, const char* callee,
    struct array* args);
struct prototype_node* prototype_node_new(struct exp_node* parent, struct source_loc loc,
    const char* name,
    struct array* params,
    struct type_exp* ret_type,
    bool is_operator,
    unsigned precedence,
    const char* op,
    bool is_variadic);
struct prototype_node* prototype_node_default_new(struct exp_node* parent, struct source_loc loc,
    const char* name,
    struct array* args, struct type_exp* ret_type, bool is_variadic);

struct condition_node* if_node_new(struct exp_node* parent, struct source_loc loc, struct exp_node* condition, struct exp_node* then_node,
    struct exp_node* else_node);
struct unary_node* unary_node_new(struct exp_node* parent, struct source_loc loc, const char* op, struct exp_node* operand);
struct binary_node* binary_node_new(struct exp_node* parent, struct source_loc loc, const char* op, struct exp_node* lhs, struct exp_node* rhs);
struct for_node* for_node_new(struct exp_node* parent, struct source_loc loc, const char* var_name, struct exp_node* start,
    struct exp_node* end, struct exp_node* step, struct exp_node* body);
struct block_node* block_node_new(struct exp_node* parent, struct array* nodes);
struct exp_node* node_copy(struct exp_node* node);
struct module* module_new(const char* mod_name, FILE* file);
void node_free(struct exp_node* node);

bool is_unary_op(struct prototype_node* pnode);
bool is_binary_op(struct prototype_node* pnode);
char get_op_name(struct prototype_node* pnode);
bool is_recursive(struct call_node* call);

#ifdef __cplusplus
}
#endif

#endif
