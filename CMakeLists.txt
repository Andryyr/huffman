cmake_minimum_required(VERSION 3.12)
project(huffman_v2)

set(CMAKE_CXX_STANDARD 14)



add_executable(huffman_v2

        #for console
#        main.cpp

        huffman.cpp
        huffman.h

        #for tests
        gtest/gtest-all.cc
        gtest/gtest.h
        gtest/gtest_main.cc
        testing.cpp
        )

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++11 -pedantic -O2")

#target_link_libraries(testing)