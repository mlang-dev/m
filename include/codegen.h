/*
 * codegen.h
 * 
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * header file for IR codegen
 */
#ifndef __MLANG_CODEGEN_H__
#define __MLANG_CODEGEN_H__

#include "env.h"
#include "parser.h"
#include "clib/hashtable.h"

#ifdef __cplusplus
extern "C"{
#endif

typedef struct _code_generator {
    void* context;
    void* builder;
    parser* parser;
    struct hashtable named_values; //hashtable of string, void*
    void* module;
    //void* fpm;
    struct hashtable protos; //hashtable of char*, prototype_node*
    struct hashtable gvs; //hashtable of char* and var_node*
}code_generator;

code_generator* cg_new(menv* env, parser* parser);
void cg_free(code_generator* cg);
void create_module_and_pass_manager(code_generator* cg, const char* module_name);
void* generate_code(code_generator* cg, exp_node* node);
void generate_runtime_module(code_generator* cg, array *builtins);
void* create_target_machine(void* module);

#ifdef __cplusplus
}
#endif

#endif
