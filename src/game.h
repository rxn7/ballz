#pragma once

#include <SDL3/SDL.h>

struct Game {
	SDL_Window *window;
	SDL_Renderer *renderer;

	float mouse_x, mouse_y;
	int32_t window_w, window_h;

	bool is_window_open;
};

void game_init(struct Game *const game);
void game_start(struct Game *const game);
void game_destroy(struct Game *const ctx);
void game_handle_event(struct Game *const ctx, const SDL_Event *const event);
void game_render(struct Game *const game);
void game_handle_window_resize(struct Game *const game, const SDL_Event *const event);
void game_handle_key_event(struct Game *const game, const SDL_Event *const event);
