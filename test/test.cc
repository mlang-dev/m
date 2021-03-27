#include "sema/analyzer.h"
#include "parser/astdump.h"
#include "parser/parser.h"
#include "repl.h"
#include "tutil.h"
#include "gtest/gtest.h"
#include <stdio.h>

TEST(testGeneral, testBuildJit)
{
    env* env = env_new(true);
    JIT* jit = build_jit(env);
    jit_free(jit);
    env_free(env);
}
