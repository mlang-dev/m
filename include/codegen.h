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

struct code_generator {
    void* context;
    void* builder;
    parser* parser;
    std::map<std::string, void*> named_values;
    void* module;
    //void* fpm;
    std::map<std::string, prototype_node*> protos;
    std::map<std::string, var_node*> gvs;
};

code_generator* cg_new(menv* env, parser* parser);
void cg_free(code_generator* cg);
void create_module_and_pass_manager(code_generator* cg, const char* module_name);
void* generate_code(code_generator* cg, exp_node* node);
void generate_runtime_module(code_generator* cg, parser* parser);
void* create_target_machine(void* module);

#endif