#pragma once

#include <stdint.h>

struct List {
	void **data;
	uint32_t count;
	uint32_t capacity;
};

void list_init(struct List *array, uint32_t capacity);
void list_free(struct List *array);
void list_remove(struct List *array, void *data);
void list_insert(struct List *array, void *data);
void list_clear(struct List *array);
