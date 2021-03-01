/*
 * env.h
 * 
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * header file of m environement
 */

#ifndef __MLANG_ENV_H__
#define __MLANG_ENV_H__

#include "clib/symboltable.h"
#include "clib/array.h"
#include "codegen.h"
#include "parser.h"

#ifdef __cplusplus
extern "C" {
#endif

struct env {
    /* 
     *  value type env: symboltable of <symbol, struct type_exp>*>
     *  binding variable name to type expression
     */
    struct symboltable venv; 

    /* 
     *  type env: symboltable of <symbol, struct type_exp>*>
     *  binding type name to type expression
     */
    struct symboltable tenv;

    /* 
     *  new type AST def: hashtable of <symbol, struct exp_node （type_node)*> 
     *      example: type Point2D = x:int y:int
     */
    struct hashtable ext_type_ast;  

    /* 
     *  builtin ASTs: symbol to exp_node*, only used for codegen
     */
    struct hashtable builtin_ast; 

    /* 
     *  generic value ASTs: hashtable of <symbol, struct exp_node*>
     */
    struct hashtable generic_ast;

    /* 
     *  struct array of struct type_exp*, specialized (non generic) 
     */
    struct array nongens; 

    /*
     *  used builtins of symbol, needs to be codegened by adding to the module
     */
    struct array used_builtin_names; 

    struct code_generator* cg;
    struct parser* parser;
};

struct env* env_new(bool is_repl);
void env_free(struct env* env);
void enter_scope(symboltable* st);
void leave_scope(symboltable* st);

#ifdef __cplusplus
}
#endif

#endif
