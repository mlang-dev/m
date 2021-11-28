/*
 * wasm_codegen.h
 * 
 * Copyright (C) 2021 Ligang Wang <ligangwangs@gmail.com>
 *
 * header file for WASM IR Text Format codegen
 */
#ifndef __MLANG_WASM_CODEGEN_H__
#define __MLANG_WASM_CODEGEN_H__

#include "parser/ast_node.h"

#ifdef __cplusplus
extern "C" {
#endif

void wat_codegen_init();
string wat_generate(struct ast_node *ast, const char *text);

#ifdef __cplusplus
}
#endif

#endif //__MLANG_CODEGEN_H__
