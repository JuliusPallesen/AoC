cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

# Find the source file in the src directory
file(GLOB SRC_FILES src/*)
get_filename_component(PROJECT_NAME ${SRC_FILES} NAME_WE)

# Set the project name
project(${PROJECT_NAME})

# Add the executable
add_executable(${PROJECT_NAME} ${SRC_FILES} include/aocutils.h)

# Add include directory
target_include_directories(${PROJECT_NAME} PRIVATE /include)