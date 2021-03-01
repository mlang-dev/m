/*
 * env.h
 * 
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * header file of m environement
 */

#ifndef __MLANG_ENV_H__
#define __MLANG_ENV_H__

#include "clib/hashtable.h"
#include "clib/array.h"
#include "codegen.h"
#include "parser.h"

#ifdef __cplusplus
extern "C" {
#endif

struct env {
    /* value type env: hashtable of <string, struct type_exp>*>
     * binding variable name to type expression
     */
    struct hashtable venv; 

    /* type env: hashtable of <string, struct type_exp>*>
     * binding type name to type expression
     */
    struct hashtable tenv;

    /*new type ast def: hashtable of <string, struct exp_node （type_node)*> 
     * example: type Point2D = x:int y:int
     */
    struct hashtable ext_type_ast;  

    struct hashtable builtin_ast; /*builtin values: symbol to type_exp*, only used for codegen right now*/

    struct hashtable generic_ast;/*generic value env: hashtable of <string, struct exp_node*>*/
    struct array nongens; //struct array of struct type_exp*, specialized (non generic)

    /*used builtins of string, needs to be codegened by adding to the module*/
    struct array used_builtin_names; 

    struct code_generator* cg;
    struct parser* parser;
};

struct env* env_new(bool is_repl);
void env_free(struct env* env);

#ifdef __cplusplus
}
#endif

#endif
