#ifndef UTIL_H
#define UTIL_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

SDL_Texture *util_render_text(SDL_Renderer* rend, TTF_Font* font, const char* text, SDL_Color color);

#endif

