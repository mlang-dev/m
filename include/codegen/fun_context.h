/*
 * fun_context.h
 * 
 * Copyright (C) 2022 Ligang Wang <ligangwangs@gmail.com>
 *
 * header file defining function conext used in generating target functions
 */

#ifndef __MLANG_FUN_CONTEXT_H__
#define __MLANG_FUN_CONTEXT_H__

#include "clib/hashtable.h"
#include "clib/array.h"
#include "parser/ast.h"
#include "sema/type.h"
#include "codegen/type_size_info.h"

#ifdef __cplusplus
extern "C" {
#endif


struct var_info{
    u32 var_index;   //local variable index
    int alloc_index; // stack alloc index, -1: indicating no stack space required
    u8 target_type;  //wasm type
};

struct fun_context {
    struct ast_node* fun;
    /*
     *  symboltable of <symbol, u32>
     *  binding variable name to index of variable in the function
     *  used in function codegen
     */
    struct symboltable varname_2_index;

    /*
     *  hashtable of <struct ast_node *, struct var_info>
     *  binding ast_node pointer to index of local variable in the function
     *  used in function codegen
     */
    struct hashtable ast_2_index;

    /*
     *  number of local variables, including number of local params
     */
    u32 local_vars;

    /*
     *  number of local params
     */
    u32 local_params;

    /*
     *  stack type: an anonymous struct type for stack allocated local variables
     */
    struct type_expr stack_type; //
    struct type_size_info stack_size_info;


    /*
     *  function's stack pointer, saved to local variable
     */
    struct var_info *local_sp;
};

void fc_init(struct fun_context *fc);
void fc_deinit(struct fun_context *fc);
int fc_register_alloc(struct fun_context *fc, struct type_expr *struct_type);

struct var_info *fc_get_var_info(struct fun_context *fc, struct ast_node *node);
i32 fc_get_stack_offset(struct fun_context *fc, struct ast_node *node);
struct struct_layout *fc_get_stack_sl(struct fun_context *fc, struct ast_node *node);

u32 fc_get_stack_size(struct fun_context *fc);

#ifdef __cplusplus
}
#endif

#endif
