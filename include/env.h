/*
 * env.h
 * 
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * header file of m environement
 */

#ifndef __MLANG_ENV_H__
#define __MLANG_ENV_H__

#include "analyzer.h"

#ifdef __cplusplus
extern "C" {
#endif

struct env {
    struct hashtable symbols; //hashtable of <string, symbol(string*)>
    struct hashtable type_env; //hashtable of <string, struct type_exp*>
    struct hashtable type_nodes;  /*hashtable of <string, struct exp_node*> */
    struct hashtable generic_nodes;/*hashtable of <string, struct exp_node*>*/
    struct hashtable builtin_types;
    struct hashtable builtin_nodes;
    struct array nongens; //struct array of struct type_exp*
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
