#include "threespeed.h"

namespace ts {
    Uint64 timerStartTime = 0;

    Sprite* stage;
};

void ts::tick() {
    // update clock

    // get inputs
    handle_input();

    tick_threads();
}

void ts::init() {
    SDL_Init(SDL_INIT_EVERYTHING);

    ts::init_renderer();
    ts::sound::init_sound();
    ts::init_listloader();

    timerStartTime = SDL_GetTicks64();
}

void ts::run() {
    try {
        uint64_t last_frame = 0;
        while (true) {
            ts::tick();

            uint64_t frame_time = (SDL_GetTicks64() - last_frame);
            // if (frame_time < 1000/30) continue;
            
            last_frame = SDL_GetTicks64();
            ts::render();
            if (frame_time > 1000.0/30) continue;
            SDL_Delay(1000.0/30 - frame_time);
        }
    } catch (CloseInput) {
        ts::quit();
    }
}

void ts::quit() {
    SDL_Quit();

    exit(0);
}