#include "world.h"
#include "cell.h"
#include "colors.h"
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

    list_init(&world->balls, balls_count * 2);

	for(uint32_t i = 0; i < CELL_COUNT * CELL_COUNT; ++i) {
		cell_init(&world->cells[i]);
	}

	for(uint32_t i = 0; i < balls_count; ++i) {
		world_add_ball(world, rand() % (game->logical_width - BALL_RADIUS) + BALL_RADIUS, rand() % (game->logical_height - BALL_RADIUS) + BALL_RADIUS);
	}

	world_print_memory_usage(world);
}

void world_free(struct World *world) {
	for(uint32_t i = 0; i < world->balls.count; ++i) {
        void *data = world->balls.data[i];
        free(data);
    }

    list_free(&world->balls);
}

void world_add_ball(struct World *world, float x, float y) {
    struct Ball *ball = (struct Ball *)malloc(sizeof(struct Ball));
    ball_init(ball, x, y);
    list_insert(&world->balls, ball);
}

void world_remove_hovered_ball(struct World *world) {
    if(world->hovered_ball == NULL) {
        return;
    }

    list_remove(&world->balls, world->hovered_ball);
    list_remove(&world->hovered_cell->balls, world->hovered_ball);
}

void world_simulate(struct World *world, float dt) {
	struct Timer simulate_timer;
	timer_start(&simulate_timer);

	for(uint32_t i = 0; i < CELL_COUNT * CELL_COUNT; ++i) {
		list_clear(&world->cells[i].balls);
	}

	for(uint32_t i = 0; i < world->balls.count; ++i) {
		struct Ball *ball = (struct Ball *)world->balls.data[i];
		struct CellList cells_list = world_get_ball_cells(world, ball);

		for(uint32_t j = 0; j < cells_list.size; ++j) {
			struct Cell *cell = cells_list.cells[j];
			cell_insert(cell, ball);
		}

		free(cells_list.cells);
	}

	for(uint32_t i = 0; i < world->balls.count; ++i) {
		struct Ball *a = (struct Ball *)world->balls.data[i];
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

	for(uint32_t i = 0; i < world->balls.count; ++i) {
		struct Ball *ball = (struct Ball *)world->balls.data[i];
		world_update_ball(world, ball, dt);
	}

	if(!world->game->debug.enabled) {
		return;
	}

	{
		struct DebugData *debug_data = debug_get_current_data(&world->game->debug);
		debug_data->balls_count = world->balls.count;
		debug_data->balls_capacity = world->balls.capacity;
	}
	{
		struct DebugData *debug_data = debug_get_next_data(&world->game->debug);
		debug_data->simulate_time = timer_elapsed(&simulate_timer);
	}
}

void world_update_ball(struct World *world, struct Ball *ball, float dt) {
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

void world_render(struct World *world) {
	struct Timer render_timer;
	timer_start(&render_timer);

	for(uint32_t i = 0; i < world->balls.count; ++i) {
		const struct Ball *ball = (const struct Ball *)world->balls.data[i];
        const bool is_hovered = ball == world->hovered_ball;
		render_ball(&world->game->render_ctx, ball, is_hovered);
	}

	if(!world->game->debug.enabled) {
		return;
	}

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
				snprintf(text, sizeof(text), "%u", cell->balls.count);

				SDL_SetRenderDrawColor(world->game->renderer, 255, 0, 255, 255);
				SDL_RenderDebugText(world->game->renderer, rect.x, rect.y, text);

				for(uint32_t i = 0; i < cell->balls.count; ++i) {
					const struct Ball *ball = (struct Ball *)cell->balls.data[i];
					const SDL_Color color = color_palette[ball->color_idx];

					SDL_SetRenderDrawColor(world->game->renderer, color.r, color.g, color.b, 255);
					SDL_RenderLine(world->game->renderer, x * CELL_SIZE, y * CELL_SIZE, ball->x, ball->y);
				}
			}
		}
	}

	struct DebugData *debug_data = debug_get_next_data(&world->game->debug);
	debug_data->render_time = timer_elapsed(&render_timer);
}

struct CellList world_get_ball_cells(struct World *world, const struct Ball *ball) {
	struct CellCoords center = cell_coords_from_position(ball->x, ball->y);

	struct CellCoords neighbours[8];
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

void world_update_hovered_ball(struct World *world) {
    const float x = world->game->mouse_x;
    const float y = world->game->mouse_y;

	const struct CellCoords coords = cell_coords_from_position(x, y);
	world->hovered_cell = world_get_cell(world, coords);

	for(uint32_t i = 0; i < world->hovered_cell->balls.count; ++i) {
		struct Ball *ball = (struct Ball *)world->hovered_cell->balls.data[i];
		const float dx = ball->x - x;
		const float dy = ball->y - y;
		const float distance_sqr = dx * dx + dy * dy;

		if(distance_sqr <= BALL_RADIUS * BALL_RADIUS) {
            world->hovered_ball = ball;
            return;
		}
	}

    world->hovered_ball = NULL;
}

struct Cell *world_get_cell(struct World *world, struct CellCoords coords) {
	return &world->cells[cell_idx(coords)];
}

void world_print_memory_usage(struct World *world) {
	printf("world size: %u balls, %u capacity (%lu bytes)\n", world->balls.count, world->balls.capacity, world->balls.capacity * sizeof(struct Ball));
}
