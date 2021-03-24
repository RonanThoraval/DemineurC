cmake_minimum_required(VERSION 2.6)
project(minesweeper C)

set(CMAKE_BUILD_TYPE DEBUG)
set(CMAKE_C_COMPILER gcc)
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_FLAGS "-std=c99 -g -Wall --coverage")
set(CMAKE_C_FLAGS_DEBUG "-g -DNDEBUG --coverage") 
set(CMAKE_C_FLAGS_RELEASE "-03")

add_executable(main main.c)
add_library(game game.c)
target_link_libraries(main game)