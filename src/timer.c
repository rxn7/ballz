#include "timer.h"

#include <stdio.h>

#include <SDL3/SDL_timer.h>

void timer_start(struct Timer *timer) {
	timer->start = SDL_GetPerformanceCounter();
}

float timer_elapsed(struct Timer *timer) {
	return (float)(SDL_GetPerformanceCounter() - timer->start) / SDL_GetPerformanceFrequency() * 1000.0f;
}

float timer_restart(struct Timer *timer) {
	const float elapsed_milliseconds = timer_elapsed(timer);
	timer_start(timer);
	return elapsed_milliseconds;
}

void timer_print_elapsed(struct Timer *timer, const char *name) {
	const float elapsed = timer_elapsed(timer);
	printf("%s: %fms\n", name, elapsed);
}
