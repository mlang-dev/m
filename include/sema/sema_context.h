/*
 * sema_context.h
 * 
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * header file of m environement
 */

#ifndef __MLANG_SEMA_CONTEXT_H__
#define __MLANG_SEMA_CONTEXT_H__

#include "clib/array.h"
#include "clib/stack.h"
#include "clib/symboltable.h"
#include "parser/ast.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * define nested if block and loop block levels inside a function
 * 
 */
struct loop_nested_level{
    u32 block_levels;
};

struct sema_context {
    /* mapping type string into type enum: hashtable of (symbol, int) */
    struct type_context *tc;

    /* 
     *  value type env: symboltable of <symbol, struct type_item>*>
     *  binding variable name to type expression
     */
    struct symboltable varname_2_typexprs;

    /* 
     *  value ast env: symboltable of <symbol, struct ast_node>*>
     *  binding variable name to ast node expression
     */
    struct symboltable varname_2_asts;

    /* 
     *  symboltable of <symbol, var ast_node*>
     *  binding global variable name to var ast_node*
     */
    struct hashtable gvar_name_2_ast;

    /* 
     *  type env: symboltable of <symbol, struct type_item_pair*>
     *  binding type name to type expression
     */
    struct symboltable typename_2_typexpr_pairs;

    /* 
     *  new type AST def: hashtable of <symbol, struct ast_node （type_item_node)*> 
     *      example: type Point2D = x:int y:int
     */
    struct hashtable struct_typename_2_asts;


    /* 
     *  hashtable of <symbol, symbol> 
     *      map type symbol to its reference type symbol: e.g. int : &int
     */
    struct hashtable type_2_ref_symbol;


    /* 
     *  function ast_node pointer stacks
     */
    struct stack func_stack;

    /* 
     *  builtin ASTs: symbol to ast_node*, only used for codegen
     */
    struct hashtable builtin_ast;
    struct ast_node *builtin_ast_block;

    /* 
     *  generic value ASTs: hashtable of <symbol, struct ast_node*>
     */
    struct hashtable generic_ast;

    /* 
     *  specialized value ASTs: hashtable of <symbol, struct ast_node*>
     */
    struct hashtable specialized_ast;

    /* 
     *  new specialized value ASTs in one analysis
     */
    struct array new_specialized_asts;

    /* 
     *  func_type declaration ASTs: hashtable of <symbol, struct ast_node*>
     */
    struct hashtable func_types;

    /* 
     *  call ASTs: hashtable of <symbol, struct ast_node*>
     */
    struct hashtable calls;
    /* 
     *  struct array of struct type_item*, specialized (non generic) 
     */
    struct array nongens;

    /*
     *  used builtins of symbol, needs to be codegened by adding to the module
     */
    struct array used_builtin_names;

    symbol scope_marker;

    /*
     * indicates block levels
     */
    size_t scope_level;

    /*
     * indicates nested if/loop block levels inside a loop of a function, used for WebAssembly codegen.
     */
    struct array nested_levels; //array of array<struct loop_nested_level>

    bool is_repl;
};

struct field_info{
    struct ast_node *offset_expr;  //offset expr
    u32 align;  //alignment of the field.
    struct type_item *type; //field type
    struct ast_node *aggr_root; //root 
};

struct sema_context *sema_context_new(struct type_context *tc, struct ast_node *sys_block, bool is_repl);
void sema_context_free(struct sema_context *env);
size_t enter_scope(struct sema_context *env);
size_t leave_scope(struct sema_context *env);
struct ast_node *find_generic_fun(struct sema_context *context, symbol fun_name);
struct ast_node *sc_aggr_get_offset_expr(struct sema_context *sc, struct type_item *aggr_type, struct ast_node *field_node);
void sc_get_field_infos_from_root(struct sema_context *sc, struct ast_node* index, struct array *field_infos);
symbol get_ref_type_symbol(struct sema_context *context, symbol type_name);
struct loop_nested_level *get_current_block_level(struct sema_context *context);
void enter_function(struct sema_context *context);
void leave_function(struct sema_context *context);
struct loop_nested_level *enter_loop(struct sema_context *context);
void leave_loop(struct sema_context *context);

#ifdef __cplusplus
}
#endif

#endif
