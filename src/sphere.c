#include "sphere.h"
#include <math.h>
#include <stdlib.h>


struct Material *mat_alloc(Vec3f col, float specular_exp, float rd, float rs)
{
    struct Material *m = malloc(sizeof(struct Material));
    m->col = col;
    m->specular_exp = specular_exp;

    m->ref_diffuse = rd;
    m->ref_specular = rs;

    return m;
}


void mat_free(struct Material *m)
{
    free(m);
}


struct Sphere *sphere_alloc(Vec3f c, float r, struct Material *mat)
{
    struct Sphere *s = malloc(sizeof(struct Sphere));
    s->c = c;
    s->r = r;

    s->mat = mat;

    return s;
}


void sphere_free(struct Sphere *s)
{
    free(s);
}


bool sphere_ray_intersect(struct Sphere *s, Vec3f ro, Vec3f rdir, float *t)
{
    Vec3f co = vec_sub(ro, s->c);

    float a = vec_dot(rdir, rdir);
    float b = vec_dot(co, rdir) * 2.f;
    float c = vec_dot(co, co) - s->r * s->r;

    float discrim = b * b - 4.f * a * c;

    if (discrim < 0)
        return false;

    float sqrt = sqrtf(discrim);
    float t1 = (-b + sqrt) / (2.f * a);
    float t2 = (-b - sqrt) / (2.f * a);

    // account for floating point errors
    if (t1 <= 1e-4f)
        return false;

    if (t2 <= 1e-4f)
        *t = t1;
    else
        *t = (t1 < t2 ? t1 : t2);

    return true;
}

