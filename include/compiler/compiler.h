/*
 * compiler.h
 * 
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * header file for m compiler
 */
#ifndef __MLANG_COMPILER_H__
#define __MLANG_COMPILER_H__

#include "parser/ast.h"
#include "sema/env.h"

#ifdef __cplusplus
extern "C" {
#endif

enum object_file_type {
    FT_UNK = 0,
    FT_BITCODE = 1,
    FT_IR = 2,
    FT_OBJECT = 3
};

int compile(const char* fn, enum object_file_type file_type);
char* emit_ir_string(struct env *env, struct exp_node* ast_node);
void free_ir_string(char *ir_string);

#ifdef __cplusplus
}
#endif

#endif
