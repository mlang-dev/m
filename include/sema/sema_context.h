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

struct sema_context {
    /* mapping type string into type enum: hashtable of (symbol, int) */
    struct hashtable *symbol_2_int_types;

    /* 
     *  value type env: symboltable of <symbol, struct type_expr>*>
     *  binding variable name to type expression
     */
    struct symboltable decl_2_typexps;

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
     *  type env: symboltable of <symbol, struct type_expr>*>
     *  binding type name to type expression
     */
    struct symboltable typename_2_typexps;

    /* 
     *  new type AST def: hashtable of <symbol, struct ast_node （type_node)*> 
     *      example: type Point2D = x:int y:int
     */
    struct hashtable struct_typename_2_asts;

    /* 
     *  function ast_node pointer stacks
     */
    struct stack func_stack;

    /* 
     *  builtin ASTs: symbol to ast_node*, only used for codegen
     */
    struct hashtable builtin_ast;

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
     *  struct array of struct type_expr*, specialized (non generic) 
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


    bool is_repl;
};

struct field_info{
    u32 offset; //offset to the root struct.
    u32 align;  //alignment of the field.
    struct type_expr *type;
    struct ast_node *root_struct;
};

struct sema_context *sema_context_new(struct hashtable *symbol_2_int_types, struct ast_node *stdio, struct ast_node *math, bool is_repl);
void sema_context_free(struct sema_context *env);
size_t enter_scope(struct sema_context *env);
size_t leave_scope(struct sema_context *env);
struct ast_node *find_generic_fun(struct sema_context *context, symbol fun_name);
struct field_info sc_get_field_info(struct sema_context *sc, symbol struct_name, symbol field_name);
struct field_info sc_get_field_info_from_root(struct sema_context *sc, struct ast_node* index);

#ifdef __cplusplus
}
#endif

#endif
