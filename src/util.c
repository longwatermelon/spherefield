#include "util.h"


SDL_Texture *util_render_text(SDL_Renderer* rend, TTF_Font* font, const char* text, SDL_Color color)
{
    if (!text[0])
        return 0;

    SDL_Surface* surf = TTF_RenderText_Blended(font, text, color);
    SDL_Texture* tex = SDL_CreateTextureFromSurface(rend, surf);

    SDL_FreeSurface(surf);

    return tex;
}

