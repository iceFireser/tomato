cmake_minimum_required(VERSION 3.10)
project(tomato)

set(CMAKE_CXX_STANDARD 11)

add_subdirectory(./src/pm)
add_subdirectory(./src/trans)
add_subdirectory(./src/store)
