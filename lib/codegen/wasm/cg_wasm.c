/*
 * Copyright (C) 2021 Ligang Wang <ligangwangs@gmail.com>
 *
 * wasm codegen functions
 * 
 */
#include "codegen/wasm/cg_wasm.h"
#include "codegen/wasm/wasm_abi.h"
#include "codegen/wasm/wasm_api.h"
#include "clib/array.h"
#include "clib/string.h"
#include "clib/symbol.h"
#include "clib/util.h"
#include "lexer/lexer.h"
#include "parser/amodule.h"
#include "parser/parser.h"
#include "sema/analyzer.h"
#include "sema/type.h"
#include "sema/frontend.h"
#include <assert.h>
#include <stdint.h>
#include <float.h>

const char wasm_magic_number[] = {0, 'a', 's', 'm'};
u8 wasm_version[] = {0x01, 0, 0, 0};
u8 type_2_const[TYPE_TYPES] = {
    /*UNK*/ 0,
    /*GENERIC*/ 0,
    /*UNIT*/ 0,
    /*BOOL*/ OPCODE_I32CONST,
    /*CHAR*/ OPCODE_I32CONST,
    /*INT*/ OPCODE_I32CONST,
    /*FLOAT*/ OPCODE_F32CONST,
    /*DOUBLE*/ OPCODE_F64CONST,
    /*STRING*/ OPCODE_I32CONST,
    /*FUNCTION*/ 0,
    /*STRUCT*/ 0,
    /*UNION*/ 0,
};

u8 type_2_wtype[TYPE_TYPES] = {
    /*UNK*/ 0,
    /*GENERIC*/ WASM_TYPE_I32,
    /*UNIT*/ WASM_TYPE_VOID,
    /*BOOL*/ WASM_TYPE_I32,
    /*CHAR*/ WASM_TYPE_I32,
    /*INT*/ WASM_TYPE_I32,
    /*FLOAT*/ WASM_TYPE_F32,
    /*DOUBLE*/ WASM_TYPE_F64,
    /*STRING*/ WASM_TYPE_I32,
    /*FUNCTION*/ 0,
    /*STRUCT*/ 0,
    /*UNION*/ 0,
};

u8 type_2_store_op[TYPE_TYPES] = {
    /*UNK*/ 0,
    /*GENERIC*/ 0,
    /*UNIT*/ 0,
    /*BOOL*/ OPCODE_I32STORE,
    /*CHAR*/ OPCODE_I32STORE,
    /*INT*/ OPCODE_I32STORE,
    /*FLOAT*/ OPCODE_F32STORE,
    /*DOUBLE*/ OPCODE_F64STORE,
    /*STRING*/ OPCODE_I32STORE,
    /*FUNCTION*/ 0,
    /*STRUCT*/ 0,
    /*UNION*/ 0,
};

u8 op_maps[OP_TOTAL][TYPE_TYPES] = {
    /*
    UNK, GENERIC, UNIT, BOOL, CHAR, INT, FLOAT, DOUBLE, STRING, FUNCTION, STRUCT, UNION     
    */
    /*OP_NULL   */{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,},
    /*OP_DOT   */{0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  0, 0,},
    /*OP_OR     */{0, 0, 0, OPCODE_I32OR, 0, 0, 0, 0, 0, 0,  0, 0,},
    /*OP_AND    */{0, 0, 0, OPCODE_I32AND, 0, 0, 0, 0, 0, 0, 0, 0,},
    /*OP_NOT    */{0, 0, 0, OPCODE_I32XOR, 0, 0, 0, 0, 0, 0, 0, 0,}, //xor 1

    /*OP_BNOT   */{0, 0, 0, OPCODE_I32XOR, OPCODE_I32XOR, OPCODE_I32XOR, 0, 0, 0, 0, 0, 0,},             //xor -1
    /*OP_BOR    */{0, 0, 0, OPCODE_I32OR, OPCODE_I32OR, OPCODE_I32OR, 0, 0, 0, 0, 0, 0,},
    /*OP_BEOR   */{0, 0, 0, OPCODE_I32XOR, OPCODE_I32XOR, OPCODE_I32XOR, 0, 0, 0, 0, 0, 0,},
    /*OP_BAND   */{0, 0, 0, OPCODE_I32AND, OPCODE_I32AND, OPCODE_I32AND, 0, 0, 0, 0, 0, 0,},
    /*OP_BSL    */{0, 0, 0, OPCODE_I32SHL, OPCODE_I32SHL, OPCODE_I32SHL, 0, 0, 0, 0, 0, 0,},
    /*OP_BSR    */{0, 0, 0, OPCODE_I32SHR_S, OPCODE_I32SHR_S, OPCODE_I32SHR_S, 0, 0, 0, 0, 0, 0,},

    /*OP_POW    */{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,},
    /*OP_TIMES  */{0, 0, 0, OPCODE_I32MUL, OPCODE_I32MUL, OPCODE_I32MUL, OPCODE_F32MUL, OPCODE_F64MUL, 0, 0, 0, 0,},
    /*OP_DIV    */{0, 0, 0, OPCODE_I32DIV_S, OPCODE_I32DIV_S, OPCODE_I32DIV_S, OPCODE_F32DIV, OPCODE_F64DIV, 0, 0, 0, 0,},
    /*OP_MOD    */{0, 0, 0, OPCODE_I32REM_S, OPCODE_I32REM_S, OPCODE_I32REM_S, 0, 0, 0, 0, 0, 0,},
    /*OP_PLUS   */{0, 0, 0, OPCODE_I32ADD, OPCODE_I32ADD, OPCODE_I32ADD, OPCODE_F32ADD, OPCODE_F64ADD, 0, 0, 0, 0,},
    /*OP_MINUS  */{0, 0, 0, OPCODE_I32SUB, OPCODE_I32SUB, OPCODE_I32SUB, OPCODE_F32SUB, OPCODE_F64SUB, 0, 0, 0, 0,},

    /*OP_LT   */{0, 0, 0, OPCODE_I32LT_S, OPCODE_I32LT_S, OPCODE_I32LT_S, OPCODE_F32LT, OPCODE_F64LT, 0, 0, 0, 0,},
    /*OP_LE  */{0, 0, 0, OPCODE_I32LE_S, OPCODE_I32LE_S, OPCODE_I32LE_S, OPCODE_F32LE, OPCODE_F64LE, 0, 0, 0, 0,},
    /*OP_EQ    */{0, 0, 0, OPCODE_I32EQ, OPCODE_I32EQ, OPCODE_I32EQ, OPCODE_F32EQ, OPCODE_F64EQ, 0, 0, 0, 0,},
    /*OP_GT    */{0, 0, 0, OPCODE_I32GT_S, OPCODE_I32GT_S, OPCODE_I32GT_S, OPCODE_F32GT, OPCODE_F64GT, 0, 0, 0, 0,},
    /*OP_GE   */{0, 0, 0, OPCODE_I32GE_S, OPCODE_I32GE_S, OPCODE_I32GE_S, OPCODE_F32GE, OPCODE_F64GE, 0, 0, 0, 0,},
    /*OP_NE  */{0, 0, 0, OPCODE_I32NE, OPCODE_I32NE, OPCODE_I32NE, OPCODE_F32NE, OPCODE_F64NE, 0, 0, 0, 0,},

    /*OP_COND  */{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,},

    /*OP_MUL_ASSN   */{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,},
    /*OP_DIV_ASSN  */{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,},
    /*OP_MOD_ASSN    */{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,},
    /*OP_ADD_ASSN    */{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,},
    /*OP_SUB_ASSN   */{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,},
    /*OP_LEFT_ASSN  */{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,},
    /*OP_RIGHT_ASSN    */{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,},
    /*OP_AND_ASSN    */{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,},
    /*OP_XOR_ASSN   */{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,},
    /*OP_OR_ASSN  */{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,},

    /*OP_INC     */{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,},
    /*OP_DEC     */{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,},
} ;


#define DATA_SECTION_START_ADDRESS 1024
#define STACK_BASE_ADDRESS  66592

#define MEMORY_BASE_VAR_INDEX 1

symbol MEMORY = 0;
symbol __MEMORY_BASE = 0;
symbol POW_FUN_NAME = 0;

void _imports_init(struct imports *imports)
{
    imports->import_block = 0;
    imports->num_fun = 0;
    imports->num_global = 0;
    imports->num_memory = 0;
}

void _imports_deinit(struct imports *imports)
{
    ast_node_free(imports->import_block);
    imports->import_block = 0;
    imports->num_fun = 0;
    imports->num_global = 0;
    imports->num_memory = 0;
}

void _cg_wasm_init(struct cg_wasm *cg)
{
    ba_init(&cg->ba, 17);
    hashtable_init_with_value_size(&cg->func_name_2_idx, sizeof(u32), 0);
    hashtable_init(&cg->func_name_2_ast);
    for(u32 i = 0; i < FUN_LEVELS; i++){
        fun_context_init(&cg->fun_contexts[i]);
    }
    _imports_init(&cg->imports);
    cg->sys_block = 0;
    cg->fun_top = 0;
    cg->var_top = 0;
    cg->func_idx = 0;
    cg->data_offset = 0;
    cg->fun_types = block_node_new_empty();
    cg->funs = block_node_new_empty();
    cg->data_block = block_node_new_empty();
}

void _cg_wasm_deinit(struct cg_wasm *cg)
{
    _imports_deinit(&cg->imports);
    ast_node_free(cg->sys_block);
    for (u32 i = 0; i < FUN_LEVELS; i++) {
        fun_context_deinit(&cg->fun_contexts[i]);
    }
    cg->fun_top = 0;
    hashtable_deinit(&cg->func_name_2_ast);
    hashtable_deinit(&cg->func_name_2_idx);
    ba_deinit(&cg->ba);
    if(cg->fun_types){
        free_block_node(cg->fun_types, false);
    }
    if (cg->funs) {
        free_block_node(cg->funs, false);
    }
    if(cg->data_block){
        free_block_node(cg->data_block, false);
    }
}

struct cg_wasm *cg_wasm_new()
{
    struct cg_wasm *cg;
    MALLOC(cg, sizeof(*cg));
    _cg_wasm_init(cg);
    MEMORY = to_symbol("memory");
    __MEMORY_BASE = to_symbol("__memory_base");
    POW_FUN_NAME = to_symbol("pow");
    cg->base.compute_fun_info = wasm_compute_fun_info;
    return cg;
}

void cg_wasm_free(struct cg_wasm *cg)
{
    _cg_wasm_deinit(cg);
    free(cg);
}

struct fun_context *get_top_fun_context(struct cg_wasm *cg)
{
    return cg->fun_top >= 1 ? &cg->fun_contexts[cg->fun_top - 1] : 0;
}

void _emit_literal(struct cg_wasm *cg, struct byte_array *ba, struct ast_node *node)
{
    assert(node->type && node->type->type < TYPE_TYPES && node->type->type >= 0);
    u32 len;
    switch(node->type->type){
        default:
            printf("not expected type: %s\n", string_get(type_symbols[node->type->type]));
            exit(-1);
        case TYPE_CHAR:
        case TYPE_BOOL:
        case TYPE_INT:
            wasm_emit_const_i32(ba, node->liter->int_val);
            break;
        case TYPE_FLOAT:
            wasm_emit_const_f32(ba, node->liter->double_val);
            break;
        case TYPE_DOUBLE:
            wasm_emit_const_f64(ba, node->liter->double_val);
            break;
        case TYPE_STRING:
            len = strlen(node->liter->str_val);
            if(cg->imports.num_memory){
                ba_add(ba, OPCODE_GLOBALGET);
                wasm_emit_uint(ba, MEMORY_BASE_VAR_INDEX);
                if(cg->data_offset){
                    wasm_emit_const_i32(ba, cg->data_offset);
                    ba_add(ba, OPCODE_I32ADD);
                }
                cg->data_offset += len + 1; //null terminated string
            } else {
                ba_add(ba, type_2_const[node->type->type]);
                wasm_emit_uint(ba, DATA_SECTION_START_ADDRESS + cg->data_offset);
                cg->data_offset += wasm_get_emit_size(len) + len;
            }
            block_node_add(cg->data_block, node);
            break;
    }
    
}

void _emit_unary(struct cg_wasm *cg, struct byte_array *ba, struct ast_node *node)
{
    struct ast_node *bin_node = 0;
    symbol s = 0;
    switch (node->unop->opcode){
        default:
            s = get_symbol_by_token_opcode(TOKEN_OP, node->unop->opcode);
            printf("Not implemented unary for : %s\n", string_get(s));
            exit(-1);
        case OP_MINUS:
            bin_node = int_node_new(0, node->loc);
            bin_node->type = node->type;
            wasm_emit_code(cg, ba, bin_node);
            break;
        case OP_NOT:
            bin_node = int_node_new(1, node->loc);
            bin_node->type = node->type;
            wasm_emit_code(cg, ba, bin_node);
            break;
        case OP_BNOT:
            bin_node = int_node_new(-1, node->loc);
            bin_node->type = node->type;
            wasm_emit_code(cg, ba, bin_node);
            break;
        }
    wasm_emit_code(cg, ba, node->unop->operand);
    enum type type_index = prune(node->unop->operand->type)->type;
    assert(type_index >= 0 && type_index < TYPE_TYPES);
    assert(node->unop->opcode >= 0 && node->unop->opcode < OP_TOTAL);
    if (bin_node) {
        u8 opcode = op_maps[node->unop->opcode][type_index];
        if(!opcode){
            symbol s = get_symbol_by_token_opcode(TOKEN_OP, node->unop->opcode);
            printf("No opcode found for op: %s, type: %s\n", string_get(s), string_get(type_symbols[type_index]));
            exit(-1);
        }
        ba_add(ba, opcode);
        ast_node_free(bin_node);
    }
}

void _emit_binary(struct cg_wasm *cg, struct byte_array *ba, struct ast_node *node)
{
    wasm_emit_code(cg, ba, node->binop->lhs);
    wasm_emit_code(cg, ba, node->binop->rhs);
    enum type type_index = prune(node->binop->lhs->type)->type;
    assert(type_index >= 0 && type_index < TYPE_TYPES);
    assert(node->binop->opcode >= 0 && node->binop->opcode < OP_TOTAL);
    if (node->binop->opcode != OP_POW){
        u8 opcode = op_maps[node->binop->opcode][type_index];
        if(!opcode){
            symbol s = get_symbol_by_token_opcode(TOKEN_OP, node->binop->opcode);
            printf("No opcode found for op: %s, type: %s\n", string_get(s), string_get(type_symbols[type_index]));
            exit(-1);
        }
        ba_add(ba, opcode);
    }else{
        //call pow function
        u32 func_index = hashtable_get_int(&cg->func_name_2_idx, POW_FUN_NAME);
        ba_add(ba, OPCODE_CALL);
        wasm_emit_uint(ba, func_index);
    }
}

bool is_variadic_call_with_optional_arguments(struct cg_wasm *cg, struct ast_node *node)
{
    struct ast_node *fun_type = hashtable_get_p(&cg->func_name_2_ast, node->call->specialized_callee ? node->call->specialized_callee : node->call->callee);
    return fun_type->ft->is_variadic && array_size(&node->call->arg_block->block->nodes) >= array_size(&fun_type->ft->params->block->nodes);
}

void _emit_const_zero(struct byte_array* ba, enum  type type)
{
    if(is_int_type(type)){
        wasm_emit_const_i32(ba, 0);
    }else{
        wasm_emit_const_f64(ba, 0.0);
    }
}

void _emit_if(struct cg_wasm *cg, struct byte_array *ba, struct ast_node *node)
{
    assert(node->node_type == IF_NODE);
    wasm_emit_code(cg, ba, node->cond->if_node);
    ba_add(ba, OPCODE_IF);
    ASSERT_TYPE(node->cond->then_node->type->type);
    ba_add(ba, type_2_wtype[node->cond->then_node->type->type]);
    wasm_emit_code(cg, ba, node->cond->then_node);
    if (node->cond->else_node) {
        ba_add(ba, OPCODE_ELSE);
        wasm_emit_code(cg, ba, node->cond->else_node);
    }
    ba_add(ba, OPCODE_END);
}

void _emit_if_local_var_ge_zero(struct byte_array *ba, u32 var_index, enum type type)
{
    ba_add(ba, OPCODE_LOCALGET);
    wasm_emit_uint(ba, var_index);
    _emit_const_zero(ba, type);
    ba_add(ba, op_maps[OP_GE][type]);
    ba_add(ba, OPCODE_IF);
    ba_add(ba, WASM_TYPE_I32);
}

void _emit_loop(struct cg_wasm *cg, struct byte_array *ba, struct ast_node *node)
{
    assert(node->node_type == FOR_NODE);
    u32 var_index = func_context_get_var_index(cg, node->forloop->var_name);
    u32 step_index = func_get_local_var_index(cg, node->forloop->step);
    u32 end_index = func_get_local_var_index(cg, node->forloop->end);
    enum type type = node->forloop->end->type->type;
    enum type body_type = node->forloop->body->type->type;
    ASSERT_TYPE(type);
    ASSERT_TYPE(body_type);
    // initializing start value
    wasm_emit_code(cg, ba, node->forloop->start);
    ba_add(ba, OPCODE_LOCALSET);
    wasm_emit_uint(ba, var_index);  //1
    // set step value
    wasm_emit_code(cg, ba, node->forloop->step);
    ba_add(ba, OPCODE_LOCALSET);
    wasm_emit_uint(ba, step_index); //2
    // set end value
    assert(node->forloop->end->node_type == BINARY_NODE);
    wasm_emit_code(cg, ba, node->forloop->end->binop->rhs);
    ba_add(ba, OPCODE_LOCALSET);
    wasm_emit_uint(ba, end_index);  //3

    ba_add(ba, OPCODE_BLOCK); // outside block branch labelidx 1
    ba_add(ba, WASM_TYPE_VOID); // type_2_wtype[body_type]); // branch type

    ba_add(ba, OPCODE_LOOP);  // loop branch, branch labelidx 0
    ba_add(ba, WASM_TYPE_VOID); //type_2_wtype[body_type]); // branch type

    //if step >= 0
    _emit_if_local_var_ge_zero(ba, step_index, node->forloop->step->type->type);
    //branch body
    //1. get var value
    //nested a if branch
    ba_add(ba, OPCODE_LOCALGET);
    wasm_emit_uint(ba, var_index);
    ba_add(ba, OPCODE_LOCALGET);
    wasm_emit_uint(ba, end_index);
    ba_add(ba, op_maps[OP_GE][type]);

    ba_add(ba, OPCODE_ELSE);

    ba_add(ba, OPCODE_LOCALGET);
    wasm_emit_uint(ba, var_index);
    ba_add(ba, OPCODE_LOCALGET);
    wasm_emit_uint(ba, end_index);
    ba_add(ba, op_maps[OP_LE][type]);
    ba_add(ba, OPCODE_END); 
    //end of if step >= 0

    ba_add(ba, OPCODE_BR_IF); //if out of scope, branch to out side block
    wasm_emit_uint(ba, 1);

    //body
    wasm_emit_code(cg, ba, node->forloop->body);
    //var += step
    ba_add(ba, OPCODE_LOCALGET);
    wasm_emit_uint(ba, var_index);
    ba_add(ba, OPCODE_LOCALGET);
    wasm_emit_uint(ba, step_index);
    ba_add(ba, op_maps[OP_PLUS][type]);
    ba_add(ba, OPCODE_LOCALSET);
    wasm_emit_uint(ba, var_index);
    ba_add(ba, OPCODE_BR); //branch to loop again
    wasm_emit_uint(ba, 0); 
    ba_add(ba, OPCODE_END); //end of loop branch
    ba_add(ba, OPCODE_END); //end of outside branch
}

void _emit_ident(struct cg_wasm *cg, struct byte_array *ba, struct ast_node *node)
{
    assert(node->node_type == IDENT_NODE);
    u32 var_index = func_context_get_var_index(cg, node->ident->name);
    //TODO: var_index zero better is not matched
    ba_add(ba, OPCODE_LOCALGET); // num local variables
    wasm_emit_uint(ba, var_index);
}

void _emit_block(struct cg_wasm *cg, struct byte_array *ba, struct ast_node *node)
{   
    u32 block_size = array_size(&node->block->nodes);
    for(u32 i = 0; i < block_size; i++){
        struct ast_node *child = *(struct ast_node **)array_get(&node->block->nodes, i);
        wasm_emit_code(cg, ba, child);
    }
}

void wasm_emit_code(struct cg_wasm *cg, struct byte_array *ba, struct ast_node *node)
{
    switch(node->node_type){
        case FUNC_NODE:
            wasm_emit_func(cg, ba, node);
            break;
        case BLOCK_NODE:
            _emit_block(cg, ba, node);
            break;
        case BINARY_NODE:
            _emit_binary(cg, ba, node);
            break;
        case UNARY_NODE:
            _emit_unary(cg, ba, node);
            break;
        case LITERAL_NODE:
            _emit_literal(cg, ba, node);
            break;
        case CALL_NODE:
            wasm_emit_call(cg, ba, node);
            break;
        case IDENT_NODE:
            _emit_ident(cg, ba, node);
            break;
        case VAR_NODE:
            wasm_emit_var(cg, ba, node);
            break;
        case IF_NODE:
            _emit_if(cg, ba, node);
            break;
        case FOR_NODE:
            _emit_loop(cg, ba, node);
            break;
        case STRUCT_NODE:
            wasm_emit_struct(cg, ba, node);
            break;
        case STRUCT_INIT_NODE:
            wasm_emit_struct_init(cg, ba, node);
            break;
        default:
            printf("%s is not implemented !\n", node_type_strings[node->node_type]);
            exit(-1);
    }
}

void _append_section(struct byte_array *ba, struct byte_array *section)
{
    wasm_emit_uint(ba, section->size); // set size
    ba_add2(ba, section); // copy data
    ba_reset(section);
}

void _emit_type_section(struct cg_wasm *cg, struct byte_array *ba, struct ast_node *block)
{
    u32 func_types = array_size(&block->block->nodes);
    wasm_emit_uint(ba, func_types);
    struct ast_node *func;
    u32 i, j;
    struct type_oper *to;
    for (i = 0; i < func_types; i++) {
        func = *(struct ast_node **)array_get(&block->block->nodes, i);
        struct type_oper *func_type = (struct type_oper *)func->type;
        u32 num_params = array_size(&func_type->args) - 1;
        ba_add(ba, TYPE_FUNC);
        wasm_emit_uint(ba, num_params); // num params
        for (j = 0; j < num_params; j++) {
            to = *(struct type_oper **)array_get(&func_type->args, j);
            ASSERT_TYPE(to->base.type);
            ba_add(ba, type_2_wtype[to->base.type]);
        }
        to = *(struct type_oper **)array_back(&func_type->args);
        ASSERT_TYPE(to->base.type);
        if (to->base.type == TYPE_UNIT) {
            ba_add(ba, 0); // num result
        } else {
            ba_add(ba, 1); // num result
            ba_add(ba, type_2_wtype[to->base.type]); // i32 output
        }
    }
}

void _emit_import_section(struct cg_wasm *cg, struct byte_array *ba, struct ast_node *block) 
{
    u32 num_imports = array_size(&block->block->nodes);
    wasm_emit_uint(ba, num_imports); // number of imports
    u32 type_index = 0;
    for(u32 i = 0; i < array_size(&block->block->nodes); i++){
        struct ast_node *node = *(struct ast_node **)array_get(&block->block->nodes, i);
        assert(node->node_type == IMPORT_NODE);
        wasm_emit_string(ba, node->import->from_module);
        node = node->import->import;
        switch(node->node_type){
        default:
            printf("%s node is not allowed in import section", node_type_strings[node->node_type]);
            exit(-1);
            break;
        case FUNC_TYPE_NODE:
            wasm_emit_string(ba, node->ft->name);
            ba_add(ba, IMPORT_FUNC);
            wasm_emit_uint(ba, type_index++); //type index
            break;
        case VAR_NODE:
            wasm_emit_string(ba, node->var->var_name);
            ba_add(ba, IMPORT_GLOBAL);
            ASSERT_TYPE(node->type->type);
            ba_add(ba, type_2_wtype[node->type->type]);
            if (__MEMORY_BASE == node->var->var_name)
                ba_add(ba, GLOBAL_CONST); // immutable
            else{
                ba_add(ba, GLOBAL_VAR); // mutable
            }
            break;
        case MEMORY_NODE:
            wasm_emit_string(ba, MEMORY);
            ba_add(ba, IMPORT_MEMORY);
            if(node->memory->max){
                ba_add(ba, LIMITS_MIN_MAX);
                wasm_emit_uint(ba, node->memory->initial->liter->int_val);
                wasm_emit_uint(ba, node->memory->max->liter->int_val);
            }else{
                ba_add(ba, LIMITS_MIN_ONLY);
                wasm_emit_uint(ba, node->memory->initial->liter->int_val);
            }
            break;
        }
    }
}

void _emit_function_section(struct cg_wasm *cg, struct byte_array *ba, struct ast_node *block)
{
    u32 num_func = array_size(&block->block->nodes);
    ba_add(ba, num_func); // num functions
    for (u32 i = 0; i < num_func; i++) {
        wasm_emit_uint(ba, i + cg->imports.num_fun); // function index
    }
}

void _emit_memory_section(struct cg_wasm *cg, struct byte_array *ba)
{
    ba_add(ba, 1); // num memories
    ba_add(ba, LIMITS_MIN_MAX);
    ba_add(ba, 2);//min 2x64k
    ba_add(ba, 10);//max 10x64k
}

void _emit_global_section(struct cg_wasm *cg, struct byte_array *ba)
{
    //__stack_pointer: base address
    ba_add(ba, 1);  //num globals
    ba_add(ba, WASM_TYPE_I32);
    ba_add(ba, GLOBAL_VAR); //mutable
    wasm_emit_const_i32(ba, STACK_BASE_ADDRESS);
    ba_add(ba, OPCODE_END);
}

void _emit_export_section(struct cg_wasm *cg, struct byte_array *ba, struct ast_node *block)
{
    u32 num_func = array_size(&block->block->nodes);
    wasm_emit_uint(ba, num_func + 1); // num of function exports plus 1 memory
    struct ast_node *func;
    for (u32 i = 0; i < num_func; i++) {
        func = *(struct ast_node **)array_get(&block->block->nodes, i);
        wasm_emit_string(ba, func->func->func_type->ft->name);
        ba_add(ba, EXPORT_FUNC);
        wasm_emit_uint(ba, i + cg->imports.num_fun); // func index
    }
    wasm_emit_string(ba, MEMORY);
    ba_add(ba, EXPORT_MEMORY);
    wasm_emit_uint(ba, 0); //export memory 0
}

void _emit_code_section(struct cg_wasm *cg, struct byte_array *ba, struct ast_node *block)
{
    u32 num_func = array_size(&block->block->nodes);
    wasm_emit_uint(ba, num_func); // num functions
    wasm_emit_code(cg, ba, block);
}

void _emit_data_section(struct cg_wasm *cg, struct byte_array *ba, struct ast_node *block)
{
    u32 num_data = array_size(&block->block->nodes);
    wasm_emit_uint(ba, 1); //1 data segment
    wasm_emit_uint(ba, DATA_ACTIVE);
    // offset of memory
    if (cg->imports.num_memory){
        ba_add(ba, OPCODE_GLOBALGET);
        wasm_emit_uint(ba, MEMORY_BASE_VAR_INDEX);
    }else{
        ba_add(ba, OPCODE_I32CONST);
        wasm_emit_uint(ba, DATA_SECTION_START_ADDRESS);
    }
    ba_add(ba, OPCODE_END);
    wasm_emit_uint(ba, cg->data_offset);
    for (u32 i = 0; i < num_data; i++) {
        struct ast_node *node = *(struct ast_node**)array_get(&block->block->nodes, i);
        assert(node->node_type == LITERAL_NODE);
        //data array size and content
        u32 str_length = strlen(node->liter->str_val);
        if (cg->imports.num_memory) {
            wasm_emit_null_terminated_string(ba, node->liter->str_val, str_length);
        } else {
            wasm_emit_chars(ba, node->liter->str_val, str_length);
        }
    }
}

void wasm_emit_module(struct cg_wasm *cg, struct ast_node *node)
{
    assert(node->node_type == BLOCK_NODE);
    struct byte_array section;
    struct byte_array *ba = &cg->ba;    
    ba_init(&section, 17);
    for(size_t i = 0; i < ARRAY_SIZE(wasm_magic_number); i++){
        ba_add(ba, wasm_magic_number[i]);
    }
    for(size_t i = 0; i < ARRAY_SIZE(wasm_version); i++){
        ba_add(ba, wasm_version[i]);
    }
    // 0.    custom section
    // 1.    type section       : type signature of the function
    // 2.    import section
    // 3.    function section   : associate code section with type section
    // 4.    table section
    // 5.    memory section
    // 6.    global section
    // 7.    export section
    // 8.    start section
    // 9.    element section
    // 10.   code section       : local variable info and bytecode of function
    // 11.   data section
    // 12.   data count section
    // type section
    ba_add(ba, TYPE_SECTION);       // code: 1
    _emit_type_section(cg, &section, cg->fun_types);
    _append_section(ba, &section);
    // import section
    ba_add(ba, IMPORT_SECTION);     // code: 2
    _emit_import_section(cg, &section, cg->imports.import_block);
    _append_section(ba, &section);

    // function section
    ba_add(ba, FUNCTION_SECTION);   // code: 3
    _emit_function_section(cg, &section, cg->funs);
    _append_section(ba, &section);

    // table section                // code: 4
    // memory section               // code: 5
    if(!cg->imports.num_memory){
        ba_add(ba, MEMORY_SECTION);
        _emit_memory_section(cg, &section);
        _append_section(ba, &section);
    }

    // global section               // code: 6
    if(!cg->imports.num_global){
        ba_add(ba, GLOBAL_SECTION);
        _emit_global_section(cg, &section);
        _append_section(ba, &section);
    }
    // export section               // code: 7
    ba_add(ba, EXPORT_SECTION); 
    _emit_export_section(cg, &section, cg->funs);
    _append_section(ba, &section);

    // start section                // code: 8
    // element section              // code: 9

    // data count section           // code: 12, data count must before code section
    if (array_size(&cg->data_block->block->nodes)) {
        ba_add(ba, DATA_COUNT_SECTION); 
        wasm_emit_uint(ba, 1); //   data count size
        wasm_emit_uint(ba, 1); //   data count
    }
    // code section                 // code: 10
    ba_add(ba, CODE_SECTION); 
    _emit_code_section(cg, &section, cg->funs);
    _append_section(ba, &section);

    // data section                 // code: 11
    if(array_size(&cg->data_block->block->nodes)){
        ba_add(ba, DATA_SECTION);
        _emit_data_section(cg, &section, cg->data_block);
        _append_section(ba, &section);
    }

    // custom secion                // code: 0
    ba_deinit(&section);
}
