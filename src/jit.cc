/*
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * JIT (just-in-time compiler)
 */
#include "jit.h"
#include "clib/util.h"

JIT* jit_new(code_generator* cg)
{
    auto jit = new JIT();
    jit->cg = cg;
    jit->mjit = new llvm::orc::KaleidoscopeJIT();
    return jit;
}

void jit_free(JIT* jit)
{
    parser_free(jit->cg->parser);
    cg_free(jit->cg);
    delete jit->mjit;
    delete jit;
}
