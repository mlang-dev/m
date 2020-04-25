/*
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * unit test for clib queue functions
 */
#include "gtest/gtest.h"

#include "clib/queue.h"
#include "clib/string.h"

TEST(testQueue, TestQueuePushAndPop)
{
    queue q;
    queue_init(&q, sizeof(int));
    int i=10; int j=20;
    queue_push(&q, &i);
    queue_push(&q, &j);

    ASSERT_EQ(2, queue_size(&q));
    int first = *((int*)queue_front(&q));
    queue_pop(&q);
    int second = *((int*)queue_front(&q));
    queue_pop(&q);
    ASSERT_EQ(10, first);
    ASSERT_EQ(20, second);
    ASSERT_EQ(NULL, queue_back(&q));
    queue_deinit(&q);
}


TEST(testQueue, TestQueuePopWithValue)
{
    queue q;
    queue_init(&q, sizeof(int));
    int i=10; int j=20;
    queue_push(&q, &i);
    queue_push(&q, &j);

    ASSERT_EQ(2, queue_size(&q));
    int first = *((int*)queue_pop(&q));
    int second = *((int*)queue_pop(&q));
    ASSERT_EQ(10, first);
    ASSERT_EQ(20, second);
    ASSERT_EQ(NULL, queue_front(&q));
    ASSERT_EQ(NULL, queue_pop(&q));
    queue_deinit(&q);

}

TEST(testQueue, TestQueueCircle)
{
    queue q;
    queue_init(&q, sizeof(int));
    ASSERT_EQ(7, q.items.cap);
    for(int i=0;i<10;i++){
        queue_push(&q, &i);
    }
    ASSERT_EQ(7, q.items.cap);
}