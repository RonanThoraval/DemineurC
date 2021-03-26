// SDL2 Demo by aurelien.esnard@u-bordeaux.fr

#include "model.h"
#include <SDL.h>
#include <SDL_image.h>  // required to load transparent texture from PNG
#include <SDL_ttf.h>    // required to use TTF fonts
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "game.h"

#define FONT "arial.ttf"
#define FLAG "drap.png"

/* **************************************************************** */

struct Env_t {
  game g;
  uint square_size;
  SDL_Texture *text;
  SDL_Texture *flag;
  SDL_Color colors[8];
  uint nb_rows;
  uint nb_cols;
  uint nb_bombs;
  bool first_click; //True if there has been a first click, false else
};

/* **************************************************************** */

Env *init(SDL_Window *win, SDL_Renderer *ren, int argc, char *argv[]) {
  Env *env = malloc(sizeof(struct Env_t));

  /* get current window size */
  int w, h;
  SDL_GetWindowSize(win, &w, &h);

  env->flag = IMG_LoadTexture(ren, FLAG);
  if (!env->flag) ERROR("IMG_LoadTexture: %s\n", FLAG);

  env->nb_cols=8;
  env->nb_rows=8;
  env->nb_bombs=10;
  env->first_click=false;
  env->square_size=fmin(w/env->nb_cols,h/env->nb_rows);

  SDL_Color bleu={0,0,255,255};
  SDL_Color noir={255,255,255,255};
  SDL_Color jaune={255,255,0,255};
  SDL_Color rouge={255,0,0,255};
  SDL_Color violet={255,0,255,255};
  SDL_Color vert={0,255,0,255};
  SDL_Color cyan={0,255,255,255};
  SDL_Color gris={125,125,125,255};
  env->colors[0]=bleu;
  env->colors[1]=vert;
  env->colors[2]=rouge;
  env->colors[3]=jaune;
  env->colors[4]=gris;
  env->colors[5]=violet;
  env->colors[6]=cyan;
  env->colors[7]=noir;
  return env;
}

/* **************************************************************** */

void render(SDL_Window *win, SDL_Renderer *ren, Env *env) {
    SDL_Rect rect;
    /* get current window size */
    int w, h;
    SDL_GetWindowSize(win, &w, &h);

    env->square_size=fmin(w/env->nb_cols,h/env->nb_rows);

    SDL_SetRenderDrawColor(ren, 0, 0, 0, SDL_ALPHA_OPAQUE);
    for (uint i=0 ; i<=env->nb_rows; i++) {
        SDL_RenderDrawLine(ren,0,i*env->square_size,env->square_size*env->nb_rows,i*env->square_size);
    }

    for (uint j=0 ; j<=env->nb_cols ; j++) {
        SDL_RenderDrawLine(ren,j*env->square_size,0,j*env->square_size,env->square_size*env->nb_rows);
    }

    if (!env->first_click) {
      return;
    }

    for (uint i=0 ; i<get_nb_rows(env->g) ; i++) {
        for (uint j=0 ; j<get_nb_cols(env->g) ; j++) {
            if (is_shown(env->g,i,j) && get_number_bombs_around(env->g,i,j)>=0) {
              TTF_Font* font = TTF_OpenFont(FONT, env->square_size);
              if (!font) ERROR("TTF_OpenFont: %s\n", FONT);
              TTF_SetFontStyle(font, TTF_STYLE_NORMAL);
              char s[2];
              sprintf(s, "%u", get_number_bombs_around(env->g,i,j));
              SDL_Surface* surf = TTF_RenderText_Blended(font, s, env->colors[get_number_bombs_around(env->g,i,j)-1]);
              env->text = SDL_CreateTextureFromSurface(ren, surf);
              SDL_FreeSurface(surf);
              TTF_CloseFont(font);

              SDL_QueryTexture(env->text, NULL, NULL, &rect.w, &rect.h);
              rect.x =  j*env->square_size + env->square_size/2 - rect.w/2  ;
              rect.y =  i*env->square_size + env->square_size/2 - rect.h/2 ;
              SDL_RenderCopy(ren, env->text, NULL, &rect);
            }
            if (is_flagged(env->g,i,j)) {
              rect.w=env->square_size;
              rect.h=env->square_size;
              rect.x = j*env->square_size;
              rect.y = i*env->square_size;
              SDL_RenderCopy(ren, env->flag, NULL, &rect);
            }
        }
    }


}

/* **************************************************************** */

bool process(SDL_Window *win, SDL_Renderer *ren, Env *env, SDL_Event *e) {
  if (e->type == SDL_QUIT) {
    return true;
  }
  if (e->type==SDL_MOUSEBUTTONDOWN) {
      SDL_Point mouse;
      SDL_GetMouseState(&mouse.x,&mouse.y);
      uint i = (mouse.y - mouse.y % env->square_size) / env->square_size;
      uint j = (mouse.x - mouse.x % env->square_size) / env->square_size;
      if (e->button.button==SDL_BUTTON_LEFT) {
          /* If first click, init game */
          if (env->first_click==false) {
            env->first_click=true;
            printf("First click : %u %u\n",i,j);
            game g= game_init(env->nb_rows,env->nb_cols,env->nb_bombs,i,j);
            env->g=g;
          }
          if (get_number_bombs_around(env->g,i,j)!=-1) {
            reveal_case(env->g,i,j);
          } else {
              return true;
          }
      } else if (e->button.button==SDL_BUTTON_RIGHT) {
          if (!is_flagged(env->g,i,j)) {
              pose_flag(env->g,i,j);
          } else {
              remove_flag(env->g,i,j);
          }
      }
  }

  return false;
}

/* **************************************************************** */

void clean(SDL_Window *win, SDL_Renderer *ren, Env *env) {
  if (env->first_click) {
    game_delete(env->g);
  }
  SDL_DestroyTexture(env->text);
  SDL_DestroyTexture(env->flag);

  free(env);
}

/* **************************************************************** */
