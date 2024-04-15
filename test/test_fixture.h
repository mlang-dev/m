#include "compiler/repl.h"
#include "compiler/engine.h"
#include "gtest/gtest.h"

class TestFixture : public testing::Test {
  protected:
    struct engine *engine = 0;
    JIT *jit = 0;
    const char *sys_path = 0;
    void SetUp() override;
    void TearDown() override;
};


class TestFixture2 : public testing::Test {
  protected:
    struct engine *engine = 0;
    JIT *jit = 0;
    const char *sys_path = 0;
    void SetUp() override;
    void TearDown() override;
};
