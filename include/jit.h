
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
#include "util.h"
#include "KaleidoscopeJIT.h"

typedef struct JIT {    
    code_generator* cg;
    llvm::orc::KaleidoscopeJIT* mjit;
}JIT;

JIT* jit_new(code_generator* cg);
void jit_free(JIT* jit);
uint64_t get_pointer_to_variable(JIT* jit, void* value);
void optimize_function(JIT* jit, void* fun);

#endif