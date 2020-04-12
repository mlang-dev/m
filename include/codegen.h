/*
 * codegen.h
 * 
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * header file for IR codegen
 */
#ifndef __MLANG_CODEGEN_H__
#define __MLANG_CODEGEN_H__

#include "parser.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/Module.h"

using namespace std;

struct code_generator {
    void* context;
    void* builder;
    parser* parser;
    map<string, void*> named_values;
    unique_ptr<llvm::Module> module;
    unique_ptr<llvm::legacy::FunctionPassManager> fpm;
    map<string, prototype_node*> protos;
    map<string, var_node*> gvs;
};

code_generator* create_code_generator(parser* parser);
void destroy_code_generator(code_generator* cg);
void create_module_and_pass_manager(code_generator* cg, const char* module_name);
void* generate_code(code_generator* cg, exp_node* node);
void generate_runtime_module(code_generator* cg, parser* parser);

#endif