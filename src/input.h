/**
 * Input handling
 */

#include <unordered_map>
#include <SDL2/SDL2.h>
#include "Events.h"

namespace ts {
    extern std::unordered_map<SDL_Keycode, bool> key_states;
    extern std::string current_input;

    class CloseInput : public std::exception {};

    extern int mouse_x;
    extern int mouse_y;
    extern bool mouse_state;

    void handle_input();
};