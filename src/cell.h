#pragma once

#include "list.h"
#include "ball.h"

#include <stdbool.h>
#include <stdint.h>

#define CELL_SIZE 8
#define CELL_COUNT (512 / CELL_SIZE)

_Static_assert(UINT8_MAX >= CELL_COUNT);

struct CellCoords {
	uint8_t x;
	uint8_t y;
};

struct Cell {
	struct List balls;
};

void cell_init(struct Cell *cell);
void cell_free(struct Cell *cell);
void cell_insert(struct Cell *cell, const struct Ball *ball);
void cell_remove(struct Cell *cell, const struct Ball *ball);

struct CellCoords cell_coords_from_position(float x, float y);
uint16_t cell_idx(struct CellCoords coords);

bool cell_coords_are_equal(const struct CellCoords *a, const struct CellCoords *b);
uint8_t cell_coords_get_neighbours(const struct CellCoords center, struct CellCoords coords[4]);
