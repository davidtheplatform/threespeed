#include "input.h"
#include "scheduler.h"
#include <SDL2/SDL2.h>
#include "rendering/ask.h"

namespace ts {
    std::unordered_map<SDL_Keycode, bool> key_states;

    int mouse_x = 0;
    int mouse_y = 0;
    bool mouse_state = false;

    void handle_input() {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    throw CloseInput();
                case SDL_KEYDOWN:
                    if (!ts::show_question) {
                        key_states[event.key.keysym.sym] = true;
                        ts::start_event(new ts::Event{EventType::event_whenkeypressed, event.key.keysym.sym});
                        // TODO start hats
                    } else {
                        if (event.key.keysym.sym == SDLK_BACKSPACE) {
                            if (ts::current_input.length() > 0) {
                                ts::current_input.erase(ts::current_input.length() - 1, 1);
                            }
                        }
                        if (event.key.keysym.sym == SDLK_RETURN) {
                            ts::confirm_answer();
                        }
                    }
                    break;
                case SDL_KEYUP:
                    key_states[event.key.keysym.sym] = false;
                    break;
                case SDL_MOUSEMOTION:
                    mouse_x = event.motion.x;
                    mouse_y = event.motion.y;
                    break;
                case SDL_MOUSEBUTTONDOWN:
                case SDL_MOUSEBUTTONUP:
                    if (event.button.button == 0) mouse_state = event.button.state;
                    // TODO start hats
                    break;
                case SDL_TEXTINPUT:
                    ts::current_input += event.text.text;
                    break;
            }
        }
    }
};