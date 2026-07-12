# cmake -S . -B build
# cmake --build build
# -S .       # source directory is current directory
# -B build   # build directory is ./build

cmake_minimum_required(VERSION 3.24)
project(MyString LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(LIB_NAME MyStringLib)

set(EX_TEST_NAME StringUnitTests)

set(SOURCE_FILES 
    source/String.cpp
)

set(HEADER_FILES
    include/String.h
)

set(SOURCE_TEST_FILES tests/tests.cpp)

add_library(${LIB_NAME} STATIC 
    ${SOURCE_FILES} 
    ${HEADER_FILES}
)

target_include_directories(${LIB_NAME} PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_options(${LIB_NAME} 
    PRIVATE
        -Wall
        -Wextra
        -Wpedantic
)

#=================DOWNLOAD GTESTS======================

# FetchContent allows CMake to download and add external projects.
include(FetchContent)

# Declare where GoogleTest should be downloaded from.
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.17.0
)

# Download GoogleTest if necessary and create its CMake targets.
#
# After this command, targets such as these become available:
#   GTest::gtest
#   GTest::gtest_main
#   GTest::gmock
#   GTest::gmock_main
FetchContent_MakeAvailable(googletest)

#=====================TURNING ON TESTS========================
enable_testing()

add_executable(${EX_TEST_NAME} 
    ${SOURCE_TEST_FILES}
)

target_link_libraries(${EX_TEST_NAME} PRIVATE 
    ${LIB_NAME}
    GTest::gtest_main
    )

target_compile_options(${EX_TEST_NAME} 
    PRIVATE
        -Wall
        -Wextra
        -Wpedantic
)

# add_test(
#     NAME ${EX_TEST_NAME}
#     COMMAND ${EX_TEST_NAME}
# )

include(GoogleTest)
# Automatically discover every TEST(...) defined in the executable
gtest_discover_tests(StringUnitTests)