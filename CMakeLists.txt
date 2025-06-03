cmake_minimum_required(VERSION 3.10)
project(HelloWorldTests)

set(CMAKE_CXX_STANDARD 17)

include_directories(src)

add_executable(runTests
    src/hello.cpp
    tests/test_hello.cpp
    src/concepts/enum/enum.hpp
    tests/test_concept_enum.cpp
)

target_link_libraries(runTests gtest gtest_main pthread)
