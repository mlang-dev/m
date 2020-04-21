/*
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * unit test for clib hashset functions
 */
#include "gtest/gtest.h"

#include "clib/object.h"
#include "clib/hashset.h"
#include "clib/string.h"

TEST(testHashset, TestAddAndGet)
{
    hashset hs;
    hashset_init(&hs, sizeof(object));
    object obj1 = make_int(10);
    object obj2 = make_int(20);
    object obj3 = make_int(30);
    hashset_add(&hs, &obj1);
    hashset_add(&hs, &obj2);
    ASSERT_EQ(2, hashset_size(&hs));
    ASSERT_TRUE(hashset_in(&hs, &obj1));
    ASSERT_TRUE(hashset_in(&hs, &obj2));
    ASSERT_FALSE(hashset_in(&hs, &obj3));
    hashset_deinit(&hs);
}
