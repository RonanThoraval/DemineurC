#include "game.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>




int main(int argc, char* argv[]) {
    game g=game_init(8,8,10,0,0);
    reveal_case(g,0,0);
    game_print(g);
    game_delete(g);
    return EXIT_SUCCESS;
}