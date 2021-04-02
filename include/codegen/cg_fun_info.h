/*
 * ast_abi_arg.h
 * 
 * Copyright (C) 2021 Ligang Wang <ligangwangs@gmail.com>
 *
 * header file defining interface bridging between ast node and abi arg info
 */

#ifndef __MLANG_AST_ABI_ARG_H__
#define __MLANG_AST_ABI_ARG_H__

#include "codegen/abi_arg_info.h"
#include "parser/ast.h"

#ifdef __cplusplus
extern "C" {
#endif

struct ast_abi_arg {
    struct exp_node *node;
    struct abi_arg_info info;
};

struct cg_fun_info {
    struct ast_abi_arg ret;
    struct array args; //array of ast_abi_arg
    bool is_chain_call;
    unsigned required_args;
};

struct cg_fun_info *create_cg_fun_info(struct call_node *call);

#ifdef __cplusplus
}
#endif

#endif
