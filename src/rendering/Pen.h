/**
 * Pen rendering
 */

#pragma once

#include <string>
#include "../Color.h"
#include "../Sprite.h"

namespace ts
{
    extern SDL_Renderer *renderer;
    extern Sprite *current_sprite;

    enum PenActionType
    {
        PENUP,
        PENDOWN,
        CLEAR,
        STAMP,
        SETCOLOR,
        SETSIZE,
        CHANGESIZE,
        DRAWLINE,
        CHANGEPARAM,
        SETPARAM
    };

    struct pen_action
    {
        PenActionType action;
        Sprite *s;
        void *data;
    };
    struct pen_param_action
    {
        std::string p;
        double a;
    };

    struct line_t {int x1; int y1; int x2; int y2; int width; uint32_t color;};

    int init_pen();

    void queue_pen_action(pen_action p);

    void draw_line(Sprite* s, int x1, int y1, int x2, int y2);

    void render_pen();
};