cmake_minimum_required(VERSION 3.16)

project(mtest, VERSION 0.0.9 LANGUAGES CXX)

# set any CMAKE_ variables here
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -stdlib=libc++")
set(CMAKE_CXX_STANDARD 17)

add_executable(mtest
  test_parser.cc
)
TARGET_LINK_LIBRARIES(mtest gtest gmock gtest_main)

enable_testing()
include(GoogleTest)
gtest_discover_tests(mtest)


