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
    queue_init(&q, sizeof(object));
    object i=make_int(10); object j=make_int(20);
    queue_push(&q, &i);
    queue_push(&q, &j);

    ASSERT_EQ(2, queue_size(&q));
    int first = ((object*)queue_front(&q))->i_data;
    queue_pop(&q);
    int second = ((object*)queue_front(&q))->i_data;
    queue_pop(&q);
    ASSERT_EQ(10, first);
    ASSERT_EQ(20, second);
    ASSERT_EQ(NULL, queue_back(&q));
    queue_deinit(&q);

}


TEST(testQueue, TestQueuePopWithValue)
{
    queue q;
    queue_init(&q, sizeof(object));
    object i=make_int(10); object j=make_int(20);
    queue_push(&q, &i);
    queue_push(&q, &j);

    ASSERT_EQ(2, queue_size(&q));
    int first = ((object*)queue_pop(&q))->i_data;
    int second = ((object*)queue_pop(&q))->i_data;
    ASSERT_EQ(10, first);
    ASSERT_EQ(20, second);
    ASSERT_EQ(NULL, queue_front(&q));
    ASSERT_EQ(NULL, queue_pop(&q));
    queue_deinit(&q);

}
