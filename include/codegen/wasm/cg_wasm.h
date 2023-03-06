/*
 * cg_wasm.h
 * 
 * Copyright (C) 2021 Ligang Wang <ligangwangs@gmail.com>
 *
 * header file for WASM Binary Format codegen
 */
#ifndef __MLANG_CG_WASM_H__
#define __MLANG_CG_WASM_H__

#include "clib/byte_array.h"
#include "clib/hashtable.h"
#include "clib/symbol.h"
#include "clib/symboltable.h"
#include "codegen/fun_info.h"
#include "codegen/fun_context.h"
#include "codegen/codegen.h"
#include "parser/ast.h"
#include <assert.h>

#ifdef __cplusplus
extern "C" {
#endif

#include "wasm-core.h"


#define FUN_LEVELS 512
#define LOCAL_VARS 1024 //TODO: need to eliminate this limitation

struct imports{
    struct ast_node *import_block;
    u32 num_global;
    u32 num_fun;
    u32 num_memory;
};

struct cg_wasm {
    struct codegen base;

    struct byte_array ba;
    struct hashtable func_name_2_idx;
    struct hashtable func_name_2_ast;
    /*
     *  symboltable of <symbol, struct fun_context>
     *  binding variable name to index of variable in the function
     *  used in function codegen
     */
    struct fun_context fun_contexts[FUN_LEVELS];
    struct var_info local_vars[LOCAL_VARS];

    u32 fun_top;
    u32 var_top;
    u32 func_idx;

    struct imports imports;

    /*
     * sys block, like cf64 complex type definition
     */
    struct ast_node *sys_block;
    /*
     * function types including imports function type, and those in fun definitions
     */
    struct ast_node *fun_types;

    /*
     * function definitions
     */
    struct ast_node *funs;

    /*
     * data section, for example: string literal
     */
    struct ast_node *data_block;

    u32 data_offset;
};
extern u8 type_2_store_op[TYPE_TYPES];
extern u8 type_2_wtype[TYPE_TYPES];
extern u8 type_2_load_op[TYPE_TYPES];

#define ASSERT_TYPE(type_index) assert(type_index > TYPE_NULL && type_index < TYPE_TYPES);

struct cg_wasm * cg_wasm_new(struct sema_context *context);
void wasm_emit_module(struct cg_wasm *cg, struct ast_node *node);
void wasm_emit_code(struct cg_wasm *cg, struct byte_array *ba, struct ast_node *node);
void wasm_emit_call(struct cg_wasm *cg, struct byte_array *ba, struct ast_node *node);
void wasm_emit_func(struct cg_wasm *cg, struct byte_array *ba, struct ast_node *node);
void wasm_emit_var(struct cg_wasm *cg, struct byte_array *ba, struct ast_node *node);
void wasm_emit_var_change(struct cg_wasm *cg, struct byte_array *ba, u32 var_index, bool is_global, u8 op, u32 elm_size, struct ast_node* offset_index);
void wasm_emit_array_init(struct cg_wasm *cg, struct byte_array *ba, struct ast_node *node);
void wasm_emit_adt_init(struct cg_wasm *cg, struct byte_array *ba, struct ast_node *node);
void wasm_emit_store_scalar_value_at(struct cg_wasm *cg, struct byte_array *ba, u32 local_address_var_index, u32 align, u32 offset, struct ast_node *node);
void wasm_emit_store_scalar_value(struct cg_wasm *cg, struct byte_array *ba, u32 align, u32 offset, struct ast_node *node);
void wasm_emit_store_record_value(struct cg_wasm *cg, struct byte_array *ba, u32 local_address_var_index, u32 offset, struct struct_layout *sl, struct ast_node *block);
void wasm_emit_store_array_value(struct cg_wasm *cg, struct byte_array *ba, u32 local_address_var_index, u32 offset, u32 elm_align, u32 elm_type_size, struct ast_node *array_init);
void wasm_emit_addr_offset_by_expr(struct cg_wasm *cg, struct byte_array *ba, u32 var_index, bool is_global, struct ast_node *offset_expr);

struct fun_context *cg_get_top_fun_context(struct cg_wasm *cg);

void cg_wasm_free(struct cg_wasm *cg);
bool is_variadic_call_with_optional_arguments(struct cg_wasm *cg, struct ast_node *node);

void fc_init(struct fun_context *fc);
void fc_deinit(struct fun_context *fc);
void func_register_local_variable(struct cg_wasm *cg, struct ast_node *node, bool is_local_var);
void collect_local_variables(struct cg_wasm *cg, struct ast_node *node);

#ifdef __cplusplus
}
#endif

#endif //__MLANG_WASM_CODEGEN_H__
