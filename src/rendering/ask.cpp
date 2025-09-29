#include <SDL2/SDL2.h>
#include "../libs/SDL_FontCache/SDL_FontCache.h"
#include "ask.h"
#include "Render.h"

namespace ts {
    FC_Font *ask_box_font;
    
    std::string current_input;
    std::string question;
    bool show_question = false;
    std::string response;

    void set_question(std::string q) {
        current_input = "";
        question = q;
        show_question = true;
        SDL_StartTextInput();
    }

    void confirm_answer() {
        response = current_input;
        current_input = "";
        show_question = false;
        SDL_StopTextInput();
    }

    std::string get_response() {
        return response;
    }

    int init_ask() {
        ask_box_font = FC_CreateFont();
        if (FC_LoadFont(ask_box_font, renderer, "../assets/fonts/monitor.ttf", 14, FC_MakeColor(128, 0, 0, 255), TTF_STYLE_NORMAL) == 0) {
            LOG_ERROR("Failed to load ask box font");
            return -1;
        }

        return 0;
    }

    void render_ask() {
        if (show_question) {
            SDL_Rect* r = new SDL_Rect;
            r->x = 10;
            r->y = 300;
            r->w = 460;
            r-> h = 50;
            SDL_RenderDrawRect(ts::renderer, r);
        
            FC_Draw(ask_box_font, ts::renderer, 15, 305, "question");
            FC_Draw(ask_box_font, ts::renderer, 15, 330, current_input.c_str());
            
            delete r;
        }
    }
}