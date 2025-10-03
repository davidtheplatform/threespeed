// Polyfill for SDL_Mixer. Currently supports SDL2, SDL3 is planned.

#pragma once
#include <string>

namespace ts::sound {
struct Sound;
struct SoundHandle;

int init_sound();
Sound *load_sound(std::string assetName);

SoundHandle *start_sound(Sound* sound, double volume);
void stop_sound(SoundHandle* handle);
void set_volume(SoundHandle* handle, double volume);
void stop_all();

bool is_playing(SoundHandle* handle);

std::string getError();

};  // namespace ts