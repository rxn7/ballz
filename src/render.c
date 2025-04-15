#include "render.h"
#include "colors.h"

#include <math.h>
#include <stdlib.h>

static void create_ball_texture(struct RenderContext *ctx);

void render_context_init(struct RenderContext *ctx, SDL_Renderer *renderer) {
	ctx->renderer = renderer;
	ctx->ball_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, BALL_RADIUS * 2, BALL_RADIUS * 2);

	create_ball_texture(ctx);
}

void render_context_free(struct RenderContext *ctx) {
	SDL_DestroyTexture(ctx->ball_texture);
}

void render_ball(struct RenderContext *ctx, const struct Ball *ball, bool is_hovered) {
    if(is_hovered) {
        float radius = BALL_RADIUS * 1.2f;
        const SDL_FRect dest = { ball->x - radius, ball->y - radius, radius * 2, radius * 2 };

        SDL_SetTextureColorMod(ctx->ball_texture, 255, 255, 255);
        SDL_RenderTexture(ctx->renderer, ctx->ball_texture, NULL, &dest);
    }

	const SDL_FRect dest = { ball->x - BALL_RADIUS, ball->y - BALL_RADIUS, BALL_DIAMETER, BALL_DIAMETER };
	const SDL_Color color = color_palette[ball->color_idx];

	SDL_SetTextureColorMod(ctx->ball_texture, color.r, color.g, color.b);
	SDL_RenderTexture(ctx->renderer, ctx->ball_texture, NULL, &dest);
}

static void create_ball_texture(struct RenderContext *ctx) {
	SDL_Surface *surface = SDL_CreateSurface(BALL_DIAMETER, BALL_DIAMETER, SDL_PIXELFORMAT_INDEX8);

	SDL_Palette *palette = SDL_CreatePalette(2);
	palette->colors = (SDL_Color[]) {
		{ 0x00, 0x00, 0x00, 0x00 },
		{ 0xff, 0xff, 0xff, 0xff },
	};
	SDL_SetSurfacePalette(surface, palette);

	for(int32_t y = 0; y < BALL_DIAMETER; ++y) {
		for(int32_t x = 0; x < BALL_DIAMETER; ++x) {
			const size_t idx = y * surface->w + x;
			uint8_t *pixels = (uint8_t *)surface->pixels;

			const float dx = x - BALL_RADIUS;
			const float dy = y - BALL_RADIUS;
			const float distance_sqr = dx * dx + dy * dy;
			pixels[idx] = distance_sqr < BALL_RADIUS * BALL_RADIUS;
		}
	}

	ctx->ball_texture = SDL_CreateTextureFromSurface(ctx->renderer, surface);
	SDL_DestroySurface(surface);

	SDL_SetTextureBlendMode(ctx->ball_texture, SDL_BLENDMODE_BLEND);
	SDL_SetTextureScaleMode(ctx->ball_texture, SDL_SCALEMODE_NEAREST);
}
