#pragma once

#include "SDL2/SDL2.h"

namespace ts {
struct Effects {
    double color;
    double fisheye;
    double whirl;
    double pixelate;
    double mosaic;
    double brightness;
    double ghost;
};

SDL_Surface* apply_effects(SDL_Surface* src, Effects e);
};