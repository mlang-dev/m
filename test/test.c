#include "wasm/wasm.h"

#ifdef WASM
#define ENTRY _start
#else
#define ENTRY main
#endif

void test_array();
void test_hashset();
void test_hashtable();
void test_math();
void test_queue();
void test_stack();
void test_string();
void test_symbol();
void test_symboltable();
void test_util();

void setUp(){}
void tearDown(){}


EXPORT int ENTRY()
{
  test_array();
  test_hashset();
  test_hashtable();
  test_math();
  test_queue();
  test_stack();
  test_string();
  test_symbol();
  test_symboltable();
  test_util();
  return 0;
}
