# cmake -S . -B build
# cmake --build build
# -S .       # source directory is current directory
# -B build   # build directory is ./build

cmake_minimum_required(VERSION 3.24)
project(MyString LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(SOURCE_FILES
    main.cpp
    String.cpp)
 
add_executable(${PROJECT_NAME} 
    ${SOURCE_FILES})

target_compile_options(${PROJECT_NAME} PRIVATE
    -Wall
    -Wextra
    -Wpedantic
)