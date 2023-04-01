#include "compiler/repl.h"
#include "compiler/engine.h"
#include "codegen/llvm/cg_llvm.h"
#include "gtest/gtest.h"

class Environment : public testing::Environment {
    struct engine *m_engine = 0;
    JIT *m_jit = 0;

 public:
  virtual void SetUp() {
    m_engine = engine_llvm_new(true);
    m_jit = build_jit(m_engine);
    printf("Global SetUp\n");
  }

  virtual void TearDown() {
    printf("Global TearDown\n");
    if (m_jit)
        jit_free(m_jit);
    if (m_engine)
        engine_free(m_engine);
  }

    struct engine *engine() { return m_engine; }

    JIT *jit() { return m_jit; }
};

Environment *get_env();