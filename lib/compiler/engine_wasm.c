/*
 * Copyright (C) 2022 Ligang Wang <ligangwangs@gmail.com>
 *
 * compiler engine targeting wasm
 */
#include "compiler/engine.h"
#include "codegen/wasm/cg_wasm.h"
#include "sema/analyzer.h"
#include <assert.h>

const char *g_imports = "\n\
from sys import memory 10\n\
from sys import __stack_pointer:int\n\
from sys import __memory_base:int\n\
from sys import fun print:() fmt:string ...\n\
from sys import fun putchar:() ch:int\n\
from math import fun acos:double x:double\n\
from math import fun asin:double x:double\n\
from math import fun atan:double x:double\n\
from math import fun atan2:double y:double x:double\n\
from math import fun cos:double x:double\n\
from math import fun sin:double x:double\n\
from math import fun sinh:double x:double\n\
from math import fun tanh:double x:double\n\
from math import fun exp:double x:double\n\
from math import fun log:double x:double\n\
from math import fun log10:double x:double\n\
from math import fun pow:double x:double y:double\n\
from math import fun sqrt:double x:double\n\
";

const char *g_sys = "\n\
struct zf64 = re:double, im:double\n\
";

void *_cg_wasm_new(struct sema_context *context)
{
    return cg_wasm_new();
}

void _cg_wasm_free(void *cg)
{
    cg_wasm_free(cg);
}

void _categorize_imports(struct imports *imports)
{
    for (u32 i = 0; i < array_size(&imports->import_block->block->nodes); i++) {
        struct ast_node *node = *(struct ast_node **)array_get(&imports->import_block->block->nodes, i);
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


/*
 * collect global statements into _start function
 */
struct ast_node *_decorate_as_module(struct cg_wasm *cg, struct hashtable *symbol_2_int_types, struct ast_node *block)
{
    struct ast_node *node, *sp_func;
    assert(block->node_type == BLOCK_NODE);
    struct ast_node *_start_block = block_node_new_empty();
    u32 nodes = array_size(&block->block->nodes);
    struct ast_node *wmodule = block_node_new_empty();
    for (u32 i = 0; i < nodes; i++) {
        node = *(struct ast_node **)array_get(&block->block->nodes, i);
        if (node->node_type == FUNC_NODE){
            if (is_generic(node->type)){
                for(u32 j = 0; j < array_size(&node->func->sp_funs); j++){
                    sp_func = *(struct ast_node **)array_get(&node->func->sp_funs, j);
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
        } else {
            block_node_add(_start_block, node);
        }
    }
    struct ast_node *_start_func = wrap_nodes_as_function(symbol_2_int_types, to_symbol("_start"), _start_block);
    if(array_size(&_start_block->block->nodes)){
        struct ast_node *ret = *(struct ast_node **)array_back(&_start_block->block->nodes);
        struct type_expr *ret_type = prune(ret->type);
        assert(ret_type->kind == KIND_OPER);
        _start_func->type = (struct type_expr *)wrap_as_fun_type((struct type_oper *)ret_type);
        _start_func->func->func_type->type = _start_func->type;
    }
    block_node_add(wmodule, _start_func);
    block_node_add(cg->fun_types, _start_func->func->func_type);
    block_node_add(cg->funs, _start_func);
    hashtable_set_int(&cg->func_name_2_idx, _start_func->func->func_type->ft->name, cg->func_idx++);
    hashtable_set_p(&cg->func_name_2_ast, _start_func->func->func_type->ft->name, _start_func->func->func_type);
    free_block_node(block, false);
    return wmodule;
}

void compile_to_wasm(struct engine *engine, const char *expr)
{
    struct cg_wasm *cg = (struct cg_wasm*)engine->be->cg;
    struct ast_node *expr_ast = parse_code(engine->fe->parser, expr);
    if (!expr_ast){
        return;
    }
    struct ast_node *ast = node_copy(cg->sys_block);
    block_node_add_block(ast, cg->imports.import_block);
    block_node_add_block(ast, expr_ast);
    free_block_node(expr_ast, false);
    analyze(engine->fe->sema_context, ast);
    ast = _decorate_as_module(cg, &engine->fe->parser->symbol_2_int_types, ast);
    wasm_emit_module(cg, ast);
    ast_node_free(ast);
}
