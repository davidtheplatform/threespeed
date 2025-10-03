#include "Render.h"
#include "Pen.h"
#include "Monitor.h"
#include "../Logger.h"
#include "threespeed.h"

namespace ts {
    SDL_Window *window;
    SDL_Renderer *renderer;

    std::vector<Sprite*> sprites;
    std::vector<Sprite*> layers;

    int init_renderer() {
        if (SDL_CreateWindowAndRenderer(480, 360, 0, &window, &renderer) != 0) {
            LOG_ERROR("Window/renderer creation failed: " + std::string(SDL_GetError()));
            exit(-1);
        }
        if (window == nullptr) {
            LOG_ERROR("Window creation failed: " + std::string(SDL_GetError()));
            exit(-1);
        }
        if (renderer == nullptr) {
            LOG_ERROR("Renderer creation failed: " + std::string(SDL_GetError()));
            exit(-1);
        }

        if (init_monitor() != 0) {
            LOG_ERROR("Initialize monitor renderer failed");
            exit(-1);
        }

        if (init_pen() != 0) {
            LOG_ERROR("Initialize pen renderer failed");
            exit(-1);
        }

        if (init_ask() != 0) {
            LOG_ERROR("Initialize ask renderer failed");
            exit(-1);
        }

        SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "2" );

        return 0;
    }

    void register_sprite(Sprite *s) {
        sprites.push_back(s);
    }

    void delete_sprite(Sprite *s) {
        std::erase_if(sprites, [s](Sprite *c) { return s == c; });
    }

    void render_sprite(SDL_Renderer* renderer, ts::Sprite* s) {
        SDL_Rect dstRect;
        dstRect.x = s->x + 240 - s->costume_offsets[s->current_costume].first;
        dstRect.y = s->y + 180 - s->costume_offsets[s->current_costume].second; // TODO figure out how offsets work
        SDL_QueryTexture(s->get_texture(), NULL, NULL, &dstRect.w, &dstRect.h);
        dstRect.w;
        dstRect.h;
        int direction = s->direction;
        if (s->rotation_mode == "dont_rotate") direction = 0;
        int code;
        if (s->rotation_mode == "left-right") {
            if (direction >= 0 && direction < 180) {
                code = SDL_RenderCopyEx(renderer, s->get_texture(), NULL, &dstRect, 0, NULL, SDL_FLIP_NONE);
            } else {
                code = SDL_RenderCopyEx(renderer, s->get_texture(), NULL, &dstRect, 0, NULL, SDL_FLIP_HORIZONTAL);
            }
        } else {
            code = SDL_RenderCopyEx(renderer, s->get_texture(), NULL, &dstRect, s->direction, NULL, SDL_FLIP_NONE);
        }

        if (code) {
            LOG_ERROR("failed to render sprite (" + std::string(SDL_GetError()) + ")");
        }
    }

    /**
     * Render everything
     */
    void render() {
        // TODO cache things that haven't changed if possible
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
        SDL_RenderClear(renderer);
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, SDL_ALPHA_OPAQUE);

        render_sprite(renderer, ts::stage);

        ts::render_pen();        

        // TODO track layer order so this isn't O(n^2)
        for (int layer = 0; layer < sprites.size(); layer++)
        {
            // break;
            for (auto s : sprites)
            {
                if (s->shown && s->layer == layer && s->name != "Stage")
                {
                    render_sprite(renderer, s);
                }
            }
        }
            
        for (auto monitor : variable_monitors) {
            if (monitor->shown) ts::render_monitor(ts::renderer, monitor->x, monitor->y, monitor->name, monitor->value);
        }

        render_ask();

        SDL_RenderPresent(renderer);
    }

    SDL_Renderer* get_renderer() {
        return renderer;
    }
};