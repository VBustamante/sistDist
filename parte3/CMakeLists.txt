cmake_minimum_required(VERSION 3.12)
project(parte3)

set(CMAKE_CXX_STANDARD 14)

find_package(Threads)

add_executable(parte3 main.cpp Networker.cpp Networker.h)

target_link_libraries (parte3 ${CMAKE_THREAD_LIBS_INIT})