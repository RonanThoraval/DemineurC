#include "game.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>

struct game_s {
    uint nb_rows;
    uint nb_cols;
    uint nb_bombs;
    int ** grid; // -1 if bomb , i if the case is surrounded by i bombs
    bool ** flags;
};

int get_square(game g, uint i, uint j) {
    assert(g!=NULL);
    return g->grid[i][j];
}

uint get_nb_rows(game g) {
    assert(g!=NULL);
    return g->nb_rows;
}

uint get_nb_cols(game g) {
    assert(g!=NULL);
    return g->nb_cols;
}

uint get_nb_bombs(game g) {
    assert(g!=NULL);
    return g->nb_bombs;
}

/********** TO_DO *********/
uint get_number_bombs_around(game g, uint i, uint j) {
    uint cpt=0 ;
    if (i!=0) {
        if (j!=0 && g->grid[i-1][j-1]==-1) {
            cpt++;
        }
        if (j!=get_nb_cols(g)-1 && g->grid[i-1][j+1]==-1) {
            cpt++;
        }
        if (g->grid[i-1][j]==-1) {
            cpt++;
        }
    } 
    if (i!=get_nb_rows(g)-1) {
        if (g->grid[i+1][j]==-1) {
            cpt++;
        }
        if (j!=0 && g->grid[i+1][j-1]==-1) {
            cpt++;
        }
        if (j!=get_nb_cols(g)-1 && g->grid[i+1][j+1]==-1) {
            cpt++;
        }
    }
    if (j!=0 && g->grid[i][j-1]==-1) {
        cpt++;
    } 
    if (j!=get_nb_cols(g)-1 && g->grid[i][j+1]==-1 ) {
        cpt ++;
    }
    return cpt;
}

/******** Fills the grid with bombs and fills numbers in the others cases ***********/
void init_grid(game g, uint x, uint y) {
    for (uint i=0 ; i < get_nb_rows(g) ; i++) {
        for (uint j=0 ; j < get_nb_cols(g) ; j++) {
            g->flags[i][j]=false; // no flags yet
            g->grid[i][j]=10; // impossible number -> to delete
        }
    }
    g->grid[x][y] = 0 ; // no bomb where the user has clicked

    /* Pose random bombs */
    uint tmp_x, tmp_y;
    for (uint n=0 ; n<get_nb_bombs(g) ; n++) {
        do {
            tmp_x = rand() % get_nb_rows(g);
            tmp_y = rand() % get_nb_rows(g);
        } while (g->grid[tmp_x][tmp_y]<1);
        g->grid[tmp_x][tmp_y]=-1;
    }

    /* Fill grid in function of bombs */
    for (uint i=0 ; i < get_nb_rows(g) ; i++) {
        for (uint j=0 ; j < get_nb_cols(g) ; j++) {
            if (g->grid[i][j]==10) {
                uint nb = get_number_bombs_around(g,i,j);
                g->grid[i][j]= nb ;
            }
        }
    }

}


/*** Allocation of the game structure ****/
game game_init(uint nb_rows, uint nb_cols, uint nb_bombs, uint x , uint y) {
    game g = malloc(sizeof(struct game_s));
    assert(g!=NULL);

    g->grid=calloc(nb_rows,sizeof(int *));
    g->flags=calloc(nb_rows,sizeof(bool *));
    if (g->flags==NULL || g->grid==NULL) {
        free(g);
        fprintf(stderr,"Erreur d'allocation.\n");
        exit(EXIT_FAILURE);
    }
    for (uint i=0 ; i<nb_rows ; i++) {
        g->grid[i]=calloc(nb_cols,sizeof(int));
        g->flags[i]=calloc(nb_cols,sizeof(bool));
        if (g->grid[i]==NULL || g->flags[i]==NULL) {
            for (uint j=0 ; j<i ; j++) {
                free(g->grid[j]);
                free(g->flags[j]);
            }
            free(g->grid);
            free(g->flags);
            free(g);
            fprintf(stderr,"Erreur d'allocation.\n");
            exit(EXIT_FAILURE);
        }
    }

    g->nb_rows=nb_rows;
    g->nb_cols=nb_cols;
    g->nb_bombs=nb_bombs;

    init_grid(g,x,y); // initialize the grid in function of the game parameters

    return g;
}

void game_delete(game g) {
    assert(g!=NULL);
    assert(g->grid!=NULL);
    assert(g->flags!=NULL);
    for (uint i=0 ; i<get_nb_rows(g) ; i++) {
        if (g->flags[i]!=NULL) {
            free(g->flags[i]);
        }
        if (g->grid[i]!=NULL) {
            free(g->grid[i]);
        }
    }
    if (g->grid!=NULL) {
        free(g->grid);
    }
    if (g->flags!=NULL) {
        free(g->flags);
    }

    free(g);
}





