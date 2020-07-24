cmake_minimum_required(VERSION 3.13)
project(project1 C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -O2 -std=c99 -pedantic")

add_executable(encode encode-skel.c)
add_executable(decode decode-skel.c)
add_executable(crack crack-skel.c)