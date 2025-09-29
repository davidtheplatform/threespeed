/**
 * Monitor rendering
 */

#pragma once

#include "../types/Value.h"
#include "../types/List.h"
#include <SDL2/SDL2.h>

namespace ts
{
    struct VariableMonitor {
        int x;
        int y;
        bool shown;
        std::string name;
        ts::Value *value;
    };
    extern std::vector<VariableMonitor*> variable_monitors;

    int init_monitor();

    void render_monitor(SDL_Renderer *renderer, int x, int y, std::string name, ts::Value *v);

    template<typename T1, typename T2>
    void render_monitor(SDL_Renderer *renderer, int x, int y, int scroll, T1 name, List<T2> *l);
};