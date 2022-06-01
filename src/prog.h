#ifndef PROG_H
#define PROG_H

#include "sphere.h"
#include "light.h"
#include <stdbool.h>
#include <SDL2/SDL.h>

struct Prog
{
    bool running;

    SDL_Window *window;
    SDL_Renderer *rend;

    struct Sphere **spheres;
    size_t nspheres;

    struct Light **lights;
    size_t nlights;
};

struct Prog *prog_alloc(SDL_Window *w, SDL_Renderer *r);
void prog_free(struct Prog *p);

void prog_mainloop(struct Prog *p);
void prog_events(struct Prog *p, SDL_Event *evt);

void prog_render(struct Prog *p);
Vec3f prog_render_cast_ray(struct Prog *p, Vec3f o, Vec3f dir);
bool render_scene_cast_ray(struct Prog *p, Vec3f o, Vec3f dir, Vec3f *hit, Vec3f *norm, struct Material **mat);

#endif

