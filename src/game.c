#include "game.h"
#include "colors.h"
#include "debug.h"

#include <stdlib.h>

#include <SDL3/SDL_render.h>

void game_init(struct Game *game) {
	SDL_assert_always(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS));

	game->window = SDL_CreateWindow("Ballz", 640, 480, SDL_WINDOW_RESIZABLE);
	SDL_assert_always(game->window != nullptr);
	game->is_window_open = true;

	game->logical_width = 500;
	game->logical_height = 500;

	game->renderer = SDL_CreateRenderer(game->window, nullptr);
	SDL_SetRenderLogicalPresentation(game->renderer, 500, 500, SDL_LOGICAL_PRESENTATION_LETTERBOX);
	SDL_assert_always(game->renderer != nullptr);

	color_palette_init();
}

void game_destroy(struct Game *game) {
	world_destroy(&game->world);

	SDL_DestroyRenderer(game->renderer);
	SDL_DestroyWindow(game->window);
	SDL_Quit();
}

void game_start(struct Game *game) {
	world_init(&game->world, game, INIT_BALL_COUNT * 2);
	for(uint32_t i = 0; i < INIT_BALL_COUNT; ++i) {
		struct Ball ball;
		ball_init(&ball, rand() % game->logical_width, rand() % game->logical_height);
		world_add_ball(&game->world, &ball);
	}

	uint64_t now = SDL_GetPerformanceCounter();
	uint64_t last_frame_tick = now;
	while(game->is_window_open) {
		SDL_Event event;
		while(SDL_PollEvent(&event)) {
			game_handle_event(game, &event);
		}

		now = SDL_GetPerformanceCounter();
		const float dt = (now - last_frame_tick) / (float)SDL_GetPerformanceFrequency();
		last_frame_tick = now;

		debug_update(&game->debug);

		world_simulate(&game->world, dt);
		game_render(game);
	}
}

void game_render(struct Game *game) {
	SDL_SetRenderDrawColor(game->renderer, 40, 40, 40, 255);
	SDL_RenderClear(game->renderer);

	world_render(&game->world, game->renderer);
	debug_render(&game->debug, game->renderer);

	SDL_RenderPresent(game->renderer);
}

void game_handle_event(struct Game *game, const SDL_Event *event) {
	switch(event->type) {
	case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
	case SDL_EVENT_QUIT:
		game->is_window_open = false;
		break;

	case SDL_EVENT_WINDOW_ENTER_FULLSCREEN:
	case SDL_EVENT_WINDOW_LEAVE_FULLSCREEN:
	case SDL_EVENT_WINDOW_RESIZED:
		game_handle_window_resize(game, event);
		break;

	case SDL_EVENT_KEY_DOWN:
		switch(event->key.key) {
		case SDLK_ESCAPE:
			game->is_window_open = false;
			break;
		}
		break;

	case SDL_EVENT_MOUSE_BUTTON_DOWN:
		switch(event->button.button) {
		case SDL_BUTTON_LEFT:
			struct Ball ball;
			ball_init(&ball, game->mouse_x, game->mouse_y);
			world_add_ball(&game->world, &ball);
			break;
		}

	case SDL_EVENT_MOUSE_MOTION:
		SDL_RenderCoordinatesFromWindow(game->renderer, event->motion.x, event->motion.y, &game->mouse_x, &game->mouse_y);
		break;
	}
}

void game_handle_window_resize(struct Game *game, const SDL_Event *event) {
	game->window_w = event->window.data1;
	game->window_h = event->window.data2;
}
