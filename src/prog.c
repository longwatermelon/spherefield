#include "prog.h"
#include "util.h"


struct Prog *prog_alloc(SDL_Window *w, SDL_Renderer *r)
{
    struct Prog *p = malloc(sizeof(struct Prog));
    p->running = true;

    p->dead = false;
    p->window = w;
    p->rend = r;

    p->font = TTF_OpenFont("res/font.ttf", 16);

    p->cam = (Vec3f){ 0.f, -1.f, 0.f };

    p->spheres = 0;
    p->nspheres = 0;

    p->nlights = 2;
    p->lights = malloc(sizeof(struct Light*) * p->nlights);
    p->lights[0] = light_alloc((Vec3f){ 1.6f, -1.8f, -1.5f }, .8f);
    p->lights[1] = light_alloc((Vec3f){ -1.5f -3.4f -3.8f }, .8f);

    p->nmats = 7;
    p->mats = malloc(sizeof(struct Material*) * p->nmats);
    p->mats[0] = mat_alloc((Vec3f){ .9f, .8f, .9f }, 50.f, 1.f, 1.f);
    p->mats[1] = mat_alloc((Vec3f){ .9f, 1.f, .9f }, 2.f, .5f, .2f);
    p->mats[2] = mat_alloc((Vec3f){ .8f, .9f, 1.f }, 50.f, 1.f, 1.f);
    p->mats[3] = mat_alloc((Vec3f){ .9f, .7f, .7f }, 90.f, 1.f, 1.f);
    p->mats[4] = mat_alloc((Vec3f){ .8f, 1.f, .8f }, 90.f, 1.f, 1.f);
    p->mats[5] = mat_alloc((Vec3f){ 1.f, .8f, .7f }, 70.f, 1.f, 1.f);
    p->mats[6] = mat_alloc((Vec3f){ 1.f, .8f, 1.f }, 90.f, 1.f, 1.f);

    p->speed = .01f;

    p->flash = false;
    p->flash_accel = 0.f;

    return p;
}


void prog_free(struct Prog *p)
{
    for (size_t i = 0; i < p->nspheres; ++i)
        sphere_free(p->spheres[i]);

    for (size_t i = 0; i < p->nlights; ++i)
        light_free(p->lights[i]);

    for (size_t i = 0; i < p->nmats; ++i)
        mat_free(p->mats[i]);

    free(p->spheres);
    free(p->lights);
    free(p->mats);

    TTF_CloseFont(p->font);

    free(p);
}


bool prog_mainloop(struct Prog *p)
{
    SDL_Event evt;

    Uint32 start = SDL_GetTicks();
    Uint32 end = SDL_GetTicks();
    bool restart = false;

    while (p->running)
    {
        end = SDL_GetTicks();
        p->timediff = end - start;
        start = end;

        while (SDL_PollEvent(&evt))
        {
            if (evt.type == SDL_QUIT)
                p->running = false;

            if (evt.type == SDL_KEYDOWN)
            {
                if (evt.key.keysym.sym == SDLK_r && p->dead)
                {
                    p->running = false;
                    restart = true;
                }

                if (!p->dead)
                {
                    if (evt.key.keysym.sym == SDLK_f)
                    {
                        p->flash = !p->flash;
                    }
                }
            }
        }

        if (!p->dead)
        {
            const Uint8* keys = SDL_GetKeyboardState(0);

            if (keys[SDL_SCANCODE_LEFT]) p->cam.x -= .003f * p->timediff;
            if (keys[SDL_SCANCODE_RIGHT]) p->cam.x += .003f * p->timediff;

            p->cam.x = fmin(fmax(p->cam.x, -3.f), 3.f);
        }

        if (p->nspheres < 10 && rand() % 100 < 3)
        {
            p->spheres = realloc(p->spheres, sizeof(struct Sphere*) * ++p->nspheres);
            p->spheres[p->nspheres - 1] = sphere_alloc((Vec3f){ (float)(rand() % 60 - 30) / 10.f, 0.f, 20.f }, .5f, p->mats[rand() % p->nmats]);
        }

        for (size_t i = 0; i < p->nspheres; ++i)
        {
            if (p->spheres[i]->c.z < p->cam.z)
            {
                p->spheres[i]->c.x = (float)(rand() % 100 - 50) / 10.f;
                p->spheres[i]->c.z = 20.f;
            }

            p->spheres[i]->c.z -= p->speed * p->timediff;
        }

        if (!p->dead)
            p->speed += .00001f;

        p->cam.y += 1.f;
        p->cam.z += 2.f;
        for (size_t i = 0; i < p->nspheres; ++i)
        {
            if (vec_len(vec_sub(p->cam, p->spheres[i]->c)) < .5f)
            {
                p->dead = true;
            }
        }
        p->cam.y -= 1.f;
        p->cam.z -= 2.f;

        static float flash_dir = .01f;

        if (p->flash)
        {
            if (p->flash_accel < -.2f || p->flash_accel > .1f)
                flash_dir *= -1.f;

            p->flash_accel += flash_dir;
            
            for (size_t i = 0; i < p->nlights; ++i)
            {
                p->lights[i]->in += p->flash_accel;
                p->lights[i]->in = fmax(p->lights[i]->in, 0.f);
            }
        }
        else
        {
            flash_dir = .01f;
        }

        SDL_RenderClear(p->rend);

        prog_render(p);

        if (p->dead)
        {
            p->speed = .01f;
            SDL_SetRenderDrawBlendMode(p->rend, SDL_BLENDMODE_BLEND);
            SDL_SetRenderDrawColor(p->rend, 0, 0, 0, 150);
            SDL_RenderFillRect(p->rend, 0);

            SDL_Texture *tex = util_render_text(p->rend, p->font, "Press [r] to restart", (SDL_Color){ 255, 255, 255 });
            SDL_Rect r;
            SDL_QueryTexture(tex, 0, 0, &r.w, &r.h);
            r.x = 200 - (r.w / 2);
            r.y = 200 - (r.h / 2);

            SDL_RenderCopy(p->rend, tex, 0, &r);
            SDL_DestroyTexture(tex);
        }

        SDL_Vertex v[3];
        v[0].color = (SDL_Color){ 255, 255, 255, 255 };
        v[1].color = (SDL_Color){ 255, 255, 255, 255 };
        v[2].color = (SDL_Color){ 255, 255, 255, 255 };
        v[0].position = (SDL_FPoint){ 195.f, 400.f };
        v[1].position = (SDL_FPoint){ 205.f, 400.f };
        v[2].position = (SDL_FPoint){ 200.f, 380.f };

        SDL_RenderGeometry(p->rend, 0, v, 3, 0, 0);

        SDL_SetRenderDrawColor(p->rend, 0, 0, 0, 255);
        SDL_RenderPresent(p->rend);
    }

    return restart;
}


void prog_render(struct Prog *p)
{
    float fov = 1.f;

    for (int y = 200; y < 400; ++y)
    {
        for (int x = 0; x < 400; ++x)
        {
            float ha = ((float)x / 400.f) * fov - (fov / 2.f);
            float va = ((float)y / 400.f) * fov - (fov / 2.f);

            Vec3f dir = vec_normalize((Vec3f){ ha, va, 1.f });

            Vec3f col = prog_render_cast_ray(p, p->cam, dir);
            SDL_SetRenderDrawColor(p->rend, col.x * 255.f, col.y * 255.f, col.z * 255.f, 255);
            SDL_RenderDrawPoint(p->rend, x, y);
        }
    }
}


Vec3f prog_render_cast_ray(struct Prog *p, Vec3f o, Vec3f dir)
{
    Vec3f hit, norm;
    struct Material *mat;

    if (!prog_scene_cast_ray(p, o, dir, &hit, &norm, &mat))
        return (Vec3f){ 0.f, 0.f, 0.f };

    float dlight = 0.f;
    float slight = 0.f;

    for (size_t i = 0; i < p->nlights; ++i)
    {
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


bool prog_scene_cast_ray(struct Prog *p, Vec3f o, Vec3f dir, Vec3f *hit, Vec3f *norm, struct Material **mat)
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

