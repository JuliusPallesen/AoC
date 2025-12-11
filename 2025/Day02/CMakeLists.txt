cmake_minimum_required(VERSION 3.10)

project(AdventOfCode)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_CXX_EXTENSIONS OFF)

# Compiler warnings
if(MSVC)
    add_compile_options(/W4 /WX)
else()
    add_compile_options(
        -Wall
        -Wextra
        -Wpedantic
        -Werror
        -Wconversion
        -Wsign-conversion
        -Wshadow
    )
endif()

# Optimization flags
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG -march=native")
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g")

# Find source files (avoid GLOB, but keeping your pattern)
file(GLOB SRC_FILES src/*.cpp)
get_filename_component(PROJECT_NAME ${SRC_FILES} NAME_WE)

# Add executable
add_executable(${PROJECT_NAME} ${SRC_FILES})

# Add include directory (fixed path)
target_include_directories(${PROJECT_NAME} PRIVATE 
    ${CMAKE_SOURCE_DIR}/include
)