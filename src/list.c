#include "list.h"

#include <stdbool.h>
#include <stdlib.h>

void list_init(struct List *array, uint32_t capacity) {
	array->data = calloc(capacity, sizeof(void *));
	array->count = 0;
	array->capacity = capacity;
}

void list_free(struct List *array) {
	free(array->data);
	array->data = NULL;
	array->count = 0;
	array->capacity = 0;
}

void list_remove(struct List *array, void *element_to_remove) {
	bool found = false;
	for(uint32_t i = 0; i < array->count; ++i) {
		if(array->data[i] == element_to_remove) {
			found = true;
		}

		if(found && i < array->count - 1) {
			array->data[i] = array->data[i + 1];
		}
	}

	if(found) {
		--array->count;
	}
}

void list_insert(struct List *array, void *data) {
	if(array->count >= array->capacity) {
		array->capacity *= 2;
		array->data = realloc(array->data, sizeof(void *) * array->capacity);
	}

	array->data[array->count++] = data;
}

void list_clear(struct List *array) {
	array->count = 0;
}
