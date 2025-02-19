#include "world.h"
#include "game.h"
#include "ball.h"

#include <stdio.h>
#include <stdlib.h>

void world_init(struct World *world, struct Game *game, uint32_t balls_capacity) {
	world->game = game;
	world->balls = malloc(sizeof(struct Ball) * balls_capacity);
	world->balls_count = 0;

	world->balls_capacity = balls_capacity;
	world->balls_rects = malloc(sizeof(SDL_FRect) * balls_capacity);

	world_print_memory_usage(world);
}

void world_destroy(struct World *world) {
	free(world->balls);
	free(world->balls_rects);
}

void world_add_ball(struct World *world, const struct Ball *ball) {
	if(world->balls_count >= world->balls_capacity) {
		world->balls_capacity *= 2; // TODO: benchmark if this is optimal
		world->balls = realloc(world->balls, sizeof(struct Ball) * world->balls_capacity);
		world->balls_rects = realloc(world->balls_rects, sizeof(SDL_FRect) * world->balls_capacity);

#ifndef NDEBUG
		world_print_memory_usage(world);
#endif
	}

	world->balls[world->balls_count++] = *ball;
}

void world_simulate(struct World *world) {
	for(uint32_t i = 0; i < world->balls_count; ++i) {
		struct Ball *ball = &world->balls[i];
		ball->x += ball->vx;
		ball->y += ball->vy;

		bool x_collided = false;
		if(ball->x - BALL_RADIUS <= 0.0f) {
			x_collided = true;
			ball->x = BALL_RADIUS + 0.01f; // TODO: Define epsilon
		} else if(ball->x + BALL_RADIUS > world->game->logical_width) {
			x_collided = true;
			ball->x = world->game->logical_width - BALL_RADIUS - 0.01f; // TODO: Define epsilon
		}

		bool y_collided = false;
		if(ball->y - BALL_RADIUS <= 0.0f) {
			y_collided = true;
			ball->y = BALL_RADIUS + 0.01f; // TODO: Define epsilon
		} else if(ball->y + BALL_RADIUS > world->game->logical_height) {
			y_collided = true;
			ball->y = world->game->logical_height - BALL_RADIUS - 0.01f; // TODO: Define epsilon
		}

		if(x_collided) {
			ball->vx *= -1.0f;
		}
		if(y_collided) {
			ball->vy *= -1.0f;
		}
	}
}

void world_render(struct World *world, SDL_Renderer *renderer) {
	for(uint32_t i = 0; i < world->balls_count; ++i) {
		const struct Ball *ball = &world->balls[i];
		world->balls_rects[i] = (SDL_FRect) {
			.x = ball->x - BALL_RADIUS,
			.y = ball->y - BALL_RADIUS,
			.w = BALL_RADIUS * 2,
			.h = BALL_RADIUS * 2
		};
	}

	SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
	SDL_RenderFillRects(renderer, world->balls_rects, world->balls_count);
}

void world_print_memory_usage(struct World *world) {
	printf("world size: %u balls, %u capacity (%lu bytes)\n", world->balls_count, world->balls_capacity, world->balls_capacity * sizeof(struct Ball));
}
