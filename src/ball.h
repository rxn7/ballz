#pragma once

#define BALL_RADIUS 2.5f
#define BALL_SPEED 40
#define BALL_RENDER_SEGMENTS 64

struct Ball {
	float x, y;
	float angle_radians;
};

void ball_get_direction(struct Ball *ball, float *x, float *y);
void ball_set_direction(struct Ball *ball, float x, float y);
void ball_flip_angle_horizontally(struct Ball *ball);
void ball_flip_angle_vertically(struct Ball *ball);
