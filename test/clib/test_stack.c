/*
 * Copyright (C) 2021 Ligang Wang <ligangwangs@gmail.com>
 *
 * unit test for clib stack functions
 */
#include "test.h"

#include "clib/stack.h"
#include "clib/string.h"

TEST(test_stack, push_and_pop)
{
    struct stack s;
    stack_init(&s, sizeof(int));
    int i = 10;
    int j = 20;
    stack_push(&s, &i);
    stack_push(&s, &j);

    ASSERT_EQ(2, stack_size(&s));
    int second = *((int*)stack_pop(&s));
    int first = *((int*)stack_pop(&s));
    ASSERT_EQ(10, first);
    ASSERT_EQ(20, second);
    ASSERT_EQ(0, stack_size(&s));
    stack_deinit(&s);
}

int test_stack(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_stack_push_and_pop);
    test_stats.total_failures += Unity.TestFailures;
    test_stats.total_tests += Unity.NumberOfTests;
    return UNITY_END();
}
