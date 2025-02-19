#include "ball.h"

#include <math.h>

void ball_get_direction(struct Ball *ball, float *x, float *y) {
	*y = sinf(ball->angle_radians);
	*x = cosf(ball->angle_radians);
}

void ball_set_direction(struct Ball *ball, float x, float y) {
	ball->angle_radians = atan2f(y, x);
}

void ball_flip_angle_horizontally(struct Ball *ball) {
	ball->angle_radians = M_PI - ball->angle_radians;
}

void ball_flip_angle_vertically(struct Ball *ball) {
	ball->angle_radians = -ball->angle_radians;
}
