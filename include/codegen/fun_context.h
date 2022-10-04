/*
 * fun_context.h
 * 
 * Copyright (C) 2022 Ligang Wang <ligangwangs@gmail.com>
 *
 * header file defining function conext in generating target functions
 */

#ifndef __MLANG_FUN_CONTEXT_H__
#define __MLANG_FUN_CONTEXT_H__

#include "clib/hashtable.h"
#include "clib/array.h"
#include "parser/ast.h"
#include "sema/type.h"

#ifdef __cplusplus
extern "C" {
#endif

enum MemType{
    Stack = 0,
    Heap
};

struct mem_alloc{
    u32 address;
    u32 size;
    enum MemType mem_type;
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
     *  hashtable of <struct ast_node *, u32>
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
     *  allocs: memory allocations supporting stack and heap memory, array of (struct mem_alloc)
     */
    struct array allocs; 
};

void fun_context_init(struct fun_context *fc);
void fun_context_deinit(struct fun_context *fc);

#ifdef __cplusplus
}
#endif

#endif
