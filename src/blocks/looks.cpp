#include "threespeed.h"
#include "looks.h"
#include "scheduler.h"
#include "../types/String.h"
#include "../types/Value.h"
#include "../types/Number.h"

namespace ts::block {
    void looks_show() {
        ts::current_sprite->shown = true;
    }
    void looks_hide() {
        ts::current_sprite->shown = false;
    }

    void looks_setsizeto(double size) {
        ts::current_sprite->size = size; // TODO check limits
    }

    ts::Number looks_size() {
        return ts::current_sprite->size;
    }

    void looks_switchcostumeto(ts::String costume) {
        ts::current_sprite->current_costume = ts::current_sprite->costume_numbers.at(costume);
    }

    ts::String looks_costume(ts::String costume) {
        return costume;
    }

    void looks_switchbackdropto(ts::String backdrop) {
        ts::stage->current_costume = ts::stage->costume_numbers.at(backdrop);
    }
    ts::String looks_backdrops(ts::String backdrop) {
        return backdrop;
    }

    ts::Value looks_costumenumbername(ts::String option) {
        if ((std::string) option == std::string("name")) {
            return ts::current_sprite->costume_names_v.at(ts::current_sprite->current_costume);
        } else if ((std::string) option == std::string("number")) {
            return ts::current_sprite->current_costume + 1;
        } else {
            LOG_WARN("invalid option for looks_costumenumbername (" + std::string(option) + ")");
            return 0;
        }
    }

    void looks_seteffectto(ts::Number value, ts::String effect) {
        LOG_WARN(ts::current_sprite->name + ": looks_seteffectto(" + std::string(value) + ", " + std::string(effect) + ")");
    }
};