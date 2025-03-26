#include "cell.h"

#include <SDL3/SDL_assert.h>
#include <SDL3/SDL_stdinc.h>

void cell_init(struct Cell *cell) {
	list_init(&cell->balls, 16);
}

void cell_free(struct Cell *cell) {
	list_free(&cell->balls);
}

void cell_insert(struct Cell *cell, const struct Ball *ball) {
	list_insert(&cell->balls, (void *)ball);
}

void cell_remove(struct Cell *cell, const struct Ball *ball) {
	list_remove(&cell->balls, (void *)ball);
}

struct CellCoords cell_coords_from_position(float x, float y) {
	int32_t cell_x = (int32_t)x / CELL_SIZE;
	int32_t cell_y = (int32_t)y / CELL_SIZE;

	return (struct CellCoords) {
		.x = SDL_max(0, SDL_min(CELL_COUNT - 1, cell_x)),
		.y = SDL_max(0, SDL_min(CELL_COUNT - 1, cell_y)),
	};
}

uint16_t cell_idx(struct CellCoords coords) {
	SDL_assert_always(coords.x < CELL_COUNT && coords.y < CELL_COUNT);
	return coords.y * CELL_COUNT + coords.x;
}

bool cell_coords_are_equal(const struct CellCoords *a, const struct CellCoords *b) {
	return a->x == b->x && a->y == b->y;
}

// Returns the number of neighbouring cells
uint8_t cell_coords_get_neighbours(const struct CellCoords center, struct CellCoords coords[8]) {
	uint8_t count = 0;

	if(center.x != 0) {
		coords[count++] = (struct CellCoords) { center.x - 1, center.y };
	}

	if(center.x < CELL_COUNT - 1) {
		coords[count++] = (struct CellCoords) { center.x + 1, center.y };
	}

	if(center.y != 0) {
		coords[count++] = (struct CellCoords) { center.x, center.y - 1 };

		if(center.x < CELL_COUNT - 1) {
			coords[count++] = (struct CellCoords) { center.x + 1, center.y - 1 };
		}

		if(center.x != 0) {
			coords[count++] = (struct CellCoords) { center.x - 1, center.y - 1 };
		}
	}

	if(center.y < CELL_COUNT - 1) {
		coords[count++] = (struct CellCoords) { center.x, center.y + 1 };

		if(center.x < CELL_COUNT - 1) {
			coords[count++] = (struct CellCoords) { center.x + 1, center.y + 1 };
		}

		if(center.x != 0) {
			coords[count++] = (struct CellCoords) { center.x - 1, center.y + 1 };
		}
	}

	return count;
}
