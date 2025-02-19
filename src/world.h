#pragma once

#include "ball.h"

#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_render.h>

struct World {
	struct Game *game;

	struct Ball *balls;
	uint32_t balls_count;
	uint32_t balls_capacity;

	SDL_FPoint *balls_points;
};

void world_init(struct World *world, struct Game *game, uint32_t balls_capacity);
void world_destroy(struct World *world);
void world_add_ball(struct World *world, const struct Ball *ball);
void world_simulate(struct World *world, float dt);
void world_render(struct World *world, SDL_Renderer *renderer);
void world_print_memory_usage(struct World *world);
