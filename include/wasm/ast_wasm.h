/*
 * parser.h
 * 
 * Copyright (C) 2021 Ligang Wang <ligangwangs@gmail.com>
 *
 * header file for functions to convert ast node to wasm data structures
 */
#ifndef __MLANG_AST_WASM_H__
#define __MLANG_AST_WASM_H__
#include <stddef.h>
#include <stdint.h>
#include "parser/amodule.h"

#ifdef __cplusplus
extern "C" {
#endif

struct wasm_module *to_wasm_module(struct amodule *amod);

#ifdef __cplusplus
}
#endif

#endif
