#include "compiler/repl.h"
#include "compiler/engine.h"
#include "codegen/llvm/cg_llvm.h"
#include "gtest/gtest.h"
#include "app/app.h"

class TestEnvironment : public testing::Environment {
 public:
    const char *sys_path = 0;
    TestEnvironment(const char *sys_path) : sys_path(sys_path) {}
    virtual ~TestEnvironment(); // Declare destructor as virtual
    virtual void setUp();       // A virtual method to set up the test environment
    virtual void tearDown();    // A virtual method to tear down the test environment    
};

TestEnvironment *get_test_env();