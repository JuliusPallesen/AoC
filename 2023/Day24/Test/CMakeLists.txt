cmake_minimum_required(VERSION 3.28)

project(d24)

set (CMAKE_CXX_STANDARD 20)

find_package(Eigen3 REQUIRED)

add_executable(${PROJECT_NAME} src/main.cpp)

target_link_libraries(${PROJECT_NAME} Eigen3::Eigen)