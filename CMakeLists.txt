cmake_minimum_required(VERSION 3.12)
project(huffman_v2)

set(CMAKE_CXX_STANDARD 14)

add_library(huffman
        huffman.cpp
        huffman.h
        )

add_executable(huffman_v2
        main.cpp
        )

add_executable(huffman_testing
        gtest/gtest-all.cc
        gtest/gtest.h
        gtest/gtest_main.cc
        testing.cpp
        )



set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++11 -pedantic")

target_link_libraries(huffman_v2 huffman)
target_link_libraries(huffman_testing huffman)

#target_link_libraries(testing)