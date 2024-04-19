/*
 * engine.h
 * 
 * Copyright (C) 2022 Ligang Wang <ligangwangs@gmail.com>
 *
 * header file for compiler engine
 */
#ifndef __MLANG_ENGINE_H__
#define __MLANG_ENGINE_H__

#include "sema/frontend.h"
#include "codegen/backend.h"

#ifdef __cplusplus
extern "C" {
#endif

struct engine{
    struct frontend *fe;
    struct backend *be;
    char *(*emit_ir_string)(void*, struct ast_node *);
    void* (*create_ir_module)(void*, const char *module_name);
};

struct engine *engine_llvm_new(const char *sys_path, bool is_repl);
struct engine *engine_mlir_new(const char *sys_path, bool is_repl);
struct engine *engine_wasm_new(void);
void engine_reset(struct engine *engine);
u8* compile_to_wasm(struct engine *cg, const char *expr);
const char *engine_version(void);
void engine_free(struct engine *engine);



#ifdef __cplusplus
}
#endif

#endif
