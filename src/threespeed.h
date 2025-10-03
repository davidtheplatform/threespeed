/**
 * Entry point and miscellaneous things
 */

#define SDL_MAIN_HANDLED

#pragma once

#include "Sprite.h"
#include "scheduler.h"
#include "input.h"

#include "types/ScratchTypes.h"

#include "blocks/blocks.h"

#include "rendering/Render.h"

#include "listLoader.h"
#include "SDL2/sound.h"

#include <exception>

namespace ts {
    extern Sprite* stage;

    /**
     * Initialize all subsystems
     */
    void init();

    /**
     * Tick forever, until the user closes the window
     */
    void run();

    /**
     * Run one frame
     */
    void tick();

    /**
     * Do any cleanup required before exiting the program
     * Not guaranteed to free any allocated memory or other resources
     */
    void quit();
};