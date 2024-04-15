#include "codegen/llvm/cg_llvm.h"
#include "codegen/mlir/cg_mlir.h"
#include "app/app.h"
#include "test_fixture.h"
#include "test_env.h"

void TestFixture::SetUp() 
{
  app_init();
  engine = CreateEngine();
  jit = jit_new(engine);
}

void TestFixture::TearDown() 
{
  if (jit)
      jit_free(jit);
  if (engine)
      engine_free(engine);
  app_deinit();
}

struct engine* TestFixture::CreateEngine() 
{
  return engine_llvm_new(get_test_env()->sys_path, false);
}

struct engine* TestFixture2::CreateEngine() 
{
  return engine_mlir_new(get_test_env()->sys_path, false);
}
