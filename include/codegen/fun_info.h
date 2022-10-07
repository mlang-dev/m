/*
 * fun_info.h
 * 
 * Copyright (C) 2021 Ligang Wang <ligangwangs@gmail.com>
 *
 * header file defining function info in generating IR functions
 */

#ifndef __MLANG_FUN_INFO_H__
#define __MLANG_FUN_INFO_H__

#include "clib/hashtable.h"
#include "codegen/abi_arg_info.h"
#include "codegen/target_arg_info.h"
#include "codegen/target_info.h"
#include "parser/ast.h"
#include "sema/type.h"

#ifdef __cplusplus
extern "C" {
#endif

extern const unsigned ALL_REQUIRED;

struct fun_info {
    struct abi_arg_info ret;
    struct target_arg_info tai;
    struct array args; //array of abi_arg_info
    bool is_chain_call;
    unsigned required_args; //required number of arguments: ~0U meaning all are requireed in args array
};

bool is_variadic(struct fun_info *fi);
void fun_info_init(struct fun_info *fi, struct ast_node *func_type);
void fun_info_deinit(struct fun_info *fi);
typedef void (*fn_compute_fun_info)(struct target_info* ti, struct fun_info *fi);
struct fun_info *compute_target_fun_info(struct target_info *ti, fn_compute_fun_info compute_fun_info, struct ast_node *func_type);
TargetType create_target_fun_type(struct target_info *ti, struct fun_info *fi);
bool check_rvo(struct fun_info *fi);
bool fi_has_sret(struct fun_info *fi);

#ifdef __cplusplus
}
#endif

#endif
