#pragma once

#include <SDL3/SDL_render.h>
#include "ball.h"

struct RenderContext {
	SDL_Renderer *renderer;
	SDL_Texture *ball_texture;
};

void render_context_init(struct RenderContext *ctx, SDL_Renderer *renderer);
void render_context_free(struct RenderContext *ctx);
void render_ball(struct RenderContext *ctx, const struct Ball *ball);
