#include "compiler/repl.h"
#include "compiler/engine.h"
#include "codegen/llvm/cg_llvm.h"
#include "gtest/gtest.h"
#include "app/app.h"

class TestEnvironment : public testing::Environment {
 public:
    const char *sys_path = 0;
    TestEnvironment(const char *sys_path) : sys_path(sys_path) {}
};

TestEnvironment *get_test_env();