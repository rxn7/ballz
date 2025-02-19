#pragma once

#include <SDL3/SDL_pixels.h>

#define BALL_RADIUS 2.5f

struct Ball {
	float x, y;
	float vx, vy;
	// float ax, ay;
};
