cmake_minimum_required(VERSION 3.10)
project(set)

set(CMAKE_CXX_STANDARD 17)

add_executable(set main.cpp set.h
        gtest/gtest_main.cc
        gtest/gtest.h
        gtest/gtest-all.cc)

target_link_libraries(set -lpthread)