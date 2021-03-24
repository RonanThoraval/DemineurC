#include "game.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>


void game_print(game g ) {
    assert(g!=NULL);
    for (uint i =0 ; i<get_nb_rows(g) ; i++) {
        for (uint j=0 ; j<get_nb_cols(g) ; j++) {
            printf("%i ",get_square(g,i,j));
        }
        printf("\n");
    }
}


int main(int argc, char* argv[]) {
    game g=game_init(5,5,2,1,1);
    game_print(g);
    game_delete(g);
    return EXIT_SUCCESS;
}