#include <time.h>

#include "SDL2/SDL.h"
#include "tetris.h"
#include "tetris_ui.h"
#include "tetris_ui_sdl.h"

static void update_level_score_lines(tetris_app_t *app, tetris_move_response_t *response, stone_type_t *type) {
	if ( !response->was_moved ) {
		/* redundant points code */ 
		if (response->full_lines != NULL) {
			app->lines += response->full_lines->cnt;
			app->score += response->full_lines->cnt * 150 * app->level;
			if ( (app->lines >= (app->level * 10)) ) {
				app->level++;
				if ( app->update_intervall > 100 ) {
					app->update_intervall -= 70;
				}
			}
			if( Mix_PlayChannel( -1, app->sfx4, 0 ) == -1 ) { 
				printf("Error play chnk: %s\n", Mix_GetError());
			}
		}
		app->score += (*type * 5);
	}
}

static void tetris_create_new_stone(tetris_app_t *app, tetris_create_stone_response_t *stone_response) {
	
	if ( app->next_stone == TETRIS_NO_STONE ) {
		app->next_stone = tetris_random_stone();
	}
	tetris_create_stone(app->next_stone, app->tetris, stone_response);
	
	app->next_stone = tetris_random_stone();
}

static void start_new_game(tetris_app_t *app, tetris_create_stone_response_t *stone_response, field_type_t fieldtype) {
	if ( app->timer != 0 ) {
		SDL_RemoveTimer(app->timer);
		app->timer = 0;
	}
	//tetris_stop(app->tetris);
	//tetris_reset(app->tetris);
	
	if (app->tetris != NULL) {
		tetris_free(&app->tetris);
	}
	app->tetris = tetris_new(fieldtype);
	tetris_start(app->tetris);
	app->score = 0;
	app->level = 1;
	app->lines = 0;
	app->next_stone = TETRIS_NO_STONE;
	app->update_intervall = 800;
	tetris_create_new_stone(app, stone_response);
	//tetris_create_stone_random(app->tetris, stone_response);
	print_tetris_field_sdl(app);
	app->timer = SDL_AddTimer(app->update_intervall, sdl_update_down, (void*)app);
}

static void tetris_check_game_over(tetris_app_t *app, tetris_create_stone_response_t *stone_response) {
	if (stone_response->type == TETRIS_MAX_STONE) {
		print_tetris_field_sdl(app);
		tetris_stop(app->tetris);
		if ( app->timer != 0 ) {
			SDL_RemoveTimer(app->timer);
			app->timer = 0;
		}
		print_tetris_game_over_sdl(app);
	}
}



void
loop(tetris_app_t *app)
{
    SDL_Event e;
	while (SDL_PollEvent(&e)) {

        if (e.type == SDL_QUIT) {
            app->done = 1;
            return;
        }		
		
		tetris_move_response_t response;
		response.full_lines = NULL;
		tetris_create_stone_response_t stone_response;		
		tetris_reset_move_response(app->tetris, &response);		
		
		
		if (e.type == SDL_KEYDOWN) {
			switch(e.key.keysym.sym) {
				/* quit application */
				case SDLK_ESCAPE: 	app->done = 1; break;	
				/* start new game */
				case SDLK_F1:		start_new_game(app, &stone_response, TETRIS_NORMAL);
									break;
				case SDLK_F2:		start_new_game(app, &stone_response, TETRIS_LARGE);
									break;
				case SDLK_F3:		start_new_game(app, &stone_response, TETRIS_EXTRA_LARGE);
									break;
				/* pause game */
				case SDLK_F4: 		if ( !tetris_was_paused(app->tetris) && tetris_is_running(app->tetris)) {
										tetris_pause(app->tetris);	
									} else if ( tetris_was_paused(app->tetris) ) {
										tetris_continue(app->tetris);
									}
									
									print_tetris_pause_sdl(app);
									SDL_UpdateWindowSurface(app->window);
									break;	
				/* move current stone left */		
				case SDLK_LEFT:		if ( !tetris_was_paused(app->tetris) && tetris_is_running(app->tetris) ) {
										tetris_move_stone_left(app->tetris, &response);
									}
									break;					
				/* move current stone right */
				case SDLK_RIGHT:	if ( !tetris_was_paused(app->tetris) && tetris_is_running(app->tetris) ) {
										tetris_move_stone_right(app->tetris, &response);
									}
									break;					
				/* move current stone down */
				case SDLK_DOWN:		if ( !tetris_was_paused(app->tetris) && tetris_is_running(app->tetris) ) {
										stone_type_t type = app->tetris->active_stone.type;
										tetris_move_stone_down(app->tetris, &response);
										if ( !response.was_moved ) {
											if( Mix_PlayChannel( -1, app->sfx3, 0 ) == -1 ) { 
												printf("Error play chnk: %s\n", Mix_GetError());
											}
											update_level_score_lines(app, &response, &type);
											tetris_create_new_stone(app, &stone_response);
										}
									}
									break;						
				/* rotate current stone */
				case SDLK_UP:		if ( !tetris_was_paused(app->tetris) ) {
										tetris_rotate_stone(app->tetris, &response);
										if( Mix_PlayChannel( -1, app->sfx2, 0 ) == -1 ) { 
											printf("Error play chnk: %s\n", Mix_GetError());
										}
									}
									break;					
				
			}
		}
		
		if ( e.type == SDL_USEREVENT) {
			if ( !tetris_was_paused(app->tetris) ) {
				stone_type_t type = app->tetris->active_stone.type;
				tetris_move_stone_down(app->tetris, &response);
				if ( !response.was_moved ) {
					if( Mix_PlayChannel( -1, app->sfx3, 0 ) == -1 ) { 
						printf("Error play chnk: %s\n", Mix_GetError());
					}
					update_level_score_lines(app, &response, &type);
					tetris_create_new_stone(app, &stone_response);
				}
			}
		}
		
		if( response.was_moved || response.was_rotated ) {
			print_tetris_field_sdl(app);
		}
		
		tetris_check_game_over(app, &stone_response);
		
		SDL_UpdateWindowSurface(app->window);
		
		//If there is no music playing 
		if( Mix_PlayingMusic() == 0 ) { 
		//Play the music 
			if( Mix_PlayMusic( app->sfx, -1 ) == -1 ) {
				printf("Error play music: %s\n", Mix_GetError());
			} 
		}
		
		//SDL_Log("run run SDL run!!!\n");
    }
}

int
main(int argc, char *argv[])
{
	srand(time(NULL));
	tetris_app_t app;	
	
	int status = init_tetris_app_sdl(&app);
	if (status > 0) return status;

    while (!app.done) {
        loop(&app);
		SDL_Delay(1); /* wait 1ms to reduce cpu usage. Here we should use another kind of logic with animations */
    }
	
	cleanup_tetris_app_sdl(&app);
	
    return 0;
}