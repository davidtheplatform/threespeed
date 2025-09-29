#include "../Logger.h"
#include "Monitor.h"
#include "../types/List.h"
#include <SDL2/SDL2.h>
#include "../libs/SDL_gfx/SDL2_gfxPrimitives.h"
#include "../libs/SDL_FontCache/SDL_FontCache.h"
#include "Render.h"

namespace ts
{
    FC_Font *monitor_label_font;
    FC_Font *monitor_value_font;

    std::vector<VariableMonitor*> variable_monitors;
    std::vector<VariableMonitor*> list_monitors;

    int init_monitor() {
        monitor_label_font = FC_CreateFont();
        if (FC_LoadFont(monitor_label_font, renderer, "../assets/fonts/monitor.ttf", 14, FC_MakeColor(87, 94, 117, 255), TTF_STYLE_NORMAL) == 0) {
            LOG_ERROR("Failed to load monitor label font");
            return -1;
        }

        monitor_value_font = FC_CreateFont();
        if (FC_LoadFont(monitor_value_font, renderer, "../assets/fonts/monitor_value.ttf", 14, FC_MakeColor(255, 255, 255, 255), TTF_STYLE_NORMAL) == 0) {
            LOG_ERROR("Failed to load monitor value font");
            return -1;
        }

        return 0;
    }

    void render_monitor(SDL_Renderer *renderer, int x, int y, std::string name, ts::Value *v)
    {
        int name_width = FC_GetWidth(monitor_label_font, ((std::string) name).c_str());
        int content_width = FC_GetWidth(monitor_label_font, ((std::string) *v).c_str()) + 33;
        int total_width = 8 + name_width + 7 + content_width + 5;

        roundedBoxRGBA(renderer, x, y, x + total_width, y + 25, 4, 230, 240, 255, 255);       // background
        roundedRectangleRGBA(renderer, x, y, x + total_width, y + 25, 4, 196, 204, 217, 255); // border
        roundedRectangleRGBA(renderer, x, y, x + total_width, y + 25, 3, 196, 204, 217, 255); // border again

        roundedBoxRGBA(renderer, x + 18 + name_width, y + 4, x + 9 + name_width + content_width, y + 21, 4, 255, 140, 26, 255); // orange area

        FC_Draw(monitor_label_font, renderer, x + 9, y + 3, ((std::string) name).c_str());                 // name
        FC_Draw(monitor_value_font, renderer, x + 18 + name_width + 14, y + 3, ((std::string) *v).c_str()); // value
    }

    void render_monitor(SDL_Renderer *renderer, int x, int y, int scroll, std::string name, List<ts::Value> *l)
    {
        // TODO change draw order to cover elements
        // should be blue box -> elements -> white top/bottom -> white top/bottom but not rounded -> outlines
        int width = 100;
        int height = 200;
        roundedBoxRGBA(renderer, x, y, x + width, y + height, 4, 255, 255, 255, 255);       // white background
        roundedRectangleRGBA(renderer, x, y, x + width, y + height, 4, 196, 204, 217, 255); // border
        roundedRectangleRGBA(renderer, x, y, x + width, y + height, 3, 196, 204, 217, 255); // border again

        thickLineRGBA(renderer, x, y + 24, x + width, y + 24, 1, 0, 0, 0, 38);                   // draw line below name
        thickLineRGBA(renderer, x, y + height - 24, x + width, y + height - 24, 1, 0, 0, 0, 38); // draw line above length
        boxRGBA(renderer, x + 1, y + 25, x + width - 1, y + height - 25, 230, 240, 255, 255);    // blue background

        int title_width = FC_GetWidth(monitor_label_font, ((std::string) name).c_str());
        FC_Draw(monitor_label_font, renderer, x + (width / 2) - (title_width / 2), y + 3, ((std::string) name).c_str()); // name
        int length_width = FC_GetWidth(monitor_label_font, ("length: " + std::to_string(l->size())).c_str());
        FC_Draw(monitor_label_font, renderer, x + (width / 2) - (length_width / 2), y + height - 21, ("length: " + std::to_string(l->size())).c_str()); // length

        // draw items
        for (int i = 0; i < l->size(); i++)
        {
            // check if its inside the outer box
            int top_offset = scroll + (i * 28) + 27;
            if (top_offset < 0)
                continue;
            if (top_offset + 24 > height)
                break;

            roundedRectangleRGBA(renderer, x + 5, y + top_offset, x - 5 + width, y + top_offset + 24, 4, 217, 87, 22, 255);      // border
            roundedBoxRGBA(renderer, x + 5, y + top_offset, x - 5 + width, y + top_offset + 24, 4, 217, 87, 22, 255);            // border
            roundedBoxRGBA(renderer, x + 6, y + top_offset + 1, x + width - 6, y + top_offset + 23, 4, 255, 102, 26, 255);       // inner area
            roundedRectangleRGBA(renderer, x + 6, y + top_offset + 1, x + width - 6, y + top_offset + 23, 4, 255, 102, 26, 255); // inner area again

            int text_width = FC_GetWidth(monitor_value_font, ((std::string) (l->at(i))).c_str());
            FC_Draw(monitor_value_font, renderer, x + 10, y + top_offset + 2, ((std::string) (l->at(i))).c_str());
        }
    }
};