#include "tetris_ui_sdl.h"

extern unsigned char _binary_tetris_ttf_start;
extern unsigned char _binary_tetris_ttf_size;

extern unsigned char _binary_tetris_ogg_start;
extern unsigned char _binary_tetris_ogg_size;

extern unsigned char _binary_tetris2_ogg_start;
extern unsigned char _binary_tetris2_ogg_size;

extern unsigned char _binary_tetris3_ogg_start;
extern unsigned char _binary_tetris3_ogg_size;

extern unsigned char _binary_tetris4_ogg_start;
extern unsigned char _binary_tetris4_ogg_size;

static int __tetris_init_sdl_screen(tetris_app_t* tetris_app) {
	SDL_Surface *surface;
    /* Enable standard application logging */
    SDL_LogSetPriority(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO);

    /* Initialize SDL */
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_AUDIO) != 0) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_Init fail : %s\n", SDL_GetError());
        return 1;
    }

    /* Create window and renderer for given surface */
    tetris_app->window = SDL_CreateWindow("Bug0r's Tetris", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 524, 524, 0);
    if (!tetris_app->window) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Window creation fail : %s\n",SDL_GetError());
        return 1;
    }
    surface = SDL_GetWindowSurface(tetris_app->window);
    tetris_app->renderer = SDL_CreateSoftwareRenderer(surface);
    if (!tetris_app->renderer) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Render creation for surface fail : %s\n",SDL_GetError());
        return 1;
    }
	
	return 0;
}


static int __tetris_init_sdl_fonts(tetris_app_t* tetris_app) {
		/* ##### font loading and setting title */
	// load font.ttf at size 16 into font
	if (TTF_Init() < 0) {
		printf("Font opening error : %s\n",TTF_GetError());
	}
	
	SDL_RWops* font_mem = SDL_RWFromMem(&_binary_tetris_ttf_start, (int)&_binary_tetris_ttf_size);
    if (font_mem == NULL) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Load font from memory fail : %s\n",SDL_GetError());
        return 1;
    }
	
	tetris_app->headline_font = TTF_OpenFontRW(font_mem, 0, 36);
	if(!tetris_app->headline_font) {
		printf("Font opening error : %s\n",TTF_GetError());
	}
	
	tetris_app->score_font = TTF_OpenFontRW(font_mem, 0, 18);
	if(!tetris_app->score_font) {
		printf("Font opening score font : %s\n",TTF_GetError());
	}
	
	tetris_app->level_font = TTF_OpenFontRW(font_mem, 0, 18);
	if(!tetris_app->level_font) {
		printf("Font opening score font : %s\n",TTF_GetError());
	}
	
	return 0;
}

static int __tetris_init_sdl_sounds(tetris_app_t* tetris_app) {

	int flags=MIX_INIT_OGG|MIX_INIT_MOD;
	int initted=Mix_Init(flags);
	if(initted&flags != flags) {
		printf("Mix_Init: Failed to init required ogg and mod support!\n");
		printf("Mix_Init: %s\n", Mix_GetError());
		return 1;
	}
	
	//Initialize SDL_mixer 
	if( Mix_OpenAudio( 22050, MIX_DEFAULT_FORMAT, 2, 4096 ) == -1 ) {
		printf("Audio init error : %s", Mix_GetError());
        return 1; 
	}
	
	SDL_RWops* sfx_mem = SDL_RWFromMem(&_binary_tetris_ogg_start, (int)&_binary_tetris_ogg_size);
    if (sfx_mem == NULL) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Load sfx from memory fail : %s\n",SDL_GetError());
        return 1;
    }

	//tetris_app->sfx = Mix_LoadMUSType_RW(sfx_mem, MUS_MOD, 0);
	tetris_app->sfx = Mix_LoadMUSType_RW(sfx_mem, MUS_OGG, 0);
	//tetris_app->sfx = Mix_LoadMUS("tetris.ogg");
	//tetris_app->sfx = Mix_LoadMUSType_RW(sfx_mem, MUS_OGG, 0);
	
	if(tetris_app->sfx == NULL) {
		printf("Mix_LoadMUS_RW: %s\n", Mix_GetError());
	}
	
	SDL_RWops* sfx_mem2 = SDL_RWFromMem(&_binary_tetris2_ogg_start, (int)&_binary_tetris2_ogg_size);
    if (sfx_mem2 == NULL) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Load sfx2 from memory fail : %s\n",SDL_GetError());
        return 1;
    }
	//SDLCALL Mix_QuickLoad_RAW(Uint8 *mem, Uint32 len)
	tetris_app->sfx2 = Mix_LoadWAV_RW(sfx_mem2, 0);
	//tetris_app->sfx2 = Mix_QuickLoad_RAW(&_binary_tetris2_ogg_start, (int)&_binary_tetris2_ogg_size);
	if(!tetris_app->sfx2) {
		printf("tetris_app->sfx2: %s\n", Mix_GetError());
		return 1;
	}
	
	SDL_RWops* sfx_mem3 = SDL_RWFromMem(&_binary_tetris3_ogg_start, (int)&_binary_tetris3_ogg_size);
    if (sfx_mem3 == NULL) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Load sfx3 from memory fail : %s\n",SDL_GetError());
        return 1;
    }

	tetris_app->sfx3 = Mix_LoadWAV_RW(sfx_mem3, 0);
	if(!tetris_app->sfx3) {
		printf("tetris_app->sfx3: %s\n", Mix_GetError());
		return 1;
	}
	
	SDL_RWops* sfx_mem4 = SDL_RWFromMem(&_binary_tetris4_ogg_start, (int)&_binary_tetris4_ogg_size);
    if (sfx_mem4 == NULL) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Load sfx3 from memory fail : %s\n",SDL_GetError());
        return 1;
    }

	tetris_app->sfx4 = Mix_LoadWAV_RW(sfx_mem4, 0);
	if(!tetris_app->sfx4) {
		printf("tetris_app->sfx4: %s\n", Mix_GetError());
		return 1;
	}
	
	return 0;
}

static void __draw_sdl_rect_ouline_rgba(SDL_Renderer *renderer, SDL_Rect *rect, Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
	SDL_SetRenderDrawColor(renderer, r, g, b, a);
	SDL_RenderDrawRect(renderer, rect);
}

static void __draw_rect_outline_rgba(SDL_Renderer *renderer, int x, int y, int width, int height, Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
	SDL_Rect rect;
	rect.x = x;
	rect.y = y;
	rect.w = width;
	rect.h = height;
	__draw_sdl_rect_ouline_rgba(renderer, &rect, r, g, b, a);
}

static void __draw_sdl_rect_rgba(SDL_Renderer *renderer, SDL_Rect *rect, Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
	SDL_SetRenderDrawColor(renderer, r, g, b, a);
	SDL_RenderFillRect(renderer, rect);
}

static void __draw_rect_rgba(SDL_Renderer *renderer, int x, int y, int width, int height, Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
	SDL_Rect rect;
	rect.x = x;
	rect.y = y;
	rect.w = width;
	rect.h = height;
	__draw_sdl_rect_rgba(renderer, &rect, r, g, b, a);
}

static void __draw_rect_sdl_color(SDL_Renderer *renderer, int x, int y, int width, int height, SDL_Color *color) {
	__draw_rect_rgba(renderer, x, y, width, height, color->r, color->g, color->b, color->a);
}

static void __draw_sdl_rect_sdl_color(SDL_Renderer *renderer, SDL_Rect *rect, SDL_Color *color) {
	__draw_rect_rgba(renderer, rect->x, rect->y, rect->w, rect->h, color->r, color->g, color->b, color->a);
}

static char * num_to_string(unsigned int number) {
	int buffsize = vsnprintf(NULL, 0, "%d", (char *)&number);
	buffsize += 1;
	char * buffer = malloc(buffsize);
	vsnprintf(buffer, buffsize, "%d", (char *)&number);
	return buffer;
}

static SDL_Surface * __create_text(const char *text, TTF_Font *font, SDL_Color *color) {
	SDL_Surface *text_surface = NULL;
	if(!(text_surface=TTF_RenderText_Solid(font ,text, *color))) {
		printf("Error drawing text : %s\n", TTF_GetError());
	} 
	return text_surface;
}

static SDL_Surface * __create_uint_text(unsigned int number, TTF_Font *font, SDL_Color *color) {
	char * num_str = num_to_string(number);
	SDL_Surface * text =  __create_text(num_str ,font, color);
	free(num_str);
	return text;
}

static void __draw_text_at_position(SDL_Surface *text, SDL_Surface *text_target, int x, int y) {
	SDL_Rect pos;
	pos.x = x;
	pos.y = y;
	pos.w = text->w;
	pos.h= text->h;
	SDL_BlitSurface(text, NULL, text_target, &pos);
}

void print_tetris_game_over_sdl(tetris_app_t* tetris_app) {

	SDL_Surface * text = __create_text("(= GAME OVER =)", tetris_app->score_font, &tetris_app->color_value);
	
	__draw_text_at_position(text ,SDL_GetWindowSurface(tetris_app->window), 
							(tetris_app->width - text->w) / 2 , (tetris_app->height - text->h) / 2);
	
	SDL_FreeSurface(text); /* No use anymore, could be cached because its a label, need text cache */
}

void print_tetris_pause_sdl(tetris_app_t* tetris_app) {

	SDL_Surface * text = __create_text("-= PAUSE =-", tetris_app->score_font, &tetris_app->color_value);
	
	__draw_text_at_position(text ,SDL_GetWindowSurface(tetris_app->window), 
							(tetris_app->width - text->w) / 2 , (tetris_app->height - text->h) / 2);
	
	SDL_FreeSurface(text); /* No use anymore, could be cached because its a label, need text cache */
}

static void __print_tetris_score_label_sdl(tetris_app_t* tetris_app) {

	SDL_Surface * text = __create_text("Score:", tetris_app->score_font, &tetris_app->color_label);
	
	__draw_text_at_position(text ,SDL_GetWindowSurface(tetris_app->window), 0, 36);
	
	SDL_FreeSurface(text); /* No use anymore, could be cached because its a label, need text cache */
}

static void __print_tetris_score_sdl(tetris_app_t* tetris_app) {

	SDL_Rect rect;
	rect.x = 0;		/* start x */
	rect.y = 54; 	/* start y at 3 * font_size height */
	rect.w = 126;	/* 7 * font_size widht */
	rect.h = 18;	/* font_size height */
	/* clear text */
	__draw_sdl_rect_rgba(tetris_app->renderer, &rect, 0, 0, 0, 255);
	
	SDL_Surface * text = __create_uint_text(tetris_app->score, tetris_app->score_font, &tetris_app->color_value);
	
	__draw_text_at_position(text ,SDL_GetWindowSurface(tetris_app->window), 0, 54);
	
	SDL_FreeSurface(text); /* No use anymore, could be cached because its a label, need text cache */
}

static void __print_tetris_level_label_sdl(tetris_app_t* tetris_app) {

	SDL_Surface * text = __create_text("Level:", tetris_app->level_font, &tetris_app->color_label);
	
	__draw_text_at_position(text ,SDL_GetWindowSurface(tetris_app->window), 0, 72);
	
	SDL_FreeSurface(text); /* No use anymore, could be cached because its a label, need text cache */
}

static void __print_tetris_level_sdl(tetris_app_t* tetris_app) {

	SDL_Rect rect;
	rect.x = 0;		/* start x */
	rect.y = 90; 	/* start y at 3 * font_size height */
	rect.w = 126;	/* 7 * font_size widht */
	rect.h = 18;	/* font_size height */
	/* clear text */
	__draw_sdl_rect_rgba(tetris_app->renderer, &rect, 0, 0, 0, 255);
	
	SDL_Surface * text = __create_uint_text(tetris_app->level, tetris_app->level_font, &tetris_app->color_value);
	
	__draw_text_at_position(text ,SDL_GetWindowSurface(tetris_app->window), 0, 90);
	
	SDL_FreeSurface(text); /* No use anymore, could be cached because its a label, need text cache */

}

static void __print_tetris_lines_label_sdl(tetris_app_t* tetris_app) {

	SDL_Surface * text = __create_text("Lines:", tetris_app->level_font, &tetris_app->color_label);
	
	__draw_text_at_position(text ,SDL_GetWindowSurface(tetris_app->window), 0, 108);
	
	SDL_FreeSurface(text); /* No use anymore, could be cached because its a label, need text cache */

}

static void __print_tetris_lines_sdl(tetris_app_t* tetris_app) {
	SDL_Rect rect;
	rect.x = 0;		/* start x */
	rect.y = 126; 	/* start y at 3 * font_size height */
	rect.w = 126;	/* 7 * font_size widht */
	rect.h = 18;	/* font_size height */
	/* clear text */
	__draw_sdl_rect_rgba(tetris_app->renderer, &rect, 0, 0, 0, 255);
	
	SDL_Surface * text = __create_uint_text(tetris_app->lines, tetris_app->level_font, &tetris_app->color_value);
	
	__draw_text_at_position(text ,SDL_GetWindowSurface(tetris_app->window), 0, 126);
	
	SDL_FreeSurface(text); /* No use anymore, could be cached because its a label, need text cache */
}

static void __print_tetris_headline_sdl(tetris_app_t* tetris_app) {
	SDL_Surface * text = __create_text("-= Tetris =-", tetris_app->headline_font, &tetris_app->color_headline);
	
	__draw_text_at_position(text ,SDL_GetWindowSurface(tetris_app->window), (tetris_app->width - text->w) / 2, 0);
	
	SDL_FreeSurface(text); /* No use anymore, could be cached because its a label, need text cache */
}

int init_tetris_app_sdl(tetris_app_t* tetris_app) {
	tetris_app->tetris = tetris_new(TETRIS_NORMAL);
	tetris_app->timer = 0;
	tetris_app->done = 0;
	tetris_app->width = 524;
	tetris_app->height = 524;
	tetris_app->color_headline.r = 64;
	tetris_app->color_headline.g = 64;
	tetris_app->color_headline.b = 64;
	tetris_app->color_label.r = 64;
	tetris_app->color_label.g = 64;
	tetris_app->color_label.b = 64;
	tetris_app->color_value.r = 255;
	tetris_app->color_value.g = 255;
	tetris_app->color_value.b = 255;
	tetris_app->score = 0;
	tetris_app->level = 1;
	tetris_app->lines = 0;
	tetris_app->field_width_px = 240;
	tetris_app->field_height_px = 480;
	tetris_app->next_stone = TETRIS_NO_STONE;
	tetris_app->sfx = NULL;
	
	int status = __tetris_init_sdl_screen(tetris_app);
	if ( status > 0 ) return status;
	
	status = __tetris_init_sdl_fonts(tetris_app);
	if ( status > 0 ) return status;
	
	status = __tetris_init_sdl_sounds(tetris_app);
	if ( status > 0 ) return status;
	
	/* Clear the rendering surface with the specified color */
    SDL_SetRenderDrawColor(tetris_app->renderer, 0x0, 0x0, 0x0, 0xFF);
    SDL_RenderClear(tetris_app->renderer);
	
	__print_tetris_headline_sdl(tetris_app);
	__print_tetris_score_label_sdl(tetris_app);
	__print_tetris_level_label_sdl(tetris_app);
	__print_tetris_lines_label_sdl(tetris_app);
	
	print_tetris_field_sdl(tetris_app);
	
	SDL_UpdateWindowSurface(tetris_app->window);
	
	return status;
}

static void __set_color_for_stone(stone_type_t *stone, float *r, float *g, float *b) {
	switch(*stone) {
		case TETRIS_QUAD: *r = 1.f; *g = 0.5f; *b = 0.25f; break;
		case TETRIS_LINE: *r = .25f; *g = 0.5f; *b = 1.f; break;
		case TETRIS_ROOF: *r = 1.f; *g = 0.25f; *b = 0.5f; break;
		case TETRIS_STEP_LEFT: *r = .5f; *g = 0.25f; *b = 1.f; break;
		case TETRIS_STEP_RIGHT: *r = .5f; *g = 1.f; *b = 0.25f; break;
		case TETRIS_L_LEFT: *r = .25f; *g = 1.f; *b = 0.5f; break;
		case TETRIS_L_RIGHT: *r = 1.f; *g = 0.5f; *b = 0.0f; break;
		default: *r = 0.05f; *g = 0.05f; *b = 0.05f; 
	}
}

static void _print_tetris_stone_cell(SDL_Renderer *renderer, stone_type_t *stone, unsigned int cell_row, unsigned int cell_col,
									 unsigned int x_offset, unsigned int y_offset, unsigned int block_size_w, unsigned int block_size_h){
	float r, g, b;
	static const unsigned int border_size = 1;
	
	__set_color_for_stone(stone, &r, &g, &b);
	
	unsigned int x = (cell_col * block_size_w) + x_offset;
	unsigned int y = (cell_row * block_size_h) + y_offset;
	
	__draw_rect_rgba(renderer, x + border_size, y + border_size, block_size_w - border_size, 
							  block_size_h - border_size, 
							  (unsigned char)(255.f * r), (unsigned char)(255.f * g),(unsigned char)(255.f * b), 255);

	
	__draw_rect_outline_rgba(renderer, x, y, block_size_w, block_size_h, 
					   (unsigned char)32, (unsigned char)32, (unsigned char)32, 255);
					   
}

static void __print_tetris_stone(tetris_app_t* tetris_app, stone_type_t *stone, stone_direction_t *direction,
									unsigned int x, unsigned int y, unsigned int block_size_w, unsigned int block_size_h) {

	const stone_t *cur_stone = &tetris_stones[*stone].stones[*direction];
	
	SDL_Renderer * renderer = tetris_app->renderer;
	
	for (unsigned int cell_no = 4; cell_no--; ) {
	
		stone_data_t cur_cell = cur_stone->cells[cell_no];
		
		_print_tetris_stone_cell(renderer, stone, cur_cell.row, cur_cell.col, x, y, block_size_w, block_size_h);
	
	}
}

static void __print_tetris_stone_in_game(tetris_app_t* tetris_app, unsigned int field_offset_x, unsigned int field_offset_y,
										 unsigned int block_size_w, unsigned int block_size_h) {
	const tetris_t *tetris = tetris_app->tetris;
	const stone_position_t cur_pos = tetris->active_stone.position;

	stone_type_t cur_stone = tetris->active_stone.type;
	stone_direction_t direction = tetris->active_stone.direction;
	
	__print_tetris_stone(tetris_app, &cur_stone, &direction, (cur_pos.col * block_size_w) + field_offset_x,
									 (cur_pos.row * block_size_h) + field_offset_y, block_size_w, block_size_h);
									 
}

static void __print_tetris_next_stone_sdl(tetris_app_t* tetris_app) {
	
	SDL_Surface * text = __create_text("Next:", tetris_app->level_font, &tetris_app->color_label);
	
	__draw_text_at_position(text ,SDL_GetWindowSurface(tetris_app->window), 0, 170);
	
	SDL_FreeSurface(text); /* No use anymore, could be cached because its a label, need text cache */
	
	__draw_rect_rgba(tetris_app->renderer, 20, 200, 96, 96, 0, 0, 0, 255);
	
	stone_direction_t direction = TETRIS_NORTH;
	__print_tetris_stone(tetris_app, &tetris_app->next_stone, &direction, 20, 200, 24, 24);
}

void cleanup_tetris_app_sdl(tetris_app_t* tetris_app) {
	/* cleanup work */
	tetris_free(&tetris_app->tetris);

	TTF_Quit();
	Mix_CloseAudio();
    SDL_Quit();
}

Uint32 sdl_update_down(Uint32 interval, void *param)
{
    SDL_Event event;
    SDL_UserEvent userevent;

    /* In this example, our callback pushes an SDL_USEREVENT event
    into the queue, and causes our callback to be called again at the
    same interval: */

    userevent.type = SDL_USEREVENT;
    userevent.code = 0;
    userevent.data1 = NULL;
    userevent.data2 = NULL;

    event.type = SDL_USEREVENT;
    event.user = userevent;

    SDL_PushEvent(&event);
	tetris_app_t *app = (tetris_app_t *)param;
    return(app->update_intervall);
}

void print_tetris_field_sdl(tetris_app_t * tetris_app) {
	/* redundant for test */
	tetris_t * tetris = tetris_app->tetris;
	unsigned int cols = tetris->field_size.cols;
	unsigned int rows = tetris->field_size.rows;
	const unsigned int block_size_w = tetris_app->field_width_px / cols;
	const unsigned int block_size_h = tetris_app->field_height_px / rows;
	const unsigned int x_offset = (tetris_app->width - (cols * block_size_w)) / 2;
	const unsigned int y_offset = (tetris_app->height - (rows * block_size_h));
	/* EO redundant for test */
	
	SDL_Renderer * renderer = tetris_app->renderer;
	
	/* print field */
	for ( unsigned int row = rows ; row--; ) {
		for ( unsigned int col = cols ; col--; ) {

			unsigned int field_value = tetris->field[row * cols + col];
			
			_print_tetris_stone_cell(renderer, &field_value, row, col, x_offset, y_offset, block_size_w, block_size_h);
			
		}
	}
	
	/* print stone in game */
	if ( tetris_was_started(tetris)) {
		__print_tetris_stone_in_game(tetris_app, x_offset, y_offset, block_size_w, block_size_h);
	}
	__print_tetris_score_sdl(tetris_app);
	__print_tetris_level_sdl(tetris_app);
	__print_tetris_lines_sdl(tetris_app);
	
	if ( tetris_app->next_stone != TETRIS_NO_STONE ) { 
		__print_tetris_next_stone_sdl(tetris_app);
	}
}