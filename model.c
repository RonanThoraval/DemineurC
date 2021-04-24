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
#define RONAN "ronan.jpeg"
#define BOMB "bombe.png"

/* **************************************************************** */

struct Env_t {
  game g;
  uint w; //width of window
  uint h; //height of window
  uint square_size; // Size of a square
  uint bar_size; // Size of the status bar
  SDL_Texture *text;
  SDL_Texture *flag;
  SDL_Texture *bomb;
  SDL_Texture *ronan;
  SDL_Color colors[8]; // Tab of the various colors used
  uint nb_rows;
  uint nb_cols;
  uint nb_bombs;
  bool has_changed; //True if the player has clicked, or the window has changed size
  bool first_click; //True if there has been a first click, false else
  bool winning; //True if the game is won
  bool losing; //True if the game is lost
  bool menu; // True if the player is on the menu
  bool choose_parameters; // If true, the player can choose parameters of game
};

/* **************************************************************** */

Env *init(SDL_Window *win, SDL_Renderer *ren, int argc, char *argv[]) {
  Env *env = malloc(sizeof(struct Env_t));
  env->menu=true;

  /* get current window size */
  int w, h;
  SDL_GetWindowSize(win, &w, &h);
  env->w=w;
  env->h=h;

  env->flag = IMG_LoadTexture(ren, FLAG);
  if (!env->flag) ERROR("IMG_LoadTexture: %s\n", FLAG);

  env->bomb = IMG_LoadTexture(ren, BOMB);
  if (!env->bomb) ERROR("IMG_LoadTexture: %s\n", BOMB);

  env->ronan = IMG_LoadTexture(ren,RONAN);
  if (!env->ronan) ERROR("IMG_LoadTexture: %s\n", RONAN);

  env->nb_cols=8;
  env->nb_rows=8;
  env->nb_bombs=10;
  env->first_click=false;
  env->bar_size = fmin(env->h / 10, env->w / 7);
  env->square_size=fmin(env->w/env->nb_cols,(env->h-env->bar_size)/env->nb_rows);
  env->has_changed=true;
  env->winning=false;
  env->choose_parameters=false;

  SDL_Color bleu={0,0,200,200};
  SDL_Color noir={255,255,255,200};
  SDL_Color jaune={200,200,0,200};
  SDL_Color rouge={150,0,0,200};
  SDL_Color violet={200,0,200,200};
  SDL_Color vert={0,125,0,255};
  SDL_Color cyan={0,125,200,200};
  SDL_Color gris={0,0,0,200};
  env->colors[0]=bleu;
  env->colors[6]=cyan;
  env->colors[2]=rouge;
  env->colors[3]=jaune;
  env->colors[4]=gris;
  env->colors[5]=violet;
  env->colors[1]=vert;
  env->colors[7]=noir;
  return env;
}

/************ RENDER TEXT ***********************/

void render_text(SDL_Window *win, SDL_Renderer *ren, Env *env, uint color, char * texte, uint x, uint y, uint text_width, uint text_height, uint text_size) {
  SDL_Rect rect;

  TTF_Font* font = TTF_OpenFont(FONT, text_size);
  if (!font) ERROR("TTF_OpenFont: %s\n", FONT);
  TTF_SetFontStyle(font, TTF_STYLE_NORMAL);
  SDL_Surface* surf = TTF_RenderText_Blended(font, texte, env->colors[color]);
  env->text = SDL_CreateTextureFromSurface(ren, surf);
  SDL_FreeSurface(surf);
  TTF_CloseFont(font);

  SDL_QueryTexture(env->text, NULL, NULL, &rect.w, &rect.h);
  if (text_width!=0 && text_height!=0) {
    rect.w= text_width;
    rect.h= text_height;
  }
  rect.x =  x - rect.w/2  ;
  rect.y =  y - rect.h/2 ;
  SDL_RenderCopy(ren, env->text, NULL, &rect);
}

/************ RENDER IMAGE ***************/

void render_image(SDL_Renderer *ren, SDL_Texture * image, uint x, uint y, uint image_width, uint image_height) {
  SDL_Rect rect;
  rect.w=image_width;
  rect.h=image_height;
  rect.x = x;
  rect.y = y;
  SDL_RenderCopy(ren, image, NULL, &rect);
}


/**************** RENDER STATUS BAR ******************/

void render_bar(SDL_Window *win, SDL_Renderer *ren, Env *env) {

  SDL_SetRenderDrawColor(ren, 0, 0, 0, SDL_ALPHA_OPAQUE);
  SDL_RenderDrawLine(ren,env->w/2,env->h-env->bar_size+1,env->w-1,env->h-env->bar_size+1);
  SDL_RenderDrawLine(ren,env->w/2,env->h-1,env->w-1,env->h-1);
  SDL_RenderDrawLine(ren,env->w/2,env->h-env->bar_size+1,env->w/2,env->h-1);
  SDL_RenderDrawLine(ren,env->w-1,env->h-env->bar_size+1,env->w-1,env->h-1);
  SDL_RenderDrawLine(ren,3*env->w/4,env->h-env->bar_size+1,3*env->w/4,env->h-1);

  render_text(win,ren,env,0,"Nouvelle Partie",7*env->w/8,env->h-(env->bar_size)/2,3*env->w/16,env->bar_size/2,env->bar_size);
  render_text(win,ren,env,0,"Menu",5*env->w/8,env->h-env->bar_size/2,2*env->w/16,env->bar_size/2,env->bar_size);

  render_image(ren, env->flag, 5, env->h-env->bar_size+1, env->bar_size - 5, env->bar_size - 5);
  render_image(ren, env->bomb, env->bar_size*2+5, env->h-env->bar_size+1, env->bar_size-5, env->bar_size-5);
  char nb_bomb[3];
  sprintf(nb_bomb, "%u", env->nb_bombs);
  char nb_flag[3];
  sprintf(nb_flag, "%u", get_nb_flags(env->g));
  render_text(win, ren, env, 0, nb_flag, env->bar_size*3/2, env->h-env->bar_size/2, 0, 0, env->bar_size);
  render_text(win, ren, env, 0, nb_bomb, env->bar_size*7/2, env->h-env->bar_size/2, 0, 0, env->bar_size);
}

/************ RENDER MENU ******************/

void render_menu(SDL_Window *win, SDL_Renderer *ren, Env *env){

  SDL_SetRenderDrawColor(ren, 0, 0, 0, SDL_ALPHA_OPAQUE);
  SDL_RenderDrawLine(ren,env->w/2,0,env->w/2,env->h);
  SDL_RenderDrawLine(ren,0,env->h/2,env->w,env->h/2);

  render_text(win,ren,env,0,"Facile : 8x8", env->w/4,env->h/4,0,0,fmin(env->w/20,env->h/20));
  render_text(win,ren,env,0,"Moyen : 13x13", 3*env->w/4,env->h/4,0,0,fmin(env->w/20,env->h/20));
  render_text(win,ren,env,0,"Difficile : 17x17", env->w/4,3*env->h/4,0,0,fmin(env->w/20,env->h/20));
  if (!env->choose_parameters) {
    render_text(win,ren,env,0,"Choisir les variables", 3*env->w/4,3*env->h/4,0,0,fmin(env->w/20,env->h/20));
  }
}

void render_parameters(SDL_Window *win, SDL_Renderer *ren, Env *env) {

  SDL_RenderDrawLine(ren,env->w/2,5*env->h/8,env->w,5*env->h/8);
  SDL_RenderDrawLine(ren,env->w/2,6*env->h/8,env->w,6*env->h/8);
  SDL_RenderDrawLine(ren,env->w/2,7*env->h/8,env->w,7*env->h/8);
  SDL_RenderDrawLine(ren,3*env->w/4,env->h/2,3*env->w/4,env->h-env->h/8);
  SDL_RenderDrawLine(ren,10*env->w/12,env->h/2,10*env->w/12,env->h-env->h/8);
  SDL_RenderDrawLine(ren,11*env->w/12,env->h/2,11*env->w/12,env->h-env->h/8);
  render_text(win,ren,env,0,"Lignes : ",env->w/2+env->w/8,env->h/2+env->h/16,0,0,fmin(env->h/24,env->w/24));
  render_text(win,ren,env,0,"Colonnes : ",env->w/2+env->w/8,5*env->h/8+env->h/16,0,0,fmin(env->h/24,env->w/24));
  render_text(win,ren,env,0,"Bombes : ",env->w/2+env->w/8,6*env->h/8+env->h/16,0,0,fmin(env->h/24,env->w/24));
  render_text(win,ren,env,0,"OK",3*env->w/4,7*env->h/8+env->h/16,0,0,fmin(env->h/24,env->w/24));

  char nb_rows[3];
  char nb_cols[3];
  char nb_bombs[3];
  sprintf(nb_rows,"%u",env->nb_rows);
  sprintf(nb_cols,"%u",env->nb_cols);
  sprintf(nb_bombs,"%u",env->nb_bombs);
  render_text(win,ren,env,0,nb_rows,10*env->w/12+env->w/24,4*env->h/8+env->h/16,0,0,fmin(env->h/24,env->w/24));
  render_text(win,ren,env,0,nb_cols,10*env->w/12+env->w/24,5*env->h/8+env->h/16,0,0,fmin(env->h/24,env->w/24));
  render_text(win,ren,env,0,nb_bombs,10*env->w/12+env->w/24,6*env->h/8+env->h/16,0,0,fmin(env->h/24,env->w/24));
  for (uint i=1 ; i<=3 ; i++) {
    render_text(win,ren,env,0,"+",11*env->w/12+env->w/24,(3+i)*env->h/8+env->h/16,0,0,fmin(env->h/24,env->w/24));
    render_text(win,ren,env,0,"-",9*env->w/12+env->w/24,(3+i)*env->h/8+env->h/16,0,0,fmin(env->h/24,env->w/24));
  }
  
}


/* **************************** RENDER ************************************ */

void render(SDL_Window *win, SDL_Renderer *ren, Env *env) {
  if (env->menu) {
    render_menu(win,ren,env);
    if (env->choose_parameters) {
      render_parameters(win,ren,env);
    }
    return;
  }
  SDL_Rect rect;

    env->bar_size = fmin(env->h / 10, env->w / 7);
    env->square_size=fmin(env->w/env->nb_cols,(env->h-env->bar_size)/env->nb_rows);
    render_bar(win,ren,env);

    if (env->winning) {
      render_image(ren, env->ronan,0,0,env->w,env->h-env->bar_size);
      render_text(win,ren,env,0,"Ronan applaudit.",env->w/2,env->h/2,0,0,env->w/10);
      return;
    }


    SDL_SetRenderDrawColor(ren, 0, 0, 0, SDL_ALPHA_OPAQUE);
    for (uint i=0 ; i<=env->nb_rows; i++) {
        SDL_RenderDrawLine(ren,0,i*env->square_size,env->square_size*env->nb_cols,i*env->square_size);
    }

    for (uint j=0 ; j<=env->nb_cols ; j++) {
        SDL_RenderDrawLine(ren,j*env->square_size,0,j*env->square_size,env->square_size*env->nb_rows);
    }

    if (!env->first_click) {
      return;
    }

    for (uint i=0 ; i<get_nb_rows(env->g) ; i++) {
        for (uint j=0 ; j<get_nb_cols(env->g) ; j++) {
          if (is_shown(env->g,i,j)) {
              if (get_number_bombs_around(env->g,i,j)==0) {
                SDL_SetRenderDrawColor( ren, 100, 100, 100, 255 );
              } else {
                SDL_SetRenderDrawColor( ren, 125, 125, 125, 255 );
              }
              rect.x=j*env->square_size+1;
              rect.y=i*env->square_size+1;
              rect.w=env->square_size-1;
              rect.h=env->square_size-1;
              SDL_RenderFillRect(ren,&rect);
            } else {
              SDL_SetRenderDrawColor(ren,210,210,210,255);
              rect.x=j*env->square_size+1;
              rect.y=i*env->square_size+1;
              rect.w=env->square_size-2;
              rect.h=env->square_size-2;
              SDL_RenderFillRect(ren,&rect);
              SDL_SetRenderDrawColor(ren,190,190,190,255);
              rect.x=j*env->square_size+1;
              rect.y=i*env->square_size+1;
              rect.w=env->square_size-4;
              rect.h=env->square_size-4;
              SDL_RenderFillRect(ren,&rect);
            }
            if (is_shown(env->g,i,j) && get_number_bombs_around(env->g,i,j)>0) {
              char s[2];
              sprintf(s, "%u", get_number_bombs_around(env->g,i,j));
              uint x = j*env->square_size + env->square_size/2;
              uint y = i*env->square_size + env->square_size/2;
              render_text(win,ren,env,get_number_bombs_around(env->g,i,j)-1,s,x,y,0,0,2*env->square_size/3);

            }
            if (env->losing && get_number_bombs_around(env->g,i,j)==-1) {
              render_image(ren,env->bomb,j*env->square_size,i*env->square_size,env->square_size,env->square_size);
            }
            if (!env->losing && is_flagged(env->g,i,j)) {
              render_image(ren,env->flag,j*env->square_size,i*env->square_size,env->square_size,env->square_size);
            }
        }
    }
    if (env->losing) {
      render_text(win,ren,env,2,"Perdu",env-> w/2,env->h/2,0,0,env->w/10);
    }
}

void set_not_changed(Env *env) {
  env->has_changed=false;
}

bool get_changed(Env *env) {
  return env->has_changed;
}

/* Choose parameters in function of the click on the menu */
void game_from_menu( Env* env,uint width, uint height, float x, float y) {
  if (x<width/2 && y<height/2) {
    env->nb_rows=8;
    env->nb_cols=8;
    env->nb_bombs=10;
    env->choose_parameters=false;
    env->menu=false;
  } else if (x>width/2 && y<height/2) {
    env->nb_rows=13;
    env->nb_cols=13;
    env->nb_bombs=30;
    env->choose_parameters=false;
    env->menu=false;
  } else if (x<width/2 && y>height/2) {
    env->nb_rows=17;
    env->nb_cols=17;
    env->nb_bombs=60;
    env->choose_parameters=false;
    env->menu=false;
  } else if (x>width/2 && y>height/2) {
    uint w=env->w;
    uint h=env->h;
    if (env->choose_parameters) {
      if (x>w/2 && y>7*h/8) {
        env->choose_parameters=false;
        env->menu=false;
      }
       else if (x>11*w/12 && y>6*h/8) {
        env->nb_bombs++;
      } else if (x>11*w/12 && y>5*h/8) {
        env->nb_cols++;
      } else if (x>11*w/12 && y>4*h/8) {
        env->nb_rows++;
      } else if (x>9*w/12 && x<10*w/12 && y>6*h/8) {
		  if(env->nb_bombs!=1){
			  env->nb_bombs--;
		  }
      } else if (x>9*w/12 && x<10*w/12 && y>5*h/8) {
		  if(env->nb_cols!=1){
			  env->nb_cols--;
		  }
      } else if (x>9*w/12 && x<10*w/12 && y>4*h/8) {
		  if(env->nb_rows!=1){
			  env->nb_rows--;
		  }
      }
    } else {
      env->choose_parameters=true;
      env->nb_rows=8;
      env->nb_cols=8;
      env->nb_bombs=10;
    }
  }
}

/* **************************************************************** */

bool process(SDL_Window *win, SDL_Renderer *ren, Env *env, SDL_Event *e) {
  int w, h;
  SDL_GetWindowSize(win, &w, &h);
  if (env->w!=w || env->h !=h) {
    env->has_changed=true;
    env->w=w;
    env->h=h;
  }
  if (e->type == SDL_QUIT || (e->type == SDL_KEYDOWN && e->key.keysym.sym == SDLK_ESCAPE)) {
    return true;
  }
  if (e->type==SDL_MOUSEBUTTONDOWN) {
      env->has_changed=true;
      SDL_Point mouse;
      SDL_GetMouseState(&mouse.x,&mouse.y);
      uint i = (mouse.y - mouse.y % env->square_size) / env->square_size;
      uint j = (mouse.x - mouse.x % env->square_size) / env->square_size;
      if (env->menu) {
        game_from_menu(env,env->w,env->h, mouse.x, mouse.y);
        return false;
      }
      if (i>=env->nb_rows || j>=env->nb_cols) {
        /* New game */
        if (mouse.x>=3*env->w/4 && mouse.y>=env->h-env->bar_size) {
          env->first_click=false;
          env->winning=false;
          env->losing=false;
		  if(env->g!=NULL){
			game_delete(env->g);
          	env->g=NULL;
		  }
        } else if (mouse.x>=env->w/2 && mouse.y>=env->h-env->bar_size) {
          env->first_click=false;
          env->winning=false;
          env->losing=false;
          if(env->g!=NULL){
			game_delete(env->g);
          	env->g=NULL;
		  }
          env->menu=true;
        }
        return false;
      }
      if (env->losing) {
        return false;
      }
      if (e->button.button==SDL_BUTTON_LEFT) {
          /* If first click, init game */
          if (env->first_click==false) {
            env->first_click=true;
            game g= game_init(env->nb_rows,env->nb_cols,env->nb_bombs,i,j);
            env->g=g;
          }
          if (get_number_bombs_around(env->g,i,j)!=-1) {
            reveal_case(env->g,i,j);
          } else {
            env->losing=!is_flagged(env->g,i,j);
          }
      } else if (e->button.button==SDL_BUTTON_RIGHT) {
		  	if(!is_shown(env->g,i,j)){
	          if (!is_flagged(env->g,i,j)) {
	              pose_flag(env->g,i,j);
	          } else {
	              remove_flag(env->g,i,j);
	          }
			}
      }
      if (env->first_click && has_won(env->g)) {
        env->winning=true;
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
  SDL_DestroyTexture(env->bomb);
  SDL_DestroyTexture(env->ronan);

  free(env);
}

/* **************************************************************** */
