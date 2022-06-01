#ifndef CORE_SPHERE_H
#define CORE_SPHERE_H

#include "vector.h"
#include <stdbool.h>
#include <SDL2/SDL.h>

struct Material
{
    Vec3f col;
    float specular_exp;

    float ref_diffuse, ref_specular;
};

struct Material *mat_alloc(Vec3f col, float specular_exp, float rd, float rs);
void mat_free(struct Material *m);

struct Sphere
{
    Vec3f c;
    float r;

    struct Material *mat;
};

struct Sphere *sphere_alloc(Vec3f c, float r, struct Material *mat);
void sphere_free(struct Sphere *s);

bool sphere_ray_intersect(struct Sphere *s, Vec3f ro, Vec3f rdir, float *t);

#endif

