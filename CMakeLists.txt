cmake_minimum_required(VERSION 3.10)
project(HelloWorldTests)

set(CMAKE_CXX_STANDARD 17)

include_directories(src)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GLIB REQUIRED glib-2.0)
pkg_check_modules(GSTREAMER REQUIRED gstreamer-1.0)

include_directories(
    ${GLIB_INCLUDE_DIRS}
    ${GSTREAMER_INCLUDE_DIRS}
)

link_directories(
    ${GLIB_LIBRARY_DIRS}
    ${GSTREAMER_LIBRARY_DIRS}
)

include_directories(/usr/lib/x86_64-linux-gnu/glib-2.0/include/)

# ---- runTests executable ----
add_executable(runTests
    src/hello.cpp
    tests/test_hello.cpp
    src/concepts/enum/enum.hpp
    src/http/server.cpp
    src/gstreamer/gst_pipeline.cpp
    tests/test_concept_enum.cpp
    tests/test_gst_pipeline.cpp
    tests/test_http_server.cpp
    tests/utils/pipeline_descriptions.cpp
)

target_link_libraries(runTests gtest gtest_main pthread ${GSTREAMER_LIBRARIES})

#target_link_libraries(runTests gtest gtest_main pthread)
