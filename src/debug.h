#pragma once

#include "timer.h"
#include <stdint.h>

struct DebugData {
	uint32_t fps;
	float render_time;
	float simulate_time;

	uint32_t balls_count;
	uint32_t balls_capacity;
};

struct Debug {
	bool enabled;

	struct Timer timer;
	uint64_t frame_count;

	struct DebugData data[2];
	bool data_idx;
};

struct DebugData *debug_get_next_data(struct Debug *debug);
struct DebugData *debug_get_current_data(struct Debug *debug);
void debug_update(struct Debug *debug);
void debug_render(struct Debug *debug, SDL_Renderer *renderer);
