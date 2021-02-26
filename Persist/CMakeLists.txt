cmake_minimum_required(VERSION 3.14)
project(Persist)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)
include_directories(common)
include_directories(FPI)

add_executable(Persist main.cpp)