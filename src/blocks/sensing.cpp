#include <SDL2/SDL2.h>
#include <chrono>
#include "sensing.h"
#include "../input.h"
#include "../rendering/ask.h"

namespace ts
{
    extern Uint64 timerStartTime;
};

namespace ts::block
{
    bool sensing_keypressed(std::string key)
    {
        SDL_Keycode k = SDL_GetKeyFromName(key.c_str());
        if (k == SDLK_UNKNOWN)
            return false;
        return ts::key_states[k];
    }

    ts::Boolean sensing_mousedown() {
        return ts::mouse_state;
    }

    ts::Number sensing_mousex() {
        return ts::mouse_x;
    }
    ts::Number sensing_mousey() {
        return ts::mouse_y;
    }

    double sensing_timer()
    {
        return (double)(SDL_GetTicks64() - timerStartTime) / 1000.0;
    }
    void sensing_resettimer()
    {
        timerStartTime = SDL_GetTicks64();
    }

    // may not match scratch exactly due differences in how leap seconds work
    double sensing_dayssince2000()
    {
        std::chrono::microseconds us = std::chrono::duration_cast<std::chrono::microseconds>(
            std::chrono::system_clock::now().time_since_epoch());

        std::tm y2k = {
            0,
            0,
            0,
            0,
            0,
            100};
        y2k.tm_isdst = -1;
        std::chrono::microseconds y2k_us = std::chrono::duration_cast<std::chrono::microseconds>(
            std::chrono::system_clock::from_time_t(std::mktime(&y2k)).time_since_epoch());

        return ((double)(us - y2k_us).count() / 1000000) / 86400;
    }

    ts::String sensing_touchingobjectmenu(ts::String input) {
        return input;
    }

    void sensing_askandwait(ts::String question) {
        ts::set_question(question);
    }
    ts::String sensing_answer() {
        return ts::get_response();
    }

    ts::String sensing_username() {
        return std::string("username");
    }
};