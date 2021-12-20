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

#include "clib/symbol.h"
#include "clib/util.h"
#include "sema/type.h"
#include "lexer/source_location.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * if node type order is changed here, the corresponding order of function pointer
 * in codegen.c & analyzer.c shall be changed accordingly.
 */
#define FOREACH_NODETYPE(ENUM_ITEM) \
    ENUM_ITEM(UNK_NODE)             \
    ENUM_ITEM(LITERAL_NODE)         \
    ENUM_ITEM(IDENT_NODE)           \
    ENUM_ITEM(VAR_NODE)             \
    ENUM_ITEM(TYPE_NODE)            \
    ENUM_ITEM(TYPE_VALUE_NODE)      \
    ENUM_ITEM(UNARY_NODE)           \
    ENUM_ITEM(BINARY_NODE)          \
    ENUM_ITEM(CONDITION_NODE)       \
    ENUM_ITEM(FOR_NODE)             \
    ENUM_ITEM(CALL_NODE)            \
    ENUM_ITEM(FUNC_TYPE_NODE)       \
    ENUM_ITEM(FUNCTION_NODE)        \
    ENUM_ITEM(BLOCK_NODE)

enum node_type { FOREACH_NODETYPE(GENERATE_ENUM) };

extern const char *node_type_strings[];

struct _block_node {
    struct array nodes; // struct array of ast_node*
};

struct module {
    symbol name;
    struct ast_node *block;
};

struct ast {
    struct array modules; // struct array of module*
};

struct _literal_node {
    union {
        double double_val;
        int int_val;
        bool bool_val;
        char char_val;
        const char *str_val;
    };
};

struct _ident_node {
    symbol name;
    /*if id is x.y, x is variable of object, y is member of the object*/
    struct array member_accessors;
};

struct _var_node {
    symbol var_name;
    bool is_global;
    struct ast_node *init_value;
};

struct _unary_node {
    enum op_code opcode;
    struct ast_node *operand;
};

struct _binary_node {
    enum op_code opcode;
    struct ast_node *lhs, *rhs;
};

struct _if_node {
    struct ast_node *if_node, *then_node, *else_node;
};

struct _for_node {
    symbol var_name;
    struct ast_node *start, *end, *step, *body;
};

struct _type_node {
    struct ast_node *body; //body block
    symbol name; /*type name*/
};

struct _type_value_node {
    struct ast_node *body; /*body block*/
};

#define ARRAY_FUN_PARAM(var) ARRAY(var, struct ast_node*, 0)

struct _func_type_node {
    symbol name;
    symbol op;
    struct array fun_params; /*struct array of var ast_node*/
    char is_operator;
    int precedence;
    bool is_variadic;
    bool is_extern;
};

#define UNARY_PARAM_SIZE 1
#define UNARY_SIG_SIZE (UNARY_PARAM_SIZE + 1)

#define BINARY_PARAM_SIZE 2
#define BINARY_SIG_SIZE (BINARY_PARAM_SIZE + 1)

struct _function_node {
    struct ast_node *func_type;
    struct ast_node *body; /*body block*/

    /*array of specialized functions if current is generic func*/
    struct array sp_funs; 
};

struct _call_node {
    symbol callee;
    symbol specialized_callee;
    struct array args; // args: struct array of ast_node*
    struct ast_node *callee_func_type;
};

struct ast_node {
    enum node_type node_type;
    enum type annotated_type_enum;

    struct type_exp *type; // type inferred
    struct source_location loc;

    symbol annotated_type_name; //this is expected to be removed
    bool is_ret;        //tihs is expected to be removed from sema analysis
    union{
        void *data; //node data represents any of following pointer
        struct _literal_node *liter;
        
        struct _ident_node *ident;
        struct _unary_node *unop;
        struct _binary_node *binop;
        struct _var_node *var;
        
        struct _func_type_node *ft;
        struct _function_node *func;
        struct _call_node *call;
        
        struct _type_node *type_def; 
        struct _type_value_node *type_value;
        
        struct _if_node *cond;
        struct _for_node *forloop;
        struct _block_node *block;
    };
};

/*
 * if node type order is changed here, the corresponding order of function pointer
 * in codegen.c & analyzer.c shall be changed accordingly.
 */
struct ast_node *ast_node_new(enum node_type node_type, enum type annotated_type_enum, struct source_location loc);
void ast_node_free(struct ast_node *node);

struct type_exp *get_ret_type(struct ast_node *fun_node);

struct ast_node *function_node_new(struct ast_node *func_type,
    struct ast_node *body, struct source_location loc);
struct ast_node *ident_node_new(symbol name, struct source_location loc);

struct ast_node *double_node_new(double val, struct source_location loc);
struct ast_node *int_node_new(int val, struct source_location loc);
struct ast_node *bool_node_new(bool val, struct source_location loc);
struct ast_node *char_node_new(char val, struct source_location loc);
struct ast_node *unit_node_new(struct source_location loc);
struct ast_node *string_node_new(const char *val, struct source_location loc);

struct ast_node *var_node_new(symbol var_name, enum type type, symbol ext_type, struct ast_node *init_value, bool is_global, struct source_location loc);
struct ast_node *call_node_new(symbol callee,
    struct array *args, struct source_location loc);
struct ast_node *func_type_node_new(
    symbol name,
    struct array *params,
    struct type_exp *ret_type,
    bool is_operator,
    unsigned precedence,
    symbol op,
    bool is_variadic, bool is_external, struct source_location loc);
struct ast_node *type_node_new(symbol name, struct ast_node *body, struct source_location loc);
struct ast_node *type_value_node_new(struct ast_node *body, symbol type_name, struct source_location loc);
struct ast_node *func_type_node_default_new(
    symbol name,
    struct array *args, struct type_exp *ret_type, bool is_variadic, bool is_external, struct source_location loc);

struct ast_node *if_node_new(struct ast_node *condition, struct ast_node *then_node,
    struct ast_node *else_node, struct source_location loc);
struct ast_node *unary_node_new(enum op_code opcode, struct ast_node *operand, struct source_location loc);
struct ast_node *binary_node_new(enum op_code opcode, struct ast_node *lhs, struct ast_node *rhs, struct source_location loc);
struct ast_node *for_node_new(symbol var_name, struct ast_node *start,
    struct ast_node *end, struct ast_node *step, struct ast_node *body, struct source_location loc);
struct ast_node *block_node_new(struct array *nodes);
struct ast_node *node_copy(struct ast_node *node);
struct module *module_new(const char *mod_name, FILE *file);
void node_free(struct ast_node *node);

bool is_unary_op(struct ast_node *pnode);
bool is_binary_op(struct ast_node *pnode);
char get_op_name(struct ast_node *pnode);
symbol get_callee(struct ast_node *call);
int find_member_index(struct ast_node *type_node, symbol member);

struct ast_node *find_sp_fun(struct ast_node *generic_fun, symbol sp_fun_name);

#ifdef __cplusplus
}
#endif

#endif
