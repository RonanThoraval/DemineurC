#include "game.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>


void game_print(game g ) {
    assert(g!=NULL);
    for (uint i =0 ; i<get_nb_rows(g) ; i++) {
        for (uint j=0 ; j<get_nb_cols(g) ; j++) {
            printf("%i ",get_number_bombs_around(g,i,j));
        }
        printf("\n");
    }
    printf("\n");
    for (uint i =0 ; i<get_nb_rows(g) ; i++) {
        for (uint j=0 ; j<get_nb_cols(g) ; j++) {
            printf("%i ",is_shown(g,i,j));
        }
        printf("\n");
    }
}


int main(int argc, char* argv[]) {
    game g=game_init(5,5,3,1,3);
    reveal_case(g,1,3);
    game_print(g);
    game_delete(g);
    return EXIT_SUCCESS;
}