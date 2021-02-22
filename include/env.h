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
    struct hashtable tenv; //type env: hashtable of <string, struct type_exp, right now including types and values*>
    struct hashtable venv;  /*value env: hashtable of <string, struct exp_node*> */

    struct hashtable builtin_tenv; /*builtin types: symbol to type_exp* map. right now including types and values*/
    struct hashtable builtin_venv; /*builtin values: symbol to type_exp*, only used for codegen right now*/
    struct hashtable generic_venv;/*generic value env: hashtable of <string, struct exp_node*>*/

    struct array nongens; //struct array of struct type_exp*, non generic
    struct array ref_builtin_names; //referred builtins of string
    struct code_generator* cg;
    struct parser* parser;
};

struct env* env_new(bool is_repl);
void env_free(struct env* env);

#ifdef __cplusplus
}
#endif

#endif
