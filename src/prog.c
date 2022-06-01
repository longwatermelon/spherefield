#include "prog.h"


struct Prog *prog_alloc(SDL_Window *w, SDL_Renderer *r)
{
    struct Prog *p = malloc(sizeof(struct Prog));
    p->running = true;
    p->window = w;
    p->rend = r;

    p->spheres = 0;
    p->nspheres = 0;

    p->lights = 0;
    p->nlights = 0;

    return p;
}


void prog_free(struct Prog *p)
{
    for (size_t i = 0; i < p->nspheres; ++i)
        sphere_free(p->spheres[i]);

    for (size_t i = 0; i < p->nlights; ++i)
        light_free(p->lights[i]);

    free(p->spheres);
    free(p->lights);

    free(p);
}


void prog_mainloop(struct Prog *p)
{
    SDL_Event evt;

    p->spheres = malloc(sizeof(struct Sphere*) * 1);
    p->nspheres = 1;
    struct Material *mat = mat_alloc((Vec3f){ 1.f, 0.f, .5f }, 50, 1.f, 1.f);
    p->spheres[0] = sphere_alloc((Vec3f){ 0.f, 0.f, 5.f }, 1.f, mat);
    p->lights = malloc(sizeof(struct Light*) * ++p->nlights);
    p->lights[0] = light_alloc((Vec3f){ 0.f, 0.f, 0.f }, .8f);

    while (p->running)
    {
        prog_events(p, &evt);

        for (size_t i = 0; i < p->nspheres; ++i)
        {
            p->spheres[i]->c.z -= .1f;
        }

        SDL_RenderClear(p->rend);

        prog_render(p);

        SDL_SetRenderDrawColor(p->rend, 0, 0, 0, 255);
        SDL_RenderPresent(p->rend);
    }
}


void prog_events(struct Prog *p, SDL_Event *evt)
{
    while (SDL_PollEvent(evt))
    {
        switch (evt->type)
        {
        case SDL_QUIT:
            p->running = false;
            break;
        }
    }
}


void prog_render(struct Prog *p)
{
    float fov = 1.f;

    for (int y = 0; y < 500; ++y)
    {
        for (int x = 0; x < 500; ++x)
        {
            float ha = ((float)x / 500.f) * fov - (fov / 2.f);
            float va = ((float)y / 500.f) * fov - (fov / 2.f);

            Vec3f dir = vec_normalize((Vec3f){ ha, va, 1.f });

            Vec3f col = prog_render_cast_ray(p, (Vec3f){ 0.f, 0.f, 0.f }, dir);
            SDL_SetRenderDrawColor(p->rend, col.x * 255.f, col.y * 255.f, col.z * 255.f, 255);
            SDL_RenderDrawPoint(p->rend, x, y);
        }
    }
}


Vec3f prog_render_cast_ray(struct Prog *p, Vec3f o, Vec3f dir)
{
    Vec3f hit, norm;
    struct Material *mat;

    if (!render_scene_cast_ray(p, o, dir, &hit, &norm, &mat))
        return (Vec3f){ 0.f, 0.f, 0.f };

    float dlight = 0.f;
    float slight = 0.f;

    for (size_t i = 0; i < p->nlights; ++i)
    {
        // shadow
        Vec3f orig = vec_addv(hit, vec_divf(norm, 1e3f));
        Vec3f sdir = vec_normalize(vec_sub(p->lights[i]->pos, orig));

        Vec3f shadow_hit;
        if (render_scene_cast_ray(p, orig, sdir, &shadow_hit, 0, 0))
        {
            if (vec_len(vec_sub(shadow_hit, hit)) <= vec_len(vec_sub(p->lights[i]->pos, hit)))
                continue;
        }

        float dist = vec_len(vec_sub(p->lights[i]->pos, hit));
        float b = fmin(p->lights[i]->in / (.005f * dist * dist), p->lights[i]->in);

        // diffuse
        Vec3f l = vec_normalize(vec_sub(p->lights[i]->pos, hit));
        dlight += b * fmax(0.f, vec_dot(l, norm));

        // specular
        Vec3f r = vec_sub(l, vec_mulf(vec_mulf(norm, 2.f), vec_dot(l, norm)));
        slight += b * powf(fmax(0.f, vec_dot(r, vec_normalize(hit))), mat->specular_exp);
    }

    Vec3f col = vec_addf(vec_mulf(vec_mulf(mat->col, dlight), mat->ref_diffuse), slight * mat->ref_specular);
    col.x = fmin(col.x, 1.f);
    col.y = fmin(col.y, 1.f);
    col.z = fmin(col.z, 1.f);

    return col;
}


bool render_scene_cast_ray(struct Prog *p, Vec3f o, Vec3f dir, Vec3f *hit, Vec3f *norm, struct Material **mat)
{
    float nearest = INFINITY;

    for (size_t i = 0; i < p->nspheres; ++i)
    {
        float t;

        if (sphere_ray_intersect(p->spheres[i], o, dir, &t) && t < nearest)
        {
            nearest = t;
            Vec3f intersect = vec_addv(o, vec_mulf(dir, t));

            if (hit) *hit = intersect;
            if (norm) *norm = vec_normalize(vec_sub(intersect, p->spheres[i]->c));
            if (mat) *mat = p->spheres[i]->mat;
        }
    }

    return nearest < 1000.f;
}

