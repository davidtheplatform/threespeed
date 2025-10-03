#include "sound.h"
#include "SDL2/sound.h"
#include "threespeed.h"

namespace ts::block {
    void sound_stopallsounds() {
        ts::sound::stop_all();
    }

    std::string sound_sounds_menu(std::string s) {
        return s;
    }

    void sound_changevolumeby(ts::Number v) {
        ts::current_sprite->volume += (double) v;
        for (auto handle : ts::current_sprite->active_sounds) {
            ts::sound::set_volume(handle, ts::current_sprite->volume);
        }
    }

    void sound_setvolumeto(ts::Number v) {
        ts::current_sprite->volume = (double) v;
        for (auto handle : ts::current_sprite->active_sounds) {
            ts::sound::set_volume(handle, ts::current_sprite->volume);
        }
    }

    void sound_play(std::string sound) {
        if (ts::current_sprite->sound_names.contains(sound)) {
            auto s = ts::current_sprite->sound_names[sound];
            auto handle = ts::sound::start_sound(s, ts::current_sprite->volume / 100.0);
            ts::current_sprite->active_sounds.emplace_back(handle);
        }
    }

    ts::Number sound_volume() {
        return ts::current_sprite->volume;
    }
};