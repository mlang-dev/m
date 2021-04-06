/*
 * ast_abi_arg.h
 * 
 * Copyright (C) 2021 Ligang Wang <ligangwangs@gmail.com>
 *
 * header file defining interface bridging between ast node and abi arg info
 */

#ifndef __MLANG_AST_ABI_ARG_H__
#define __MLANG_AST_ABI_ARG_H__

#include "clib/hashtable.h"
#include "codegen/abi_arg_info.h"
#include "sema/type.h"

#ifdef __cplusplus
extern "C" {
#endif

struct ast_abi_arg {
    struct type_exp *type;
    struct abi_arg_info info;
};

struct fun_info {
    struct ast_abi_arg ret;
    struct array args; //array of ast_abi_arg
    bool is_chain_call;
    unsigned required_args;
};

void fun_info_init(struct fun_info *fi);
void fun_info_deinit(struct fun_info *fi);

struct fun_info *get_fun_info(symbol fun_name, struct type_oper *fun_type);

#ifdef __cplusplus
}
#endif

#endif
