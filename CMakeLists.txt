cmake_minimum_required(VERSION 3.23)
project(ThreadPool)

set(CMAKE_CXX_STANDARD 11)

add_library(threadpool STATIC threadpool.cpp)