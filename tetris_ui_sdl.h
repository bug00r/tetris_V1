#ifndef TETRIS_UI_SDL_H
#define TETRIS_UI_SDL_H
#include "SDL2/SDL.h"
#include "SDL2/SDL_ttf.h"
#include "SDL2/SDL_mixer.h"

#include "tetris.h"
#include "tetris_ui.h"

/*
	Here we have all ui responsible rendering Functions based on Tetris and SDL.
*/

typedef struct {
	SDL_Window *window;
	SDL_Renderer *renderer;
	tetris_t* tetris;
	SDL_TimerID timer;
	TTF_Font *headline_font;
	TTF_Font *score_font;
	TTF_Font *level_font;
	int done;
	int width;
	int height;
	SDL_Color color_headline;
	SDL_Color color_label;
	SDL_Color color_value;
	unsigned int score;
	unsigned int level;
	unsigned int lines;
	unsigned int update_intervall;
	unsigned int field_width_px;
	unsigned int field_height_px;
	stone_type_t next_stone;
	Mix_Music *sfx;
	Mix_Chunk *sfx2;	
	Mix_Chunk *sfx3;
	Mix_Chunk *sfx4;
	
} tetris_app_t;

int init_tetris_app_sdl(tetris_app_t* tetris_app);
void cleanup_tetris_app_sdl(tetris_app_t* tetris_app);

void print_tetris_field_sdl(tetris_app_t* tetris_app);

void print_tetris_headline_sdl(tetris_app_t* tetris_app);

void print_tetris_pause_sdl(tetris_app_t* tetris_app);

void print_tetris_game_over_sdl(tetris_app_t* tetris_app);

Uint32 sdl_update_down(Uint32 interval, void *param);
#endif