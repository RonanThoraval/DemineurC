#include "game.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>

typedef struct {
    int nb_bombs_around; //number of bombs around the square, -1 if it's a bomb
    bool flag; //True if there is a flag on the square, false else
    bool is_shown; // True if nb_bombs_around has been revealed, false else
} square; // a case (i,j)

struct game_s {
    uint nb_rows;
    uint nb_cols;
    uint nb_bombs; //total number of bombs in the game
    square ** grid; // Game grid, composed of squares
};

/* Returns the number of bombs around the square of the cas (i,j) */
int get_number_bombs_around(game g, uint i, uint j) {
    assert(g!=NULL);
    return g->grid[i][j].nb_bombs_around;
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

/* Sets the number of bombs around the cas (i,j) to nb */
void set_number_bombs_around(game g, uint i, uint j, uint nb) {
    g->grid[i][j].nb_bombs_around = nb;
}

/********** Retuns the number of bombs around the case (i,j) *********/
uint number_bombs_around(game g, uint i, uint j) {
    uint cpt=0 ;
    if (i!=0) {
        if (j!=0 && get_number_bombs_around(g,i-1,j-1)==-1) {
            cpt++;
        }
        if (j!=get_nb_cols(g)-1 && get_number_bombs_around(g,i-1,j+1)==-1) {
            cpt++;
        }
        if (get_number_bombs_around(g,i-1,j)==-1) {
            cpt++;
        }
    } 
    if (i!=get_nb_rows(g)-1) {
        if (get_number_bombs_around(g,i+1,j)==-1) {
            cpt++;
        }
        if (j!=0 &&get_number_bombs_around(g,i+1,j-1)==-1) {
            cpt++;
        }
        if (j!=get_nb_cols(g)-1 && get_number_bombs_around(g,i+1,j+1)==-1) {
            cpt++;
        }
    }
    if (j!=0 && get_number_bombs_around(g,i,j-1)==-1) {
        cpt++;
    } 
    if (j!=get_nb_cols(g)-1 && get_number_bombs_around(g,i,j+1)==-1 ) {
        cpt ++;
    }
    return cpt;
}

/******** Fills the grid with bombs and fills numbers in the others cases ***********/
void init_grid(game g, uint x, uint y) {
    for (uint i=0 ; i < get_nb_rows(g) ; i++) {
        for (uint j=0 ; j < get_nb_cols(g) ; j++) {
            g->grid[i][j].flag=false; // no flags yet
            set_number_bombs_around(g,i,j,10); // impossible number -> to delete
            g->grid[i][j].is_shown=false;
        }
    }
    set_number_bombs_around(g,x,y,0); // no bomb where the user has clicked
    set_number_bombs_around(g,x,y+1,11);
    set_number_bombs_around(g,x,y-1,11);
    set_number_bombs_around(g,x+1,y,11);
    set_number_bombs_around(g,x+1,y+1,11);
    set_number_bombs_around(g,x+1,y-1,11);
    set_number_bombs_around(g,x-1,y,11);
    set_number_bombs_around(g,x-1,y+1,11);
    set_number_bombs_around(g,x-1,y-1,11);

    /* Pose random bombs */
    uint tmp_x, tmp_y;
    for (uint n=0 ; n<get_nb_bombs(g) ; n++) {
        do {
            tmp_x = rand() ;
            printf("rand_x=%u\n", tmp_x);
            tmp_x = tmp_x % get_nb_rows(g);
            tmp_y = rand() ;
            printf("rand_y=%u\n", tmp_y);
            tmp_y = tmp_y % get_nb_cols(g);
        } while (get_number_bombs_around(g,tmp_x,tmp_y)<1 || get_number_bombs_around(g,tmp_x,tmp_y)==11);
        printf("n=%u, x=%u, y=%u\n",n,tmp_x,tmp_y);
        set_number_bombs_around(g,tmp_x,tmp_y,-1);
    }

    /* Filling grid in function of bombs */
    for (uint i=0 ; i < get_nb_rows(g) ; i++) {
        for (uint j=0 ; j < get_nb_cols(g) ; j++) {
            if (get_number_bombs_around(g,i,j)==10 || get_number_bombs_around(g,i,j)==11) {
                uint nb = number_bombs_around(g,i,j);
                set_number_bombs_around(g,i,j,nb);
            }
        }
    }

}


/*** Allocation of the game structure ****/
game game_init(uint nb_rows, uint nb_cols, uint nb_bombs, uint x , uint y) {
    game g = malloc(sizeof(struct game_s));
    assert(g!=NULL);

    g->grid=calloc(nb_rows,sizeof(square *));
    if (g->grid==NULL) {
        free(g);
        fprintf(stderr,"Erreur d'allocation.\n");
        exit(EXIT_FAILURE);
    }
    for (uint i=0 ; i<nb_rows ; i++) {
        g->grid[i]=calloc(nb_cols,sizeof(square));
        if (g->grid[i]==NULL) {
            for (uint j=0 ; j<i ; j++) {
                free(g->grid[j]);
            }
            free(g->grid);
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
    for (uint i=0 ; i<get_nb_rows(g) ; i++) {
        if (g->grid[i]!=NULL) {
            free(g->grid[i]);
        }
    }
    if (g->grid!=NULL) {
        free(g->grid);
    }
    free(g);
}

/* Return true if there is a flag in the case (i,j), False else */
bool is_flagged(game g, uint i, uint j) {
    return g->grid[i][j].flag;
}

/*  Puts a flag in case (i,j) */
void pose_flag(game g, uint i, uint j) {
    g->grid[i][j].flag=true;
}


/* Removes the flag in case (i,j) */
void remove_flag(game g, uint i, uint j) {
    g->grid[i][j].flag=false;
}


/* Returns true if the number in the case (i,j) has been revealed, False else */
bool is_shown(game g, uint i, uint j) {
    assert(g!=NULL);
    return g->grid[i][j].is_shown;
}


/* Reveals the number in case (i,j) */
void show(game g , uint i, uint j) {
    g->grid[i][j].is_shown=true;
}


/* (Recursive) Reveals case and the others around if the case is a zero */
void reveal_case(game g, uint i, uint j) {
    show(g,i,j); //reveals current case
    if (get_number_bombs_around(g,i,j)==0) {
        if (i!=0) {
            if (j!=0 && !is_shown(g,i-1,j-1)) {
                reveal_case(g,i-1,j-1);
            }
            if (j!=get_nb_cols(g)-1 && !is_shown(g,i-1,j+1)) {
                reveal_case(g,i-1,j+1);
            }
            if ( !is_shown(g,i-1,j)) {
                reveal_case(g,i-1,j);
            }
        }
        if (i!=get_nb_rows(g)-1) {
            if (j!=0 && !is_shown(g,i+1,j-1)) {
                reveal_case(g,i+1,j-1);
            }
            if (j!=get_nb_cols(g)-1 && !is_shown(g,i+1,j+1)) {
                reveal_case(g,i+1,j+1);
            }
            if ( !is_shown(g,i+1,j)) {
                reveal_case(g,i+1,j);
            }
        }
        if (j!=0 && !is_shown(g,i,j-1))  {
            reveal_case(g,i,j-1);
        }
        if (j!=get_nb_cols(g)-1 && !is_shown(g,i,j+1)) {
            reveal_case(g,i,j+1);
        }
    }
}