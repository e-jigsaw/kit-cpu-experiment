cmake_minimum_required(VERSION 2.8)
project(simulator)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu99")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11")

set(SRC io.c main.c)
add_executable(simulator ${SRC})
