cmake_minimum_required(VERSION 2.8)

project(sink)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(CMAKE_BUILD_TYPE "Debug")

include_directories(./include)

aux_source_directory(./src DIR_SRCS)

set(link_lib pthread zmq)

add_executable(sink ${DIR_SRCS})

target_link_libraries(sink ${link_lib})
