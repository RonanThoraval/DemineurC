cmake_minimum_required(VERSION 2.6)
project(minesweeper C)

set(CMAKE_BUILD_TYPE DEBUG)
set(CMAKE_C_COMPILER gcc)
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_FLAGS "-std=c99 -g -Wall --coverage")
set(CMAKE_C_FLAGS_DEBUG "-g -DNDEBUG --coverage") 
set(CMAKE_C_FLAGS_RELEASE "-03")

## use a CMake3 package to solve SDL2 dependencies
find_package(SDL2 REQUIRED)
set(SDL2_ALL_INC ${SDL2_INCLUDE_DIRS})
set(SDL2_ALL_LIBS ${SDL2_LIBRARIES} -lSDL2_ttf -lSDL2_image) # add extra SDL2 libraries

add_executable(main main.c)
add_library(game game.c)
target_link_libraries(main game)

include_directories(${SDL2_ALL_INC})
add_executable(sdl sdl.c game.c model.c)
target_link_libraries(sdl ${SDL2_ALL_LIBS} m)

foreach(file "arial.ttf" "drap.png")
  file(COPY ${file} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
endforeach(file)