# cmake_minimum_required(VERSION <specify CMake version here>)
project(project_a3)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES
    src/cvsUtil.cpp
    src/cvsUtil.hpp
    src/kanon.cpp
    src/record.cpp
    src/record.hpp
    submit/kanon/src/src/cvsUtil.cpp
    submit/kanon/src/src/cvsUtil.hpp
    submit/kanon/src/src/kanon.cpp
    submit/kanon/src/src/record.cpp
    submit/kanon/src/src/record.hpp)

add_executable(project_a3 ${SOURCE_FILES})