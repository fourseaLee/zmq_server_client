cmake_minimum_required(VERSION 2.8)

project(worker)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(CMAKE_BUILD_TYPE "Debug")

include_directories(./include)

aux_source_directory(./src DIR_SRCS)

set(link_lib pthread zmq)

add_executable(worker ${DIR_SRCS})

target_link_libraries(worker ${link_lib})
