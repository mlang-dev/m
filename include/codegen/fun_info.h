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
#include "clib/hashtable.h"

#ifdef __cplusplus
extern "C" {
#endif

struct ast_abi_arg {
    struct exp_node *node;
    struct abi_arg_info info;
};

struct fun_info {
    struct ast_abi_arg ret;
    struct array args; //array of ast_abi_arg
    bool is_chain_call;
    unsigned required_args;
};

struct fun_info *get_fun_info(struct call_node *call);

//TODO: implement with more elegant way 
void clear_fun_info(struct hash_entry* entry);

#ifdef __cplusplus
}
#endif

#endif
