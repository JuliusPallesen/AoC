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
        -Wall                           # All warnings
        -Wextra                         # Extra warnings
        -Wpedantic                      # Pedantic warnings
        -Werror                         # Warnings as errors
        -Wshadow                        # Variable shadowing
        -Wnon-virtual-dtor              # Non-virtual destructor
        -Wcast-align                    # Pointer casts on systems where access bounds might not be given   
        -Wconversion                    # Implicit conversions
        -Wsign-conversion               # Implicit conversions
        -Wfloat-equal                   # Floating point variable equals 
        -Wlogical-op                    # Suspicious use of logical ops where bitwise should be used
        -Wredundant-decls               # Superfluous declarations
        -Wduplicated-cond               # if branches have same condition
        -Wduplicated-branches           # branches have same logic
        -Wzero-as-null-pointer-constant # `nullptr` not used
        -Wformat=2                      # Additional format checks
        -fdiagnostics-color=always
        -fconcepts-diagnostics-depth=0
        -ftemplate-backtrace-limit=0    # Stop showing template instantiation chains
        -fno-elide-type                 # Don't abbreviate types (ironically helps readability)
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