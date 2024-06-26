// Copyright 2006, Google Inc.
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//     * Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
//     * Neither the name of Google Inc. nor the names of its
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#include <cstdio>

#include "gtest/gtest.h"
#include "test_env.h"

static TestEnvironment *env = 0;

#if defined(GTEST_OS_ESP8266) || defined(GTEST_OS_ESP32)
// Arduino-like platforms: program entry points are setup/loop instead of main.

#ifdef GTEST_OS_ESP8266
extern "C" {
#endif

void setup() { testing::InitGoogleTest(); }

void loop() { RUN_ALL_TESTS(); }

#ifdef GTEST_OS_ESP8266
}
#endif

#elif defined(GTEST_OS_QURT)
// QuRT: program entry point is main, but argc/argv are unusable.

GTEST_API_ int main() {
  printf("Running main() from %s\n", __FILE__);
  testing::InitGoogleTest();
  return RUN_ALL_TESTS();
}
#else
// Normal platforms: program entry point is main, argc/argv are initialized.

GTEST_API_ int main(int argc, char **argv) {
  if(argc < 2){
    printf("Usage: %s <sys_path>", argv[0]);
    return 1;
  }
  printf("Running main() from %s\n", __FILE__);
  testing::InitGoogleTest(&argc, argv);
  env = new TestEnvironment(argv[1]);
  testing::AddGlobalTestEnvironment(env);
  return RUN_ALL_TESTS();
}

TestEnvironment::~TestEnvironment() {
    // Implementation of virtual destructor
}

void TestEnvironment::setUp() {
    // Implementation of setUp
}

void TestEnvironment::tearDown() {
    // Implementation of tearDown
}

TestEnvironment * get_test_env() {
    return env;
}
#endif