#pragma once

#define BALL_RADIUS 1.0f
#define BALL_SPEED 10
#define BALL_RENDER_SEGMENTS 32

struct Ball {
	float x, y;
	float angle_radians;
};

void ball_get_direction(struct Ball *ball, float *x, float *y);
void ball_set_direction(struct Ball *ball, float x, float y);
void ball_flip_angle_horizontally(struct Ball *ball);
void ball_flip_angle_vertically(struct Ball *ball);
