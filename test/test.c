#include "wasm/wasm.h"
#include <stdio.h>

#ifdef WASM
#define ENTRY _start
#else
#define ENTRY main
#endif

int test_array();
int test_byte_array();
int test_hashset();
int test_hashtable();
int test_math();
int test_queue();
int test_stack();
int test_string();
int test_symbol();
int test_symboltable();
int test_util();

int test_tok();
int test_g_parser();
int test_grammar();
int test_wat_codegen();
int test_mwat();

void setUp(){}
void tearDown(){}


EXPORT int ENTRY()
{
  int failures = 0;
  failures += test_array();
  failures += test_byte_array();
  failures += test_hashset();
  failures += test_hashtable();
  failures += test_math();
  failures += test_queue();
  failures += test_stack();
  failures += test_string();
  failures += test_symbol();
  failures += test_symboltable();
  failures += test_util();

  failures += test_tok();
  failures += test_g_parser();
  failures += test_grammar();
  failures += test_wat_codegen();
  failures += test_mwat();
  if (!failures)
    printf("Unit tests passed !\n");
  else
    printf("Unit tests failures: %d\n", failures);
  return failures;
}
