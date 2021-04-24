# minesweeper
Minesweepper but this time, it's in C, using SDL2.

## Installation 

### Pull repository

Clone this repository on your machine :

```
$ git clone https://github.com/erellgachon/minesweeper
```

### Debian/Ubuntu 

You have to download the SDL2 dependencies :

```
$ sudo sudo apt-get install libsdl2-dev libsdl2-image-dev  libsdl2-ttf-dev
```

## Run

To run the game, go to the folder "minesweeper" and build the project in a folder (here named "build"), like that :

```
$ mkdir build
$ cd build
$ cmake ..
$ make
$ ./sdl
```

