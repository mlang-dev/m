/*
 * Copyright (C) 2022 Ligang Wang <ligangwangs@gmail.com>
 *
 * compiler engine targeting wasm
 */
#include "compiler/engine.h"
#include "codegen/wasm/cg_wasm.h"
#include "sema/analyzer.h"
#include "error/error.h"
#include <assert.h>

const char *g_imports = "\n\
from sys import memory 10\n\
from sys import __stack_pointer:int\n\
from sys import __memory_base:int\n\
from sys import func print fmt:string ... -> ()\n\
from sys import func putchar ch:int -> ()\n\
from sys import func setImageData data:u32 width:u32 height:u32 -> ()\n\
from math import func pow x:f64 y:f64 -> f64\n\
from math import func log x:f64 -> f64\n\
from math import func log2 x:f64 -> f64\n\
";

const char *g_sys = "\n\
struct cf64 = re:f64, im:f64\n\
";

struct codegen *_cg_wasm_new(struct sema_context *context)
{
    return (struct codegen *)cg_wasm_new(context);
}

void _cg_wasm_free(struct codegen *cg)
{
    cg_wasm_free((struct cg_wasm *)cg);
}

void _categorize_imports(struct imports *imports)
{
    if(!imports->import_block) return;
    for (u32 i = 0; i < array_size(&imports->import_block->block->nodes); i++) {
        struct ast_node *node = array_get_ptr(&imports->import_block->block->nodes, i);
        assert(node->node_type == IMPORT_NODE);
        node = node->import->import;
        if (node->node_type == FUNC_TYPE_NODE) {
            imports->num_fun ++;
        }
        else if(node->node_type == MEMORY_NODE){
            imports->num_memory ++;
        }
        else if(node->node_type == VAR_NODE){
            imports->num_global ++;
        }
    }
}

struct engine *engine_wasm_new()
{
    struct engine *engine;
    MALLOC(engine, sizeof(*engine));
    engine->fe = frontend_init();
    engine->be = backend_init(engine->fe->sema_context, _cg_wasm_new, _cg_wasm_free);
    struct cg_wasm *cg = engine->be->cg;
    cg->imports.import_block = parse_code(engine->fe->parser, g_imports);
    cg->sys_block = parse_code(engine->fe->parser, g_sys);
    _categorize_imports(&cg->imports);
    return engine;
}


#define IS_OUT_OF_FUNC(node_type)  (node_type == FUNC_NODE || node_type == WasmImportTypeFunc || node_type == STRUCT_NODE)
/*
 * collect global statements into _start function
 */
struct ast_node *_start_func_node(struct cg_wasm *cg, struct hashtable *symbol_2_int_types, struct ast_node *expr_ast, struct ast_node *others)
{
    u32 nodes = array_size(&expr_ast->block->nodes);
    struct ast_node *node;
    struct ast_node *_start_block = block_node_new_empty();
    for (u32 i = 0; i < nodes; i++) {
        node = array_get_ptr(&expr_ast->block->nodes, i);
        if (IS_OUT_OF_FUNC(node->node_type)){
            block_node_add(others, node);
        } else {
            block_node_add(_start_block, node);
        }
    } 
    return wrap_nodes_as_function(symbol_2_int_types, to_symbol("_start"), _start_block);
}

struct ast_node *_decorate_as_module(struct cg_wasm *cg, struct hashtable *symbol_2_int_types, struct ast_node *block)
{
    struct ast_node *node, *sp_func;
    assert(block->node_type == BLOCK_NODE);
    u32 nodes = array_size(&block->block->nodes);
    struct ast_node *wmodule = block_node_new_empty();
    for (u32 i = 0; i < nodes; i++) {
        node = array_get_ptr(&block->block->nodes, i);
        if (node->node_type == FUNC_NODE){
            if (is_generic(node->type)){
                for(u32 j = 0; j < array_size(&node->func->sp_funs); j++){
                    sp_func = array_get_ptr(&node->func->sp_funs, j);
                    block_node_add(wmodule, sp_func);
                    block_node_add(cg->fun_types, sp_func->func->func_type);
                    block_node_add(cg->funs, sp_func);
                    hashtable_set_int(&cg->func_name_2_idx, sp_func->func->func_type->ft->name, cg->func_idx++);
                    hashtable_set_p(&cg->func_name_2_ast, sp_func->func->func_type->ft->name, sp_func->func->func_type);
                }
            }else{
                block_node_add(wmodule, node);
                block_node_add(cg->fun_types, node->func->func_type);
                block_node_add(cg->funs, node);
                hashtable_set_int(&cg->func_name_2_idx, node->func->func_type->ft->name, cg->func_idx++);
                hashtable_set_p(&cg->func_name_2_ast, node->func->func_type->ft->name, node->func->func_type);
            }
        } else if(node->node_type == IMPORT_NODE){
            node = node->import->import;
            if(node->node_type == FUNC_TYPE_NODE){
                block_node_add(cg->fun_types, node);
                hashtable_set_int(&cg->func_name_2_idx, node->ft->name, cg->func_idx++);
                hashtable_set_p(&cg->func_name_2_ast, node->ft->name, node);
            }
        } else if(node->node_type == STRUCT_NODE){
            block_node_add(wmodule, node);
        } else {
            assert(false);
        }
    }
    return wmodule;
}

u8* compile_to_wasm(struct engine *engine, const char *expr)
{
    struct cg_wasm *cg = (struct cg_wasm*)engine->be->cg;
    struct ast_node *expr_ast = parse_code(engine->fe->parser, expr);
    if (!expr_ast){
        return 0;
    }
    struct ast_node *user_global_block = block_node_new_empty();
    struct ast_node *user_start_func = _start_func_node(cg, &engine->fe->parser->symbol_2_int_types, expr_ast, user_global_block);
    free_block_node(expr_ast, false);
    struct ast_node *ast_block = block_node_new_empty();
    block_node_add_block(ast_block, cg->sys_block);
    block_node_add_block(ast_block, cg->imports.import_block);
    block_node_add_block(ast_block, user_global_block);
    block_node_add(ast_block, user_start_func);
    analyze(engine->fe->sema_context, ast_block);
    struct error_report *er = get_last_error_report(engine->fe->sema_context);
    if(er){
        printf("%s loc (line, col): (%d, %d)\n", er->error_msg, er->loc.line, er->loc.col);
        goto exit;
    }
    struct ast_node *ast = _decorate_as_module(cg, &engine->fe->parser->symbol_2_int_types, ast_block);
    wasm_emit_module(cg, ast);
    free_block_node(ast, false);
exit:
    free_block_node(ast_block, false);
    node_free(user_global_block);
    node_free(user_start_func);
    return cg->ba.data;
}
