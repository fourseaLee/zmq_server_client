cmake_minimum_required(VERSION 2.8)

project(push_pull)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(CMAKE_BUILD_TYPE "Debug")

add_subdirectory(ventilator)
add_subdirectory(worker)
add_subdirectory(sink)

