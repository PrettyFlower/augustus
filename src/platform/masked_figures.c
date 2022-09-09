#include "masked_figures.h"

static struct {
	SDL_Texture *figures_texture;
	SDL_Texture *buildings_texture;
	SDL_Texture *masking_texture;
} data;

static void destroy_textures()
{
	SDL_DestroyTexture(data.figures_texture);
	data.figures_texture = 0;
	SDL_DestroyTexture(data.buildings_texture);
	data.buildings_texture = 0;
	SDL_DestroyTexture(data.masking_texture);
	data.masking_texture = 0;
}

void masked_figures_init(SDL_Renderer *renderer, int width, int height)
{
	destroy_textures();
	data.figures_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_TARGET, width, height);
	data.buildings_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_TARGET, width, height);
	data.masking_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_TARGET, width, height);
}

static SDL_Texture *get_texture(render_texture render_texture, SDL_Texture *default_texture)
{
	switch (render_texture)
	{
		case TEXTURE_FIGURES:
			return data.figures_texture;
		case TEXTURE_BUILDINGS:
			return data.buildings_texture;
		case TEXTURE_MASKING:
			return data.masking_texture;
		default:
			return default_texture;
	}
}

void masked_figures_change_target_texture(SDL_Renderer *renderer, SDL_Texture *default_texture, render_texture render_texture)
{
	SDL_Texture *texture = get_texture(render_texture, default_texture);
	SDL_SetRenderTarget(renderer, texture);
}

void masked_figures_change_blend_mode(SDL_Renderer *renderer, SDL_Texture *default_texture, render_texture render_texture, blend_mode mode)
{
	SDL_Texture *texture = get_texture(render_texture, default_texture);
	if (mode == BLEND_MASK) {
		mode = SDL_ComposeCustomBlendMode(SDL_BLENDFACTOR_ZERO, SDL_BLENDFACTOR_DST_COLOR, SDL_BLENDOPERATION_ADD, SDL_BLENDFACTOR_DST_ALPHA, SDL_BLENDFACTOR_SRC_ALPHA, SDL_BLENDOPERATION_MINIMUM);
	}
	int success = SDL_SetTextureBlendMode(texture, mode);
	if (success == -1)
	{
		// idk something bad
		mode = -1;
	}
}

void masked_figures_draw_target_texture(SDL_Renderer *renderer, SDL_Texture *default_texture, render_texture render_texture)
{
	SDL_Texture *texture = get_texture(render_texture, default_texture);
	SDL_RenderCopy(renderer, texture, NULL, NULL);
}