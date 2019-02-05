cmake_minimum_required(VERSION 2.8)

project(pub_sub)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(CMAKE_BUILD_TYPE "Debug")

add_subdirectory(publibsher)
add_subdirectory(subscriber)
