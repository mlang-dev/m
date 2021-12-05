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
#include "lexer/lexer.h"
#include "sema/type.h"

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

struct exp_node {
    enum node_type node_type;
    enum type annotated_type_enum;
    struct type_exp *type; // type inferred
    struct source_location loc;

    symbol annotated_type_name;
    bool is_ret;
    struct exp_node *parent;
};

struct _block_node {
    struct array nodes; // struct array of exp_node*
};

struct module {
    symbol name;
    struct ast_node *block;
    struct tokenizer *tokenizer;
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
    struct exp_node *init_value;
};

struct _unary_node {
    symbol op;
    struct exp_node *operand;
};

struct _binary_node {
    symbol op;
    struct exp_node *lhs, *rhs;
};

struct _if_node {
    struct exp_node *if_node, *then_node, *else_node;
};

struct _for_node {
    symbol var_name;
    struct exp_node *start, *end, *step, *body;
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

struct function_node {
    struct exp_node base;
    struct ast_node *func_type;
    struct ast_node *body; /*body block*/
};

struct call_node {
    struct exp_node base;
    symbol callee;
    symbol specialized_callee;
    struct array args; // args: struct array of exp_node*
    struct ast_node *callee_func_type;
};

struct ast_node {
    enum node_type node_type;
    enum type annotated_type_enum;

    struct type_exp *type; // type inferred
    struct source_location loc;

    symbol annotated_type_name; //this is expected to be removed
    bool is_ret;        //tihs is expected to be removed from sema analysis
    struct exp_node *parent;  //this is expected to be removed from hand-crafted parser
    symbol node_type_name; //this is expected to be removed from parser generator
    struct array children; //list of pointer to child ast_node, this is expected to be removed from parser generator
    union{
        void *data; //node data represents any of following pointer
        struct _literal_node *liter;
        
        struct _ident_node *ident;
        struct _unary_node *unop;
        struct _binary_node *binop;
        struct _var_node *var;
        
        struct _func_type_node *func_type;
        struct function_node *func;
        struct call_node *call;
        
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
struct ast_node *ast_node_new(symbol node_type_name, enum node_type node_type, enum type annotated_type_enum, struct source_location loc, struct exp_node *parent);
void ast_node_free(struct ast_node *node);

struct type_exp *get_ret_type(struct function_node *fun_node);

struct function_node *function_node_new(struct ast_node *func_type,
    struct ast_node *body);
struct ast_node *ident_node_new(struct exp_node *parent, struct source_location loc, symbol name);

struct ast_node *double_node_new(struct exp_node *parent, struct source_location loc, double val);
struct ast_node *int_node_new(struct exp_node *parent, struct source_location loc, int val);
struct ast_node *bool_node_new(struct exp_node *parent, struct source_location loc, bool val);
struct ast_node *char_node_new(struct exp_node *parent, struct source_location loc, char val);
struct ast_node *unit_node_new(struct exp_node *parent, struct source_location loc);
struct ast_node *string_node_new(struct exp_node *parent, struct source_location loc, const char *val);

struct ast_node *var_node_new(struct exp_node *parent, struct source_location loc, symbol var_name, enum type type, symbol ext_type, struct exp_node *init_value);
struct call_node *call_node_new(struct exp_node *parent, struct source_location loc, symbol callee,
    struct array *args);
struct ast_node *func_type_node_new(struct exp_node *parent, struct source_location loc,
    symbol name,
    struct array *params,
    struct type_exp *ret_type,
    bool is_operator,
    unsigned precedence,
    symbol op,
    bool is_variadic, bool is_external);
struct ast_node *type_node_new(struct exp_node *parent, struct source_location loc, symbol name, struct ast_node *body);
struct ast_node *type_value_node_new(struct exp_node *parent, struct source_location loc, struct ast_node *body, symbol type_name);
struct ast_node *func_type_node_default_new(struct exp_node *parent, struct source_location loc,
    symbol name,
    struct array *args, struct type_exp *ret_type, bool is_variadic, bool is_external);

struct ast_node *if_node_new(struct exp_node *parent, struct source_location loc, struct exp_node *condition, struct exp_node *then_node,
    struct exp_node *else_node);
struct ast_node *unary_node_new(struct exp_node *parent, struct source_location loc, symbol op, struct exp_node *operand);
struct ast_node *binary_node_new(struct exp_node *parent, struct source_location loc, symbol op, struct exp_node *lhs, struct exp_node *rhs);
struct ast_node *for_node_new(struct exp_node *parent, struct source_location loc, symbol var_name, struct exp_node *start,
    struct exp_node *end, struct exp_node *step, struct exp_node *body);
struct ast_node *block_node_new(struct exp_node *parent, struct array *nodes);
struct exp_node *node_copy(struct exp_node *node);
struct module *module_new(const char *mod_name, FILE *file);
void node_free(struct exp_node *node);

bool is_unary_op(struct ast_node *pnode);
bool is_binary_op(struct ast_node *pnode);
char get_op_name(struct ast_node *pnode);
bool is_recursive(struct call_node *call);
symbol get_callee(struct call_node *call);
int find_member_index(struct ast_node *type_node, symbol member);

struct ast_node *find_parent_proto(struct exp_node *node);

#ifdef __cplusplus
}
#endif

#endif
