#include "colors.h"

SDL_Color color_palette[256];

void color_palette_init() {
	for(int i = 0; i < 256; ++i) {
		color_palette[i].r = (i * 5) % 205 + 50;
		color_palette[i].g = (i * 3) % 205 + 50;
		color_palette[i].b = (i * 7) % 205 + 50;
		color_palette[i].a = 255;
	}
}
