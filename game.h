#ifndef __GAME_H__
#define __GAME_H__


typedef struct game_s *game;
typedef unsigned int uint ;

game game_init(uint nb_rows, uint nb_cols, uint nb_bombs, uint x , uint y);

void game_delete(game g);

int get_square(game g, uint i, uint j);

uint get_nb_rows(game g);

uint get_nb_cols(game g);

#endif