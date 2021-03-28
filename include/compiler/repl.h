/*
 * repl.h
 * 
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * header file for m repl
 */
#ifndef __MLANG_REPL_H__
#define __MLANG_REPL_H__

#include "sema/env.h"
#include "compiler/jit.h"
#include "sema/type.h"

#ifdef __cplusplus
extern "C" {
#endif

struct eval_result {
    enum type type;
    union {
        double d_value;
        int i_value;
        bool b_value;
        char c_value;
        const char* s_value;
    };
};

struct JIT* build_jit(struct env* env);
void eval_statement(void* p_jit, struct exp_node* node);
struct eval_result eval_exp(struct JIT* jit, struct exp_node* node);
int run_repl();

#ifdef __cplusplus
}
#endif

#endif
