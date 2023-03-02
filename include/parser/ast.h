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
#include "lexer/terminal.h"
#include "parser/node_type.h"


#ifdef __cplusplus
extern "C" {
#endif

struct module {
    symbol name;
    struct ast_node *block;
};

struct block_node {
    struct array nodes; // struct array of ast_node*
};

struct literal_node {
    enum type type;
    union {
        f64 double_val;
        int int_val;
        const char *str_val;
    };
};

struct ident_node {
    symbol name;
    struct ast_node *var; //reference to its variable declaration
    bool is_member_index_object;
};

struct memory_node {
    struct ast_node *initial;
    struct ast_node *max;
};

struct type_expr_item_node {
    struct ast_node *ident;
    struct ast_node *is_of_type;
};

struct name_type_node {
    symbol var_name; //
    struct ast_node *is_of_type;
};

struct var_node {
    bool is_global;
    enum Mut mut; //is mutable
    bool is_init_shared; //is init value is a shared node, not owning it.
    struct ast_node *var;
    struct ast_node *is_of_type;
    struct ast_node *init_value;
};

struct unary_node {
    enum op_code opcode;
    bool is_postfix;
    struct ast_node *operand;
};

struct binary_node {
    enum op_code opcode;
    struct ast_node *lhs, *rhs;
};

enum IndexType {
    IndexTypeInteger = 0,
    IndexTypeName = 1
};

struct member_index_node {
    enum IndexType index_type;
    struct ast_node *object, *index;
};

struct if_node {
    struct ast_node *if_node, *then_node, *else_node;
};

struct for_node {
    struct ast_node *var;
    struct ast_node *range;
    struct ast_node *body;
};

struct while_node {
    struct ast_node *expr;
    struct ast_node *body;
};

struct jump_node {
    enum token_type token_type;
    /* 
     * nested_block_levels: generated in Analyzer and used in WebAssembly codegen for break/continue statement,
     * It indicates the innermost loop levels from the break/continue statement
     */
    u32 nested_block_levels; 
    struct ast_node *expr;
};

struct type_node {
    symbol type_name;
    struct ast_node *type_body;
};

/**
 * algebraic data type
*/
struct adt_node {
    enum ADTKind kind;
    struct ast_node *body; //body block
    symbol name; /*type name*/
};

/*record or tuple initializer node*/
enum ADTInitKind {
    ADTInitRecord = 0,
    ADTInitTuple
};
struct adt_init_node { 
    enum ADTInitKind kind;
    struct ast_node *is_of_type;
    struct ast_node *body; /*body block*/
};

struct range_node{
    struct ast_node *start, *end, *step;
};

#define ARRAY_FUN_PARAM(var) ARRAY(var, struct ast_node*, 0)

struct func_type_node {
    symbol name;
    symbol op;
    struct ast_node *params; /*block ast_node for params*/
    struct ast_node *ret_type_item_node; /*returning ast_node type*/
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
    struct ast_node *func_type;
    struct ast_node *body; /*body block*/

    /*array of specialized functions if current is generic func*/
    struct array sp_funs; 
};

struct call_node {
    symbol callee;
    symbol specialized_callee;
    struct ast_node *arg_block; // args: block ast node
    struct ast_node *callee_func_type;
};

struct array_type_node {
    struct ast_node *elm_type;
    struct ast_node *dims; 
};

struct import_node {
    symbol from_module;
    struct ast_node *import;
};

struct cast_node {
    struct ast_node *to_type_item_node;
    struct ast_node *expr;
};

enum TypeNodeKind {
    BuiltinType = 0,
    TypeName,  //builtin types or user defined types
    ArrayType,
    TupleType, //tuple types 
    RefType
};

struct type_item_node {
    enum Mut mut;
    enum TypeNodeKind kind;
    union{
        symbol type_name;
        struct array_type_node *array_type_node;
        struct ast_node *tuple_block;
        struct type_item_node *val_node; //used in ref node;
    };
};

enum UnionKind {
    TaggedUnion = 0,
    UntaggedUnion = 1,
    EnumTagOnly = 2,
    EnumTagValue = 3
};

/**
 * @brief enum type has only tag, but no named types. untagged union (c type of union) has only 
 * named types while tagged union has both.
 * 
 */
struct variant_type_node {
    enum UnionKind kind;
    symbol tag;   
    int tag_repr;
    /**
     * @brief tag_value for tagged union is block_node, is type_item_node for untagged union, enum tag only is None, and enum tag value is int const value 
     * 
     */
    struct ast_node *tag_value; 
};

struct match_node {
    struct ast_node *test_expr;
    struct ast_node *match_cases;
};

struct match_case_node {
    struct ast_node *pattern;
    struct ast_node *guard;
    struct ast_node *expr;
};

struct ast_node {
    enum node_type node_type;

    struct type_item *type; // type inferred
    struct source_location loc;

    /**
     * @brief transformed is set when original ast is transformed into new either more basic (syntactic sugar) one
     *  or do compile time function evaluation (constant folding) optimization.
     * 
     */
    struct ast_node *transformed; 

    bool is_addressed;   //
    bool is_ret;        //this is expected to be removed from sema analysis
    bool is_lvalue;      //default is zero (read), for left side of assignment node, it will be set as 1
    bool is_addressable;     // is left value
    union{
        void *data; //node data represents any of following pointer
        struct literal_node *liter;
        
        struct ident_node *ident;
        struct unary_node *unop;
        struct binary_node *binop;
        struct member_index_node *index;
        struct var_node *var;
        
        struct func_type_node *ft;
        struct function_node *func;
        struct call_node *call;
        
        struct adt_node *adt_type; 
        struct variant_type_node * variant_type_node;
        struct adt_init_node *adt_init;
        struct ast_node *array_init;
        struct array_type_node *array_type;
        struct type_item_node *type_item_node;
        struct type_node *type_node;
        struct range_node *range;
        struct import_node *import;
        struct if_node *cond;
        struct for_node *forloop;
        struct while_node *whileloop;
        struct jump_node *jump;
        struct block_node *block;
        struct memory_node *memory;
        struct cast_node *cast;
        struct match_node *match;
        struct match_case_node *match_case;
        struct type_expr_item_node *type_expr_item;
    };
};

void ast_init();
void ast_deinit();  
struct node_type_name *get_node_type_name_by_symbol(symbol symbol);
/*
 * if node type order is changed here, the corresponding order of function pointer
 * in codegen.c & analyzer.c shall be changed accordingly.
 */
/*construct ast node with type enum directly*/
struct ast_node *ast_node_new(enum node_type node_type, struct source_location loc);
void ast_node_free(struct ast_node *node);
struct type_item *get_ret_type(struct ast_node *fun_node);

struct ast_node *function_node_new(struct ast_node *func_type,
    struct ast_node *body, struct source_location loc);
struct ast_node *ident_node_new(symbol name, struct source_location loc);
struct ast_node *type_item_node_new_with_type_name(symbol type_name, enum Mut mut, struct source_location loc);
struct ast_node *type_item_node_new_with_array_type(struct array_type_node *array_type_node, enum Mut mut, struct source_location loc);
struct ast_node *type_item_node_new_with_tuple_type(struct ast_node *tuple_block, enum Mut mut, struct source_location loc);
struct ast_node *type_item_node_new_with_builtin_type(symbol type_name, enum Mut mut, struct source_location loc);
struct ast_node *type_item_node_new_with_ref_type(struct type_item_node *val_node, enum Mut mut, struct source_location loc);
struct ast_node *type_node_new(symbol type_name, struct ast_node *type_body, struct source_location loc);
struct ast_node *double_node_new(f64 val, struct source_location loc);
struct ast_node *int_node_new(int val, struct source_location loc);
struct ast_node *bool_node_new(bool val, struct source_location loc);
struct ast_node *char_node_new(char val, struct source_location loc);
struct ast_node *unit_node_new(struct source_location loc);
struct ast_node *string_node_new(const char *val, struct source_location loc);
struct ast_node *const_one_node_new(enum type type, struct source_location loc);
struct ast_node *var_node_new(struct ast_node *var, struct ast_node *is_of_type, struct ast_node *init_value, bool is_global, enum Mut mut, struct source_location loc);
struct ast_node *call_node_new(symbol callee,
    struct ast_node *arg_block, struct source_location loc);
struct ast_node *import_node_new(symbol from_module, struct ast_node *node, struct source_location loc);
struct ast_node *memory_node_new(struct ast_node *initial, struct ast_node *max, struct source_location loc);
struct ast_node *func_type_item_node_new(
    symbol name,
    struct ast_node *params, 
    symbol ret_type,
    struct ast_node *ret_type_item_node, 
    bool is_variadic, bool is_external, struct source_location loc);
struct ast_node *adt_node_new(enum node_type node_type, symbol name, struct ast_node *body, struct source_location loc);
struct ast_node *variant_type_node_new(enum UnionKind kind, symbol tag, struct ast_node * name_types, struct source_location loc);
struct ast_node *adt_init_node_new(enum ADTInitKind kind, struct ast_node *body, struct ast_node *type_item_node, struct source_location loc);
struct ast_node *array_init_node_new(struct ast_node *comp, struct source_location loc);
struct ast_node *array_type_node_new(struct ast_node *elm_type, struct ast_node *dims, struct source_location loc);
struct ast_node *type_expr_item_node_new(struct ast_node *ident, struct ast_node *is_of_type, struct source_location loc);
struct ast_node *range_node_new(struct ast_node *start, struct ast_node *end, struct ast_node *step, struct source_location loc);
struct ast_node *func_type_item_node_default_new(
    symbol name,
    struct ast_node *arg_block, symbol ret_type, struct ast_node *ret_type_item_node, bool is_variadic, bool is_external, struct source_location loc);

struct ast_node *if_node_new(struct ast_node *condition, struct ast_node *then_node,
    struct ast_node *else_node, struct source_location loc);
struct ast_node *match_node_new(struct ast_node *condition, struct ast_node *match_cases,
    struct source_location loc);
struct ast_node *match_item_node_new(struct ast_node *pattern, struct ast_node *when_condition, struct ast_node *expr,
    struct source_location loc);
struct ast_node *unary_node_new(enum op_code opcode, struct ast_node *operand, bool is_postfix, struct source_location loc);
struct ast_node *cast_node_new(struct ast_node *to_type_item_node, struct ast_node *expr, struct source_location loc);
struct ast_node *binary_node_new(enum op_code opcode, struct ast_node *lhs, struct ast_node *rhs, struct source_location loc);
struct ast_node *assign_node_new(enum op_code opcode, struct ast_node *lhs, struct ast_node *rhs, struct source_location loc);
struct ast_node *member_index_node_new(enum IndexType index_type, struct ast_node *object, struct ast_node *index, struct source_location loc);
struct ast_node *for_node_new(struct ast_node *var, struct ast_node *range, struct ast_node *body, struct source_location loc);
struct ast_node *while_node_new(struct ast_node *expr, struct ast_node *body, struct source_location loc);
struct ast_node *jump_node_new(enum token_type token_type, struct ast_node *expr, struct source_location loc);
struct ast_node *block_node_new_empty();
struct ast_node *block_node_new(struct array *nodes);
struct ast_node *block_node_add(struct ast_node *block, struct ast_node *node);
struct ast_node *block_node_add_block(struct ast_node *block, struct ast_node *node);
void free_block_node(struct ast_node *node, bool deep_free);
struct ast_node *wrap_as_block_node(struct ast_node *node);

struct ast_node *node_copy(struct ast_node *node);
struct module *module_new(const char *mod_name, FILE *file);
void node_free(struct ast_node *node);

bool is_unary_op(struct ast_node *pnode);
bool is_binary_op(struct ast_node *pnode);
char get_op_name(struct ast_node *pnode);
symbol get_callee(struct ast_node *call);
int find_member_index(struct ast_node *type_item_node, symbol member);
int find_field_index(struct ast_node *type_item_node, struct ast_node *index);

struct ast_node *find_sp_fun(struct ast_node *generic_fun, symbol sp_fun_name);

struct ast_node *wrap_expr_as_function(struct hashtable *symbol_2_int_types, struct ast_node *exp, symbol fn);
struct ast_node *wrap_nodes_as_function(struct hashtable *symbol_2_int_types, symbol func_name, struct ast_node *block);
struct ast_node *get_root_object(struct ast_node *node);
bool is_refered_later(struct ast_node *node);
void set_lvalue(struct ast_node *node);

#ifdef __cplusplus
}
#endif

#endif

