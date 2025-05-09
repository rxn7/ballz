#include "game.h"
#include "colors.h"
#include "debug.h"

#include <SDL3/SDL_render.h>

void game_init(struct Game *game) {
	SDL_assert_always(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS));

	game->window = SDL_CreateWindow("Ballz", 640, 480, SDL_WINDOW_RESIZABLE);
	SDL_assert_always(game->window != NULL);
	game->is_window_open = true;

	game->logical_width = 1024;
	game->logical_height = 1024;

	game->renderer = SDL_CreateRenderer(game->window, NULL);
	SDL_assert_always(game->renderer != NULL);

	SDL_SetRenderLogicalPresentation(game->renderer, game->logical_width, game->logical_height, SDL_LOGICAL_PRESENTATION_LETTERBOX);

	color_palette_init();
	render_context_init(&game->render_ctx, game->renderer);

	game->debug.enabled = false;

	game->normal_cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_DEFAULT);
	game->hovered_cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_POINTER);
}

void game_free(struct Game *game) {
	world_free(&game->world);

	SDL_DestroyRenderer(game->renderer);
	SDL_DestroyWindow(game->window);
	SDL_Quit();
}

void game_start(struct Game *game) {
	world_init(&game->world, game, INITIAL_BALL_COUNT);

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
		world_update_hovered_ball(&game->world);
		game_update_cursor(game);

		game_render(game);
	}
}

void game_render(struct Game *game) {
	SDL_SetRenderDrawColor(game->renderer, 40, 40, 40, 255);
	SDL_RenderClear(game->renderer);

	world_render(&game->world);

	if(game->debug.enabled) {
		debug_render(&game->debug, game->renderer);
	}

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

		case SDLK_F1:
			game->debug.enabled = !game->debug.enabled;
			break;
		}
		break;

	case SDL_EVENT_MOUSE_BUTTON_DOWN:
		switch(event->button.button) {
			case SDL_BUTTON_LEFT: {
				world_add_ball(&game->world, game->mouse_x, game->mouse_y);
				break;
			}

			case SDL_BUTTON_RIGHT: {
				world_remove_hovered_ball(&game->world);
				break;
			}
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

void game_update_cursor(struct Game *game) {
	if(game->world.hovered_ball != NULL) {
		SDL_SetCursor(game->hovered_cursor);
	} else {
		SDL_SetCursor(game->normal_cursor);
	}
}
