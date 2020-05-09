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

struct code_generator {
    void* context;
    void* builder;
    struct parser* parser;
    struct hashtable named_values; //hashtable of string, void*
    void* module;
    //void* fpm;
    struct hashtable protos; //hashtable of char*, prototype_node*
    struct hashtable gvs; //hashtable of char* and var_node*
};

struct code_generator* cg_new(struct menv* env, struct parser* parser);
void cg_free(struct code_generator* cg);
void create_module_and_pass_manager(struct code_generator* cg, const char* module_name);
void* generate_code(struct code_generator* cg, struct exp_node* node);
void generate_runtime_module(struct code_generator* cg, struct array *builtins);
void* create_target_machine(void* module);

#define is_int_type(type) (false)//(type == TYPE_INT || type == TYPE_BOOL)

#ifdef __cplusplus
}
#endif

#endif
