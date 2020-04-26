
//===- KaleidoscopeJIT.h - A simple JIT for Kaleidoscope --------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// Contains a simple JIT definition for use in the kaleidoscope tutorials.
//
//===----------------------------------------------------------------------===//
#ifndef __MLANG_JIT_H__
#define __MLANG_JIT_H__

#include "codegen.h"
#include "clib/util.h"

#ifdef __cplusplus
extern "C"{
#endif

typedef struct JIT {    
    code_generator* cg;
    void* instance;
}JIT;

JIT* jit_new(code_generator* cg);
void jit_free(JIT* jit);

void add_module(JIT *jit, void* module);
typedef double (*target_address)();
target_address find_target_address(JIT *jit, const char *symbol);

#ifdef __cplusplus
}
#endif

#endif
