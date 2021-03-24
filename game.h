#ifndef __GAME_H__
#define __GAME_H__
#include <stdbool.h>


typedef struct game_s *game;
typedef unsigned int uint ;

game game_init(uint nb_rows, uint nb_cols, uint nb_bombs, uint x , uint y);

void game_delete(game g);

int get_number_bombs_around(game g, uint i, uint j);

uint get_nb_rows(game g);

uint get_nb_cols(game g);

uint get_nb_bombs(game g);

void set_number_bombs_around(game g, uint i, uint j, uint nb);

bool is_flagged(game g, uint i, uint j) ;

void pose_flag(game g, uint i, uint j);

void remove_flag(game g, uint i, uint j);

bool is_shown(game g, uint i, uint j);

void show(game g, uint i, uint j);

void reveal_case(game g, uint i, uint j);

#endif