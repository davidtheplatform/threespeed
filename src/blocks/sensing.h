#pragma once

#include <string>
#include "../types/ScratchTypes.h"

namespace ts::block {
    bool sensing_keypressed(std::string key);
    ts::Boolean sensing_mousedown();
    ts::Number sensing_mousex();
    ts::Number sensing_mousey();

    template<typename T>
    constexpr T sensing_keyoptions(T key) {
        return key;
    }

    void sensing_askandwait(ts::String question);
    ts::String sensing_answer();

    double sensing_timer();
    void sensing_resettimer();

    double sensing_dayssince2000();

    ts::String sensing_username();

    ts::String sensing_touchingobjectmenu(ts::String input);
}