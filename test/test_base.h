/*
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * Unit tests for parser
 */
#include "tutil.h"
#include "gtest/gtest.h"
#include <stdio.h>

class TestBase : public testing::Test {
 protected:
  // Per-test-suite set-up.
  // Called before the first test in this test suite.
  // Can be omitted if not needed.
  static void SetUpTestSuite() {
      symbols_init();
  }

  // Per-test-suite tear-down.
  // Called after the last test in this test suite.
  // Can be omitted if not needed.
  static void TearDownTestSuite() {
      symbols_deinit();
  }

  // You can define per-test set-up logic as usual.
  virtual void SetUp() {}

  // You can define per-test tear-down logic as usual.
  virtual void TearDown() {}

  // Some expensive resource shared by all tests.
  //static T* shared_resource_;
};