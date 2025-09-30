#include "looks.h"
#include "scheduler.h"
#include "../types/Number.h"
#include "clone.h"

namespace ts::block {
    ts::Thread control_wait(ts::Number time) {
        if ((double) time == 0) co_return {ts::thread_signal::EXIT};
        
        long sTime = SDL_GetTicks64();
        while (SDL_GetTicks64() < sTime + ((double) time * 1000)) {
            co_yield {ts::thread_signal::YIELD};
        }

        co_return {ts::thread_signal::EXIT};
    }

    ts::String control_create_clone_of_menu(ts::String clone) {
        return clone;
    }
};