/*
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * unit test for clib array functions
 */
#include "gtest/gtest.h"

#include "clib/queue.h"
#include "clib/string.h"

TEST(testQueue, TestQueuePushAndPop)
{
    queue q;
    int i=10; int j=20;
    queue_push(&q, &i);
    queue_push(&q, &j);
    int first = *((int*)queue_back(&q));
    queue_pop(&q);
    int second = *((int*)queue_back(&q));
    queue_pop(&q);
    ASSERT_EQ(10, first);
    ASSERT_EQ(20, second);
    ASSERT_EQ(NULL, queue_back(&q));

}