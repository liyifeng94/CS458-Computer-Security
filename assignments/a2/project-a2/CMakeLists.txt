cmake_minimum_required(VERSION 3.2)
project(project_a2)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES
    src/main.cpp)

add_executable(project_a2 ${SOURCE_FILES} src/issExp.h)