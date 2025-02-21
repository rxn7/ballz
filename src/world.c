#include "world.h"
#include "colors.h"
#include "debug.h"
#include "game.h"
#include "ball.h"
#include "timer.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

void world_init(struct World *world, struct Game *game, uint32_t balls_capacity) {
	world->game = game;
	world->balls = calloc(balls_capacity, sizeof(struct Ball));
	world->balls_count = 0;

	world->balls_capacity = balls_capacity;

	world_print_memory_usage(world);
}

void world_destroy(struct World *world) {
	free(world->balls);
}

void world_add_ball(struct World *world, const struct Ball *ball) {
	if(world->balls_count >= world->balls_capacity) {
		world->balls_capacity *= 2; // TODO: benchmark if this is optimal
		world->balls = realloc(world->balls, sizeof(struct Ball) * world->balls_capacity);

#ifndef NDEBUG
		world_print_memory_usage(world);
#endif
	}

	world->balls[world->balls_count++] = *ball;
}

void world_simulate(struct World *world, float dt) {
	struct Timer simulate_timer;
	timer_start(&simulate_timer);

	const float COLLISION_MARGIN = 0.01f;

	for(uint32_t i = 0; i < world->balls_count; ++i) {
		struct Ball *a = &world->balls[i];

		// ball collisions
		for(uint32_t j = 0; j < world->balls_count; ++j) {
			if(i == j) {
				continue;
			}

			struct Ball *b = &world->balls[j];

			const float delta_x = a->x - b->x;
			const float delta_y = a->y - b->y;

			const float distance_sqr = delta_x * delta_x + delta_y * delta_y;
			if(distance_sqr > (BALL_RADIUS * 2.0) * (BALL_RADIUS * 2.0)) {
				continue;
			}

			const float distance = sqrtf(distance_sqr);
			if(distance < COLLISION_MARGIN) {
				continue;
			}

			const float overlap = (distance - BALL_RADIUS * 2.0f) * 0.5f / distance;
			const float overlap_x = overlap * delta_x;
			const float overlap_y = overlap * delta_y;

			a->x -= overlap_x;
			a->y -= overlap_y;
			b->x += overlap_x;
			b->y += overlap_y;

			const float normal_x = delta_x / distance; 
			const float normal_y = delta_y / distance; 

			float a_direction_x, a_direction_y;
			float b_direction_x, b_direction_y;
			ball_get_direction(a, &a_direction_x, &a_direction_y);
			ball_get_direction(b, &b_direction_x, &b_direction_y);

			const float dot_product_a = a_direction_x * normal_x + a_direction_y * normal_y;
			const float dot_product_b = b_direction_x * normal_x + b_direction_y * normal_y;

			const float reflected_a_direction_x = a_direction_x - 2.0f * normal_x * dot_product_a;
			const float reflected_a_direction_y = a_direction_y - 2.0f * normal_y * dot_product_a;
			const float reflected_b_direction_x = b_direction_x - 2.0f * normal_x * dot_product_b;
			const float reflected_b_direction_y = b_direction_y - 2.0f * normal_y * dot_product_b;

			ball_set_direction(a, reflected_a_direction_x, reflected_a_direction_y);
			ball_set_direction(b, reflected_b_direction_x, reflected_b_direction_y);
		}
	}

	for(uint32_t i = 0; i < world->balls_count; ++i) {
		struct Ball *ball = &world->balls[i];

		// wall collisions
		if(ball->x - BALL_RADIUS <= 0.0f) {
			ball_flip_angle_horizontally(ball);
			ball->x = BALL_RADIUS + COLLISION_MARGIN; 
		} else if(ball->x + BALL_RADIUS > world->game->logical_width) {
			ball_flip_angle_horizontally(ball);
			ball->x = world->game->logical_width - BALL_RADIUS - COLLISION_MARGIN;
		}

		if(ball->y - BALL_RADIUS <= 0.0f) {
			ball_flip_angle_vertically(ball);
			ball->y = BALL_RADIUS + COLLISION_MARGIN;
		} else if(ball->y + BALL_RADIUS > world->game->logical_height) {
			ball_flip_angle_vertically(ball);
			ball->y = world->game->logical_height - BALL_RADIUS - COLLISION_MARGIN;
		}

		float dx, dy;
		ball_get_direction(ball, &dx, &dy);

		ball->x += dx * BALL_SPEED * dt;
		ball->y += dy * BALL_SPEED * dt;
	}

	{
		struct DebugData *debug_data = debug_get_current_data(&world->game->debug);
		debug_data->balls_count = world->balls_count;
		debug_data->balls_capacity = world->balls_capacity;
	}
	{
		struct DebugData *debug_data = debug_get_next_data(&world->game->debug);
		debug_data->simulate_time = timer_elapsed(&simulate_timer);
	}
}

void world_render(struct World *world, SDL_Renderer *renderer) {
	struct Timer render_timer;
	timer_start(&render_timer);

	for(uint32_t i = 0; i < world->balls_count; ++i) {
		const struct Ball *ball = &world->balls[i];
		const int32_t radius = ceilf(BALL_RADIUS);
		const int32_t diameter = ceilf(BALL_DIAMETER);

		int32_t x = radius - 1;
		int32_t y = 0;
		int32_t tx = 1;
		int32_t ty = 1;
		int32_t error = tx - diameter;
		uint32_t draw_count = 0;

		const uint32_t point_count = diameter * 8 * 35 / 49;
		SDL_FPoint points[point_count];
		
		while(x >= y) {
			points[draw_count+0] = (SDL_FPoint) { ball->x + x, ball->y + y };
			points[draw_count+1] = (SDL_FPoint) { ball->x + x, ball->y - y };
			points[draw_count+2] = (SDL_FPoint) { ball->x - x, ball->y - y };
			points[draw_count+3] = (SDL_FPoint) { ball->x - x, ball->y + y };
			points[draw_count+4] = (SDL_FPoint) { ball->x + y, ball->y + x };
			points[draw_count+5] = (SDL_FPoint) { ball->x + y, ball->y - x };
			points[draw_count+6] = (SDL_FPoint) { ball->x - y, ball->y - x };
			points[draw_count+7] = (SDL_FPoint) { ball->x - y, ball->y + x };
			draw_count += 8;

			if(error <= 0) {
				++y;
				error += ty;
				ty += 2;
			}
			if(error > 0) {
				--x;
				tx += 2;
				error += tx - ceilf(BALL_DIAMETER);
			}
		}

		SDL_Color *color = &color_palette[ball->color];
		SDL_SetRenderDrawColor(renderer, color->r, color->g, color->b, color->a);
		SDL_RenderPoints(renderer, points, draw_count);
	}

	struct DebugData *debug_data = debug_get_next_data(&world->game->debug);
	debug_data->render_time = timer_elapsed(&render_timer);
}

void world_print_memory_usage(struct World *world) {
	printf("world size: %u balls, %u capacity (%lu bytes)\n", world->balls_count, world->balls_capacity, world->balls_capacity * sizeof(struct Ball));
}
