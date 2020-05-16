
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

#include "clib/util.h"
#include "codegen.h"
#include "env.h"

#ifdef __cplusplus
extern "C" {
#endif

struct JIT {
    struct code_generator* cg;
    void* instance;
    struct menv* env;
};

struct JIT* jit_new(struct code_generator* cg);
void jit_free(struct JIT* jit);
void add_module(struct JIT* jit, void* module);
typedef double (*target_address_double)();
typedef int (*target_address_int)();
void* find_target_address(struct JIT* jit, const char* symbol);

#ifdef __cplusplus
}
#endif

#endif
