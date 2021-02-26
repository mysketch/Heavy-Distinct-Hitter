cmake_minimum_required(VERSION 3.14)
project(Super)

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -msse2 -mssse3 -msse4.1 -msse4.2 -mavx -mavx2 -mbmi")

include_directories(.)
include_directories(common)
include_directories(FSS)

add_executable(Super main.cpp)