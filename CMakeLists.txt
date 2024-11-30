cmake_minimum_required(VERSION 3.16)

project(silly_reflect C CXX)

add_library(reflect INTERFACE)
target_include_directories(reflect INTERFACE .)

add_executable(test test.cpp)
