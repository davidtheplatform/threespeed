#include "effects.h"
#include "Render.h"
#include <cmath>
#include <algorithm>

namespace ts {
    struct __attribute__((packed)) Pixel {
        uint8_t a;
        uint8_t r;
        uint8_t g;
        uint8_t b;
    };

    void apply_brightness(Pixel& src, double power) {
        int p = power * 2.55 + 0.5;
        src.r = std::clamp((int) src.r + p, 0, 255);
        src.g = std::clamp((int) src.g + p, 0, 255);
        src.b = std::clamp((int) src.b + p, 0, 255);
    }

    SDL_Surface* apply_effects(SDL_Surface* src, Effects e) {
        int width, height;
        width = src->w;
        height = src->h;
        SDL_Surface* dest = SDL_CreateRGBSurface(0, width, height, 32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);

        for (int x = 0; x < width; x++) {
            for (int y = 0; y < height; y++) {
                uint32_t raw = *(uint32_t *)(src->pixels + y * src->pitch + x * 4);
                // TODO does endianness matter here? Pixel format should be ARGB but seems to be ABGR instead
                Pixel p = {
                    raw >> 24,
                    raw & 0xFF,
                    raw >> 8 & 0xFF,
                    raw >> 16 & 0xFF,
                };

                apply_brightness(p, e.brightness);

                raw = p.a << 24 | p.r << 16 | p.g << 8 | p.b;
                *((uint32_t *)(dest->pixels + y * dest->pitch + x * 4)) = raw;
            }
        }
        return dest;
    }
}