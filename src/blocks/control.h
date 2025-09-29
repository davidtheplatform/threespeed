#pragma once
#include <string>
#include "../Sprite.h"
#include "../types/ScratchTypes.h"
#include "clone.h"

namespace ts::block
{
    ts::Thread control_wait(ts::Number time);
    ts::Thread control_wait_until(ts::Boolean condition);

    ts::String control_create_clone_of_menu(ts::String clone);
    
    template <typename T>
    void control_create_clone_of(ts::String clone) {
        if (clone.value == "_myself_") {
            ts::create_clone<T>(ts::current_sprite);
        } else {
            LOG_WARN("Create clone of " + (std::string) clone + " ignored");
        }
    }
};

/**
 * The control_wait_until block. IDK how to implement this without making it a macro.
 */
#define CONTROL_WAIT_UNTIL(cond) {\
    while (cond) {\
        co_yield {ts::thread_signal::YIELD};\
    }\
}\

/**
 * The control_wait_until block, but for warp blocks.
 */
#define CONTROL_WAIT_UNTIL_WARP(cond) {\
    while (cond) {}\
}
