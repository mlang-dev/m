#include "wasm/wasm.h"

#ifdef WASM
#define ENTRY _start
#else
#define ENTRY main
#endif

void test_array();

EXPORT int ENTRY()
{
  test_array();
  return 0;
}
