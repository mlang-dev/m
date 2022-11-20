/*
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * unit test for clib queue functions
 */
#include "test.h"

#include "clib/queue.h"
#include "clib/string.h"

TEST(test_queue, push_and_pop)
{
    struct queue q;
    queue_init(&q, sizeof(int));
    int i = 10;
    int j = 20;
    queue_push(&q, &i);
    queue_push(&q, &j);

    ASSERT_EQ(2, queue_size(&q));
    int first = *((int*)queue_front(&q));
    queue_pop(&q);
    int second = *((int*)queue_front(&q));
    queue_pop(&q);
    ASSERT_EQ(10, first);
    ASSERT_EQ(20, second);
    ASSERT_EQ(0, queue_back(&q));
    queue_deinit(&q);
}

TEST(test_queue, pop_with_value)
{
    struct queue q;
    queue_init(&q, sizeof(int));
    int i = 10;
    int j = 20;
    queue_push(&q, &i);
    queue_push(&q, &j);

    ASSERT_EQ(2, queue_size(&q));
    int first = *((int*)queue_pop(&q));
    int second = *((int*)queue_pop(&q));
    ASSERT_EQ(10, first);
    ASSERT_EQ(20, second);
    ASSERT_EQ(0, queue_front(&q));
    ASSERT_EQ(0, queue_pop(&q));
    queue_deinit(&q);
}

TEST(test_queue, circle)
{
    struct queue q;
    queue_init(&q, sizeof(int));
    ASSERT_EQ(7, q.items.cap);
    for (int i = 0; i < 10; i++) {
        queue_push(&q, &i);
    }
    ASSERT_EQ(7, q.items.cap);
    queue_deinit(&q);
}

int test_queue()
{
    UNITY_BEGIN();
    RUN_TEST(test_queue_push_and_pop);
    RUN_TEST(test_queue_pop_with_value);
    RUN_TEST(test_queue_circle);
    return UNITY_END();
}
