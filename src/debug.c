#include "debug.h"	
#include "ball.h"

#include <stdio.h>

struct DebugData *debug_get_next_data(struct Debug *debug) {
	return &debug->data[!debug->data_idx];
}

struct DebugData *debug_get_current_data(struct Debug *debug) {
	return &debug->data[debug->data_idx];
}

void debug_update(struct Debug *debug) {
	++debug->frame_count;
	
	if(timer_elapsed(&debug->timer) >= 1000.0f) {
		timer_restart(&debug->timer);

		struct DebugData *data = debug_get_next_data(debug);
		data->fps = debug->frame_count;

		debug->frame_count = 0;
		debug->data_idx = !debug->data_idx;
	}
}

void debug_render(struct Debug *debug, SDL_Renderer *renderer) {
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	char buffer[64];

	struct DebugData *data = debug_get_current_data(debug);

	snprintf(buffer, sizeof(buffer), "FPS:    %d", data->fps);
	SDL_RenderDebugText(renderer, 0, 0, buffer);

	snprintf(buffer, sizeof(buffer), "Render: %gms", data->render_time);
	SDL_RenderDebugText(renderer, 0, 10, buffer);

	snprintf(buffer, sizeof(buffer), "Sim:    %gms", data->simulate_time);
	SDL_RenderDebugText(renderer, 0, 20, buffer);

	snprintf(buffer, sizeof(buffer), "Balls:  %d (%lu bytes)", data->balls_count, sizeof(struct Ball) * data->balls_capacity);
	SDL_RenderDebugText(renderer, 0, 30, buffer);
}
