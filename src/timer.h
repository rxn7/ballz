#pragma once

#include <stdint.h>

#include <SDL3/SDL_render.h>

struct Timer {
	uint64_t start;
};

void timer_start(struct Timer *timer);
float timer_elapsed(struct Timer *timer);
float timer_restart(struct Timer *timer);
void timer_print_elapsed(struct Timer *timer, const char *name);
