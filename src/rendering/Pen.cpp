#include <SDL2/SDL2.h>
#include "../libs/SDL_gfx/SDL2_gfxPrimitives.h"
#include "Pen.h"
#include "../utils.h"
#include <algorithm>
#include <vector>
#include <mutex>

namespace ts
{
    SDL_Renderer* pen_renderer;
    SDL_Surface* pen_surface;

    std::vector<pen_action> pen_queue;
    std::mutex pen_queue_m;

    int init_pen() {
        pen_surface = SDL_CreateRGBSurface(0, 480, 360, 32, 0xFF000000, 0x00FF0000, 0x0000FF00, 0x000000FF);
        pen_renderer = SDL_CreateSoftwareRenderer(pen_surface);
        return 0;
    }

    void queue_pen_action(pen_action p)
    {
        pen_queue_m.lock();
        pen_queue.push_back(p);
        pen_queue_m.unlock();
    }

    void render_pen()
    {
        pen_queue_m.lock();

        for (auto action : pen_queue)
        {
            switch (action.action)
            {
            case PenActionType::PENUP:
                action.s->pendown = false;
                break;
            case PenActionType::PENDOWN:
                action.s->pendown = true;
                break;
            case PenActionType::CLEAR:
                SDL_FillRect(pen_surface, NULL, 0);
                break;
            case PenActionType::STAMP:
            {
                SDL_Rect r;
                auto pos = ts::coord_scratch2sdl(action.s->x, action.s->y);
                r.x = pos.first;
                r.y = pos.second;
                r.w = 50; // todo fix
                r.h = 50;
                SDL_RenderCopyEx(pen_renderer, action.s->get_texture(), NULL, &r, action.s->direction, NULL, SDL_FLIP_NONE);
                break;
            }
            case PenActionType::SETCOLOR:
            {
                // this code is very broken
                uint32_t c = *(uint32_t *)action.data;
                rgb color;
                color.r = (c >> 16) & 0xFF;
                color.g = (c >> 8) & 0xFF;
                color.b = c & 0xFF;
                action.s->pen_color = rgb2hsv(color);
                action.s->pen_alpha = (c >> 24) & 0xFF;

                action.s->pen_color_packed = 0xFF000000 | (c & 0x00FFFFFF);
                // action.s->pen_color_packed = 0xFF000000;
                break;
            }
            case PenActionType::SETSIZE:
            {
                action.s->pen_size = *((double *)action.data);
                if (action.s->pen_size < 1)
                    action.s->pen_size = 1;
                break;
            }
            case PenActionType::CHANGESIZE:
            {
                action.s->pen_size += *((double *)action.data);
                if (action.s->pen_size < 1)
                    action.s->pen_size = 1;
                break;
            }
            case PenActionType::CHANGEPARAM:
            {
                pen_param_action *values = (pen_param_action *)action.data;
                hsv &hsv = action.s->pen_color;
                if (values->p == "color")
                {
                    hsv.h += values->a;
                    hsv.h = abs((int)hsv.h % 360);
                }
                if (values->p == "saturation")
                {
                    hsv.s += values->a / 100;
                    hsv.s = std::clamp(hsv.s, 0.0, 1.0);
                }
                if (values->p == "brightness")
                {
                    hsv.v += values->a / 100;
                    hsv.v = std::clamp(hsv.v, 0.0, 1.0);
                }
                if (values->p == "transparency")
                {
                    action.s->pen_alpha -= values->a;
                }
                rgb c = hsv2rgb(hsv);
                action.s->pen_color_packed = action.s->pen_alpha << 24 | (uint8_t)(c.b * 255) << 16 | (uint8_t)(c.g * 255) << 8 | (uint8_t)(c.r * 255);
                break;
            }
            case PenActionType::SETPARAM:
            {
                pen_param_action *values = (pen_param_action *)action.data;
                hsv &hsv = action.s->pen_color;
                if (values->p == "color")
                {
                    hsv.h = values->a;
                    hsv.h = abs((int)hsv.h % 360);
                }
                if (values->p == "saturation")
                {
                    hsv.s = values->a / 100;
                    hsv.s = std::clamp(hsv.s, 0.0, 1.0);
                }
                if (values->p == "brightness")
                {
                    hsv.v = values->a / 100;
                    hsv.v = std::clamp(hsv.v, 0.0, 1.0);
                }
                if (values->p == "transparency")
                {
                    action.s->pen_alpha = values->a;
                }
                rgb c = hsv2rgb(hsv);
                action.s->pen_color_packed = action.s->pen_alpha << 24 | (uint8_t)(c.b * 255) << 16 | (uint8_t)(c.g * 255) << 8 | (uint8_t)(c.r * 255);
                break;
            }
            case PenActionType::DRAWLINE:
                if (action.s->pendown)
                {
                    line_t data = *((line_t *)action.data);
                    auto p1 = ts::coord_scratch2sdl(data.x1, data.y1);
                    auto p2 = ts::coord_scratch2sdl(data.x2, data.y2);

                    thickLineColor(pen_renderer, p1.first, p1.second, p2.first, p2.second, action.s->pen_size, action.s->pen_color_packed);
                    filledCircleColor(pen_renderer, p1.first, p1.second, data.width / 2, action.s->pen_color_packed);
                    filledCircleColor(pen_renderer, p2.first, p2.second, data.width / 2, action.s->pen_color_packed);
                    break;
                }
            }
        }

        pen_queue.clear();
        pen_queue_m.unlock();

        SDL_Texture *pen_texture = SDL_CreateTextureFromSurface(ts::renderer, pen_surface);
        if (!pen_texture) {
            LOG_ERROR(std::string(SDL_GetError()));
        }
        // TODO software vs hardware renderer
        if (SDL_RenderCopy(ts::renderer, pen_texture, NULL, NULL)) {
            LOG_ERROR(std::string(SDL_GetError()));
        }
        SDL_DestroyTexture(pen_texture);                   // TODO keep it around
    }

    void draw_line(Sprite* s, int x1, int y1, int x2, int y2) {
        queue_pen_action({
            PenActionType::DRAWLINE,
            s,
            (void*) new line_t{x1, y1, x2, y2, s->pen_size, s->pen_color_packed}
        });
    }
};