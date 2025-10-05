/**
 * Delegates rendering to individual renderers.
 */

#pragma once

#include "Pen.h"
#include "Monitor.h"
#include "ask.h"
#include <SDL2/SDL2.h>

namespace ts {
    extern SDL_Window *window;
    extern SDL_Renderer *renderer;

    extern std::vector<Sprite*> sprites;
    extern std::vector<Sprite*> layers;

    int init_renderer();

    void register_sprite(Sprite *s);
    void delete_sprite(Sprite *s);

    void render();

    SDL_Renderer* get_renderer();
};