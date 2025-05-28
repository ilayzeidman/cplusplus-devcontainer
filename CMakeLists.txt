cmake_minimum_required(VERSION 3.10)
project(HelloWorldTests)

set(CMAKE_CXX_STANDARD 17)

include_directories(src)

add_executable(runTests
    src/hello.cpp
    tests/test_hello.cpp
)

target_link_libraries(runTests gtest gtest_main pthread)
