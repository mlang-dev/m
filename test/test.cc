#include "gtest/gtest.h"
#include "parser.h"
#include "repl.h"
#include "tutil.h"
#include "analyzer.h"
#include <stdio.h>
#include "astdump.h"
TEST(testArray, testRef)
{
  array arr;
  array_init(&arr, sizeof(object));
  char exp[] = "hello";
  object ref = make_ref(exp);
  array_push(&arr, &ref);
  ASSERT_EQ(1, array_size(&arr));
  ASSERT_STREQ("hello", (char*)array_get(&arr, 0)->p_data);
  array_deinit(&arr);
}
