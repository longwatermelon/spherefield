#include "prog.h"
#include <time.h>
#include <SDL2/SDL_ttf.h>


int main(int argc, char **argv)
{
    srand(time(0));

    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();
    SDL_Window *w = SDL_CreateWindow("Spherefield", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 400, 400, SDL_WINDOW_SHOWN);
    SDL_Renderer *r = SDL_CreateRenderer(w, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    while (true)
    {
        struct Prog *p = prog_alloc(w, r);
        bool restart = prog_mainloop(p);
        prog_free(p);

        if (!restart)
            break;
    }

    SDL_DestroyRenderer(r);
    SDL_DestroyWindow(w);

    TTF_Quit();
    SDL_Quit();

    return 0;
}

