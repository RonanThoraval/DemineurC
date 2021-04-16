#ifndef __GAME_H__
#define __GAME_H__
#include <stdbool.h>


typedef struct game_s *game;
typedef unsigned int uint ;

game game_init(int nb_rows, int nb_cols, int nb_bombs, int x , int y);

void game_delete(game g);

int get_number_bombs_around(game g, int i, int j);

int get_nb_rows(game g);

int get_nb_cols(game g);

int get_nb_bombs(game g);

int get_nb_flags(game g);

void set_number_bombs_around(game g, int i, int j, int nb);

bool is_flagged(game g, int i, int j) ;

void pose_flag(game g, int i, int j);

void remove_flag(game g, int i, int j);

bool is_shown(game g, int i, int j);

void show(game g, int i, int j);

void reveal_case(game g, int i, int j);

void game_print(game g );

bool has_won(game g);

#endif