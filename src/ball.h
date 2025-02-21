#pragma once

#include <stdint.h>

#define BALL_RADIUS 10.0f
#define BALL_DIAMETER (BALL_RADIUS * 2.0f)
#define BALL_SPEED 40

struct Ball {
	float x, y;
	float angle_radians;
	uint8_t color;
};

void ball_init(struct Ball *ball, float x, float y);
void ball_get_direction(struct Ball *ball, float *x, float *y);
void ball_set_direction(struct Ball *ball, float x, float y);
void ball_flip_angle_horizontally(struct Ball *ball);
void ball_flip_angle_vertically(struct Ball *ball);
