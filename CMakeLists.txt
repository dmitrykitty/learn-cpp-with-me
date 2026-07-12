# ============================================================================
# Example CMake project
# ============================================================================

# Configure the project:
#   cmake -S . -B build
#
# Build the project:
#   cmake --build build
#
# Run the executable on Linux:
#   ./build/MyString
#
# Run the executable on Windows:
#   .\build\Debug\MyString.exe
#
# -S .       means that the source directory is the current directory.
# -B build   means that generated build files should be placed in ./build.
#
# CMake does not compile the project during the configure step.
# It first reads CMakeLists.txt and generates a build system,
# for example Ninja files, Unix Makefiles, or Visual Studio project files.


# Older alternative:
#
#   mkdir build
#   cd build
#   cmake ..
#   cmake --build .
#
# Running "make" directly works only when CMake generated Unix Makefiles.
# "cmake --build" is more portable because it works with Make, Ninja,
# Visual Studio, and other CMake generators.

# Run all registered tests:
# ctest --test-dir build 

# Run tests and display their output: 
# ctest --test-dir build --output-on-failure 

# Build only the test executable: 
# cmake --build build --target MyStringTests 

# Run the test executable directly: 
# ./build/MyStringTests


# Require at least CMake 3.24.
cmake_minimum_required(VERSION 3.24)


# Define the project name and declare that the project uses C++.
#
# PROJECT_NAME will contain "MyString".
project(MyString LANGUAGES CXX)


# ============================================================================
# Global C++ configuration
# ============================================================================

# Request the C++20 standard.
set(CMAKE_CXX_STANDARD 20)

# Fail during configuration if the compiler does not support C++20.
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Disable compiler-specific language extensions.
#
# GCC will use -std=c++20 instead of -std=gnu++20.
set(CMAKE_CXX_EXTENSIONS OFF)


# ============================================================================
# MyStringLibrary target
# ============================================================================

# Create a library target.
#
# STATIC means that CMake creates a static library:
#   libMyStringLibrary.a   on Linux
#   MyStringLibrary.lib    on Windows
#
# The library contains the implementation of the String class.
add_library(MyStringLibrary STATIC)


# Add source files to the library target.
#
# PRIVATE sources are compiled as part of this target but are not propagated
# to targets that link against MyStringLibrary.
#
# String.cpp is an implementation detail, so it should be PRIVATE.
target_sources(MyStringLibrary
    PRIVATE
        source/String.cpp

    # FILE_SET HEADERS describes the public headers provided by the library.
    #
    # PUBLIC means that the headers belong to the public interface of the
    # library and are visible to targets that consume MyStringLibrary.
    PUBLIC
        FILE_SET HEADERS
        BASE_DIRS
            include
        FILES
            include/MyString.h
)


# Add the include directory to the library.
#
# PUBLIC means:
#   1. MyStringLibrary itself can include headers from this directory.
#   2. Targets linked with MyStringLibrary inherit this include directory.
#
# Because of this, main.cpp can use:
#
#   #include "MyString.h"
#
# without writing:
#
#   #include "include/MyString.h"
target_include_directories(MyStringLibrary
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)


# Add warnings while compiling the library.
#
# PRIVATE means these exact compile options are used only when compiling
# MyStringLibrary. They are not automatically propagated to consumers.
target_compile_options(MyStringLibrary
    PRIVATE
        -Wall
        -Wextra
        -Wpedantic
)


# ============================================================================
# MyString executable target
# ============================================================================

# Create an executable target named after the project.
#
# PROJECT_NAME expands to MyString.
add_executable(${PROJECT_NAME})


# Add source files belonging directly to the executable.
#
# main.cpp belongs to the application, while String.cpp belongs to the library.
target_sources(${PROJECT_NAME}
    PRIVATE
        main.cpp
)


# Link the executable with MyStringLibrary.
#
# This tells CMake that:
#   - MyString depends on MyStringLibrary,
#   - the library must be built before the executable,
#   - the compiled library must be passed to the linker,
#   - PUBLIC usage requirements of MyStringLibrary are inherited by MyString.
#
# Because MyStringLibrary exposes its include directory as PUBLIC,
# MyString automatically knows where MyString.h is located.
#
# PRIVATE means that MyString uses MyStringLibrary, but does not expose
# MyStringLibrary as part of its own public interface.
target_link_libraries(${PROJECT_NAME}
    PRIVATE
        MyStringLibrary
)


# Add warnings while compiling main.cpp.
#
# Compile options added to MyStringLibrary do not automatically apply here,
# because they were declared PRIVATE for the library.
target_compile_options(${PROJECT_NAME}
    PRIVATE
        -Wall
        -Wextra
        -Wpedantic
)

# ============================================================================ 
# Testing 
# ============================================================================

# Enable CTest support for this project. 
# Without enable_testing(), tests added with add_test() are not registered. 
enable_testing()