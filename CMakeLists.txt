cmake_minimum_required(VERSION 3.1)

project(dodge)

add_executable(dodge dodge.cpp)

target_link_libraries(dodge raylib)
