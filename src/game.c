#include "game.h"
#include <SDL3/SDL_render.h>

void game_init(struct Game *const game) {
	SDL_assert_always(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS));

	game->window = SDL_CreateWindow("Ballz", 640, 480, SDL_WINDOW_RESIZABLE);
	SDL_assert_always(game->window != nullptr);
	game->is_window_open = true;

	game->renderer = SDL_CreateRenderer(game->window, nullptr);
	SDL_assert_always(game->renderer != nullptr);
}

void game_destroy(struct Game *const ctx) {
	SDL_DestroyRenderer(ctx->renderer);
	SDL_DestroyWindow(ctx->window);
	SDL_Quit();
}

void game_start(struct Game *const game) {
	uint64_t now = SDL_GetPerformanceCounter();
	uint64_t last_frame_tick = now;
	while(game->is_window_open) {
		now = SDL_GetPerformanceCounter();
		last_frame_tick = now;

		SDL_Event event;
		while(SDL_PollEvent(&event)) {
			game_handle_event(game, &event);
		}

		game_render(game);
	}
}

void game_render(struct Game *const game) {
	SDL_SetRenderDrawColor(game->renderer, 255, 0, 0, 255);
	SDL_RenderClear(game->renderer);
	SDL_RenderPresent(game->renderer);
}

void game_handle_event(struct Game *const game, const SDL_Event *const event) {
	switch(event->type) {
	case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
	case SDL_EVENT_QUIT:
		game->is_window_open = false;
		break;

	case SDL_EVENT_WINDOW_RESIZED:
		game_handle_window_resize(game, event);
		break;
	}
}

void game_handle_window_resize(struct Game *const game, const SDL_Event *const event) {
	game->window_w = event->window.data1;
	game->window_h = event->window.data2;
}
