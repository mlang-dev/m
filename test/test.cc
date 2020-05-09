#include "analyzer.h"
#include "astdump.h"
#include "parser.h"
#include "repl.h"
#include "tutil.h"
#include "gtest/gtest.h"
#include <stdio.h>

TEST(testGeneral, testBuildJit)
{
    menv* env = env_new();
    parser* parser = parser_new(0, true, 0);
    JIT* jit = build_jit(env, parser);
    jit_free(jit);
    env_free(env);
}