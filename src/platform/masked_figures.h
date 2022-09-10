#ifndef PLATFORM_MASKED_FIGURES_H
#define PLATFORM_MASKED_FIGURES_H

#include "graphics/renderer.h"

#include "SDL.h"

void masked_figures_init(SDL_Renderer *renderer, int width, int height);
void masked_figures_change_target_texture(SDL_Renderer *renderer, SDL_Texture *default_texture, render_texture render_texture);
void masked_figures_change_blend_mode(SDL_Texture *default_texture, render_texture render_texture, blend_mode mode);
void masked_figures_change_color_mode(SDL_Texture *default_texture, render_texture render_texture, color_t color);
void masked_figures_draw_target_texture(SDL_Renderer *renderer, SDL_Texture *default_texture, render_texture render_texture);

#endif // PLATFORM_MASKED_FIGURES_H
