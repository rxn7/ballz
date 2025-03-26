#pragma once

#include "ball.h"
#include "cell.h"

#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_render.h>

#define RENDER_CELLS

struct CellList {
	struct Cell **cells;
	uint8_t size;
};

struct World {
	struct Game *game;
	struct Cell cells[CELL_COUNT * CELL_COUNT];

	struct Ball *balls;
	uint32_t balls_count;
	uint32_t balls_capacity;
};

void world_init(struct World *world, struct Game *game, uint32_t balls_count);
void world_free(struct World *world);
void world_add_ball(struct World *world, const struct Ball *ball);
void world_remove_ball(struct World *world, float x, float y);
void world_simulate(struct World *world, float dt);
void world_update_ball(struct World *world, struct Ball *ball, float dt);
void world_render(struct World *world);

// must free result.cells
struct CellList world_get_ball_cells(struct World *world, const struct Ball *ball);

struct Cell *world_get_cell(struct World *world, struct CellCoords coords);
void world_print_memory_usage(struct World *world);
