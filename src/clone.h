#pragma once

#include "Sprite.h"
#include "scheduler.h"
#include "rendering/Render.h"

namespace ts
{
    template <typename T>
    Sprite *create_clone(Sprite* original) {
        Sprite* clone = new Sprite(*original);
        int handler_len = ts::event_handlers.size();
        for (auto& handler : ts::event_handlers) {
        // for (int i = 0; i < handler_len; i++) {
            // auto handler = ts::event_handlers.at(i);
            if (handler.sprite == original) {
                ts::event_handlers.push_back({handler.event, clone, handler.callback, nullptr});
            }
        }

        clone->personal_variables = new T(*(T*) original->personal_variables);

        clone->is_clone = true;

        ts::register_sprite(clone);

        printf("there are now %d sprites and %d handlers\n", ts::sprites.size(), ts::event_handlers.size());
        return clone;
    }


    template <typename T>
    void delete_personal_vars(Sprite *clone) {
        if (ts::current_sprite->is_clone) {
            delete (T*) clone->personal_variables;
        }
    }
};