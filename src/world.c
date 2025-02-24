#include "world.h"
#include "SDL3/SDL_assert.h"
#include "cell.h"
#include "debug.h"
#include "game.h"
#include "ball.h"
#include "timer.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

const float COLLISION_MARGIN = 0.01f;

void world_init(struct World *world, struct Game *game, uint32_t balls_count) {
	world->game = game;

	world->balls_capacity = balls_count * 2;
	world->balls_count = 0;
	world->balls = (struct Ball *)malloc(sizeof(struct Ball) * world->balls_capacity);

	for(uint32_t i = 0; i < CELL_COUNT * CELL_COUNT; ++i) {
			cell_init(&world->cells[i]);
	}

	for(uint32_t i = 0; i < balls_count; ++i) {
		struct Ball ball;
		ball_init(&ball, rand() % game->logical_width, rand() % game->logical_height);

		world_add_ball(&game->world, &ball);
	}

	world_print_memory_usage(world);
}

void world_free(struct World *world) {
	free(world->balls);
}

void world_add_ball(struct World *world, const struct Ball *ball) {
	if(world->balls_count >= world->balls_capacity) {
		world->balls_capacity *= 2;
		world->balls = realloc(world->balls, sizeof(struct Ball) * world->balls_capacity);
	}

	world->balls[world->balls_count++] = *ball;
}

void world_simulate(struct World *world, float dt) {
	struct Timer simulate_timer;
	timer_start(&simulate_timer);

	for(uint32_t i = 0; i < world->balls_count; ++i) {
		struct Ball *a = (struct Ball *)&world->balls[i];
		const struct Cell *cell = world_get_cell(world, (cell_coords_from_position(a->x, a->y)));

		const uint32_t balls_count = cell->balls.count;

		for(uint32_t k = 0; k < balls_count; ++k) {
			struct Ball *b = (struct Ball *)cell->balls.data[k];
			if(a == b) {
				continue;
			}

			const float delta_x = a->x - b->x;
			const float delta_y = a->y - b->y;

			const float distance_sqr = delta_x * delta_x + delta_y * delta_y;

			if(distance_sqr > BALL_DIAMETER * BALL_DIAMETER) {
				continue;
			}

			if(distance_sqr < COLLISION_MARGIN * COLLISION_MARGIN) {
				continue;
			}

			const float distance = SDL_max(sqrtf(distance_sqr), BALL_RADIUS);
			const float overlap = (distance - BALL_DIAMETER) * 0.5f / distance;
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
		struct Ball *ball = (struct Ball *)&world->balls[i];
		world_update_ball(world, ball, dt);
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

void world_update_ball(struct World *world, struct Ball *ball, float dt) {
	struct CellCoords old_coords = cell_coords_from_position(ball->x, ball->y);

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

	struct CellCoords new_coords = cell_coords_from_position(ball->x, ball->y);
	if(cell_coords_are_equal(&old_coords, &new_coords)) {
		return;
	}

	struct CellCoords old_cell_coords[9], new_cell_coords[9];
	uint8_t old_cells_count = cell_coords_get_neighbours(old_coords, old_cell_coords);
	uint8_t new_cells_count = cell_coords_get_neighbours(new_coords, new_cell_coords);

	SDL_assert_always(old_cells_count < 9);
	SDL_assert_always(new_cells_count < 9);

	old_cell_coords[old_cells_count++] = old_coords;
	new_cell_coords[new_cells_count++] = new_coords;

	for(uint8_t i = 0; i < old_cells_count; ++i) {
		bool found = false;
		for(uint8_t j = 0; j < new_cells_count; ++j) {
			if(cell_coords_are_equal(&old_cell_coords[i], &new_cell_coords[j])) {
				found = true;
				break;
			}
		}
		// If the old cell was not found in the new cells, remove the ball from the old cell
		if(!found) {
			struct CellCoords c = old_cell_coords[i];
			cell_remove(world_get_cell(world, c), ball);
		}
	}

	for(uint8_t i = 0; i < new_cells_count; ++i) {
		bool found = false;
		for(uint8_t j = 0; j < old_cells_count; ++j) {
			if(cell_coords_are_equal(&new_cell_coords[i], &old_cell_coords[j])) {
				found = true;
				break;
			}
		}
		// If the new cell was not found in the old cells, insert the ball into the new cell
		if(!found) {
			struct CellCoords c = new_cell_coords[i];
			cell_insert(world_get_cell(world, c), ball);
		}
	}
}

void world_render(struct World *world) {
	struct Timer render_timer;
	timer_start(&render_timer);

	for(uint32_t i = 0; i < world->balls_count; ++i) {
		const struct Ball *ball = (void *)&world->balls[i];
		render_ball(&world->game->render_ctx, ball);
	}

#ifdef RENDER_CELLS
	for(uint8_t x = 0; x < CELL_COUNT; ++x) {
		for(uint8_t y = 0; y < CELL_COUNT; ++y) {
			const struct Cell *cell = world_get_cell(world, (struct CellCoords) { x, y });
			const SDL_FRect rect = (SDL_FRect) {
				.x = x * CELL_SIZE,
				.y = y * CELL_SIZE,
				.w = CELL_SIZE,
				.h = CELL_SIZE
			};

			if(cell->balls.count > 0)
				SDL_SetRenderDrawColor(world->game->renderer, 0, 255, 0, 255);
			else
				SDL_SetRenderDrawColor(world->game->renderer, 255, 0, 0, 255);

			SDL_RenderRect(world->game->renderer, &rect);

			if(cell->balls.count > 0) {
				char text[6];
				sprintf(text, "%u", cell->balls.count);

				SDL_SetRenderDrawColor(world->game->renderer, 255, 0, 255, 255);
				SDL_RenderDebugText(world->game->renderer, rect.x, rect.y, text);
			}
		}
	}
#endif

	struct DebugData *debug_data = debug_get_next_data(&world->game->debug);
	debug_data->render_time = timer_elapsed(&render_timer);
}

struct CellList world_get_ball_cells(struct World *world, const struct Ball *ball) {
	struct CellCoords center = cell_coords_from_position(ball->x, ball->y);

	struct CellCoords neighbours[4];
	uint8_t neighbour_count = cell_coords_get_neighbours(center, neighbours);

	struct Cell **cells = malloc(sizeof(struct Cell *) * (neighbour_count + 1));

	for(uint8_t i = 0; i < neighbour_count; ++i) {
		struct CellCoords c = neighbours[i];
		cells[i] = world_get_cell(world, c);
	}

	cells[neighbour_count] = world_get_cell(world, center);

	return (struct CellList) {
		.cells = cells,
		.size = neighbour_count + 1
	};
}

struct Cell *world_get_cell(struct World *world, struct CellCoords coords) {
	return &world->cells[cell_idx(coords)];
}

void world_print_memory_usage(struct World *world) {
	printf("world size: %u balls, %u capacity (%lu bytes)\n", world->balls_count, world->balls_capacity, world->balls_capacity * sizeof(struct Ball));
}
