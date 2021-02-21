/*
 * analyzer.h
 * 
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * header file for type inference & semantic analsysis
 */
#ifndef __MLANG_ANALYZER_H__
#define __MLANG_ANALYZER_H__

#include "ast.h"
#include "clib/hashtable.h"
#include "codegen.h"

#ifdef __cplusplus
extern "C" {
#endif

struct type_env {
    struct hashtable symbols; //hashtable of <string, symbol(string*)>
    struct hashtable type_env; //hashtable of <string, struct type_exp*>
    struct hashtable type_nodes;  /*hashtable of <string, struct exp_node*> */
    struct hashtable generic_nodes;/*hashtable of <string, struct exp_node*>*/
    struct hashtable builtin_types;
    struct hashtable builtin_nodes;
    struct array nongens; //struct array of struct type_exp*
    struct array ref_builtin_names; //referred builtins of string
    struct code_generator* cg;
};

struct type_exp* retrieve(struct type_env* env, const char* name);
struct type_env* type_env_new(struct parser* parser);
void type_env_free(struct type_env* env);
struct type_exp* analyze(struct type_env* env, struct exp_node* node);
bool is_builtin(struct type_env* env, const char* name);

#ifdef __cplusplus
}
#endif

#endif
