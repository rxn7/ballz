#pragma once

#include "world.h"
#include "debug.h"

#include <SDL3/SDL.h>

#define INIT_BALL_COUNT 100

struct Game {
	SDL_Window *window;
	SDL_Renderer *renderer;

	float mouse_x, mouse_y;
	int32_t window_w, window_h;
	int32_t logical_width, logical_height;

	bool is_window_open;

	struct World world;
	struct Debug debug;
};

void game_init(struct Game *game);
void game_start(struct Game *game);
void game_destroy(struct Game *game);
void game_handle_event(struct Game *ctx, const SDL_Event *event);
void game_render(struct Game *game);
void game_handle_window_resize(struct Game *game, const SDL_Event *event);
void game_handle_key_event(struct Game *game, const SDL_Event *event);
void game_render_balls(struct Game *game);
