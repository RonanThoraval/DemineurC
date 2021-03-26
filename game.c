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
    int nb_rows;
    int nb_cols;
    int nb_bombs; //total number of bombs in the game
    square ** grid; // Game grid, composed of squares
};

void game_print(game g ) {
    assert(g!=NULL);
    for (int i =0 ; i<get_nb_rows(g) ; i++) {
        for (int j=0 ; j<get_nb_cols(g) ; j++) {
            printf("%i ",get_number_bombs_around(g,i,j));
        }
        printf("\n");
    }
    printf("\n");
    for (int i =0 ; i<get_nb_rows(g) ; i++) {
        for (int j=0 ; j<get_nb_cols(g) ; j++) {
            printf("%i ",is_shown(g,i,j));
        }
        printf("\n");
    }
}


/* Returns the number of bombs around the square of the cas (i,j) */
int get_number_bombs_around(game g, int i, int j) {
    assert(g!=NULL);
    return g->grid[i][j].nb_bombs_around;
}

int get_nb_rows(game g) {
    assert(g!=NULL);
    return g->nb_rows;
}

int get_nb_cols(game g) {
    assert(g!=NULL);
    return g->nb_cols;
}

int get_nb_bombs(game g) {
    assert(g!=NULL);
    return g->nb_bombs;
}

/* Sets the number of bombs around the cas (i,j) to nb */
void set_number_bombs_around(game g, int i, int j, int nb) {
    g->grid[i][j].nb_bombs_around = nb;
}


/* Returns true if the case (i,j) is in the grid, False else */
bool is_inside_grid(game g, int i, int j) {
    return (i>=0 && j>=0 && i<get_nb_rows(g) && j<get_nb_cols(g));
}

/********** Retuns the number of bombs around the case (i,j) *********/
int number_bombs_around(game g, int i, int j) {
    int cpt=0 ;
    for (int k=i-1 ; k<=i+1 ; k++) {
        for (int l=j-1 ; l<=j+1 ; l++) {
            if (is_inside_grid(g,k,l) && g->grid[k][l].nb_bombs_around==-1 && (k!=i || l!=j)) { //if the case(k,l)!=(i,j) and there is a bomb in the case (k,l)
                cpt++;
            }
        }
    }
    return cpt;
}

/******** Fills the grid with bombs and fills numbers in the others cases ***********/
void init_grid(game g, int x, int y) {
    for (int i=0 ; i < get_nb_rows(g) ; i++) {
        for (int j=0 ; j < get_nb_cols(g) ; j++) {
            g->grid[i][j].flag=false; // no flags yet
            set_number_bombs_around(g,i,j,10); // impossible number -> to delete
            g->grid[i][j].is_shown=false;
        }
    }
    set_number_bombs_around(g,x,y,0); // no bomb where the user has clicked
    // No bomb around where the user has clicked

    for (int k=x-1 ; k<=x+1 ; k++) {
        for (int l=y-1 ; l<=y+1 ; l++) {
            if (is_inside_grid(g,k,l) && (k!=x || l!=y)) {
                set_number_bombs_around(g,k,l,11);
            }
        }
    }

    /* Pose random bombs */
    int tmp_x, tmp_y;
    for (int n=0 ; n<get_nb_bombs(g) ; n++) {
        do {
            tmp_x = rand() ;
            //printf("rand_x=%d\n", tmp_x);
            tmp_x = tmp_x % get_nb_rows(g);
            tmp_y = rand() ;
            //printf("rand_y=%d\n", tmp_y);
            tmp_y = tmp_y % get_nb_cols(g);
        } while (get_number_bombs_around(g,tmp_x,tmp_y)<1 || get_number_bombs_around(g,tmp_x,tmp_y)==11);
        //printf("n=%d, x=%d, y=%d\n",n,tmp_x,tmp_y);
        set_number_bombs_around(g,tmp_x,tmp_y,-1);
    }
    game_print(g);

    /* Filling grid in function of bombs */
    for (int i=0 ; i < get_nb_rows(g) ; i++) {
        for (int j=0 ; j < get_nb_cols(g) ; j++) {
            if (get_number_bombs_around(g,i,j)==10 || get_number_bombs_around(g,i,j)==11) {
                int nb = number_bombs_around(g,i,j);
                set_number_bombs_around(g,i,j,nb);
            }
        }
    }

}


/*** Allocation of the game structure ****/
game game_init(int nb_rows, int nb_cols, int nb_bombs, int x , int y) {
    game g = malloc(sizeof(struct game_s));
    assert(g!=NULL);

    g->grid=calloc(nb_rows,sizeof(square *));
    if (g->grid==NULL) {
        free(g);
        fprintf(stderr,"Erreur d'allocation.\n");
        exit(EXIT_FAILURE);
    }
    for (int i=0 ; i<nb_rows ; i++) {
        g->grid[i]=calloc(nb_cols,sizeof(square));
        if (g->grid[i]==NULL) {
            for (int j=0 ; j<i ; j++) {
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
    for (int i=0 ; i<get_nb_rows(g) ; i++) {
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
bool is_flagged(game g, int i, int j) {
    return g->grid[i][j].flag;
}

/*  Puts a flag in case (i,j) */
void pose_flag(game g, int i, int j) {
    g->grid[i][j].flag=true;
}


/* Removes the flag in case (i,j) */
void remove_flag(game g, int i, int j) {
    g->grid[i][j].flag=false;
}


/* Returns true if the number in the case (i,j) has been revealed, False else */
bool is_shown(game g, int i, int j) {
    assert(g!=NULL);
    return g->grid[i][j].is_shown;
}


/* Reveals the number in case (i,j) */
void show(game g , int i, int j) {
    assert(g!=NULL);
    assert(g->grid!=NULL);
    g->grid[i][j].is_shown=true;
}


/* (Recursive) Reveals case and the others around if the case is a zero */
void reveal_case(game g, int i, int j) {
    show(g,i,j); //reveals current case
    for (int k=i-1 ; k<=i+1 ; k++) {
        for (int l=j-1 ; l<=j+1 ; l++) {
            if (is_inside_grid(g,k,l) && (k!=i || l!=j) && !is_shown(g,k,l) && get_number_bombs_around(g,i,j)==0) {
                reveal_case(g,k,l);
            }
        }
    }
}