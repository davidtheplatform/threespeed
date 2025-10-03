#include <SDL2/SDL_mixer.h>

#include "sound.h"
// #if defined(SDL_MAJOR_VERSION) && (SDL_MAJOR_VERSION == 2)
#include <map>
#include <mutex>

#include "Logger.h"

namespace ts::sound {
struct Sound {
    Mix_Chunk* chunk;
};

struct SoundHandle {
    int channel;
    bool finished;
    Sound* sound;
};

std::map<int, SoundHandle*> active_handles;
std::mutex active_handles_mutex;

void channel_finished(int channel) {
    active_handles_mutex.lock();
    if (active_handles.contains(channel)) {
        active_handles[channel]->finished = true;
    }
    active_handles_mutex.unlock();
}

int init_sound() {
    if (Mix_Init(MIX_INIT_MP3 | MIX_INIT_FLAC | MIX_INIT_WAVPACK) != MIX_INIT_MP3 | MIX_INIT_FLAC | MIX_INIT_WAVPACK) {
        LOG_WARN("SDL_Mixer not fully initialized");
        LOG_WARN(Mix_GetError());
    }

    if (Mix_OpenAudio(48000, 0, 1, 2048) == -1) {
        LOG_ERROR("Failed to open audio device");
        LOG_ERROR(Mix_GetError());
        return -1;
    }

    Mix_ChannelFinished(channel_finished);

    return 0;
}

Sound* load_sound(std::string filename) {
    Mix_Chunk* chunk = Mix_LoadWAV(filename.c_str());

    if (chunk == NULL) {
        LOG_ERROR("Failed to load " + filename + " (" + Mix_GetError() + ")");
        exit(-1);
    }

    Sound* s = new Sound();
    s->chunk = chunk;

    return s;
}

SoundHandle* start_sound(Sound* sound, double volume) {
    int channel = Mix_PlayChannel(-1, sound->chunk, 0);
    if (channel == -1) {
        Mix_AllocateChannels(Mix_AllocateChannels(-1) + 1);
        channel = Mix_PlayChannel(-1, sound->chunk, 0);
        if (channel == -1) LOG_ERROR("Newly allocated channel is already in use (this should never happen)");
    }
    Mix_Volume(channel, (int)(MIX_MAX_VOLUME * volume));

    SoundHandle* h = new SoundHandle();
    h->channel = channel;
    h->finished = false;
    h->sound = sound;

    active_handles_mutex.lock();
    active_handles[channel] = h;
    active_handles_mutex.unlock();

    return h;
}

void stop_sound(SoundHandle* handle) {
    if (handle->finished) return;

    Mix_HaltChannel(handle->channel);
    handle->finished = true;
}

void set_volume(SoundHandle* handle, double volume) {
    if (handle->finished) return;

    Mix_Volume(handle->channel, (int)(MIX_MAX_VOLUME * volume));
}

void stop_all() {
    for (auto sound : active_handles) {
        stop_sound(sound.second);
    }
}

std::string getError() {
    return Mix_GetError();
}

};  // namespace ts

// #endif