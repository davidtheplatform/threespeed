#pragma once

#include <vector>
#include <string>
#include <map>
#include "SDL2/SDL2.h"
#include "SDL2/SDL_image.h"
#include "SDL2/sound.h"
#include "Events.h"
#include "Logger.h"
#include "Color.h"
#include "rendering/effects.h"

namespace ts
{
    class Sprite;
    extern Sprite* stage;

    /**
     * Represents a single target.
     * This class is data-only after the project has initialized
     */
    class Sprite
    {
    public:
        std::string name;

        /// motion ///
        std::string rotation_mode;
        double direction;
        double x, y;

        /// looks ///
        SDL_Renderer *renderer;

        int layer;

        int current_costume;
        std::vector<SDL_Texture *> costumes;
        std::vector<SDL_Surface *> costumes_s;
        std::map<std::string, SDL_Texture *> costume_names;
        std::vector<std::string> costume_names_v;
        std::map<std::string, int> costume_numbers;
        std::vector<std::pair<int, int>> costume_offsets;

        std::string text = "";
        bool saying = false;
        bool thinking = false;

        bool shown;

        double size;

        Effects effects;
        // whether the sprite's effects have changed and need to be re-rendered
        bool effects_updated = false;
        SDL_Texture *after_effects;

        /// sound ///
        double volume = 100;

        std::vector<ts::sound::Sound*> sounds;
        std::map<std::string, ts::sound::Sound*> sound_names;
        std::map<std::string, int> sound_numbers;
        std::vector<ts::sound::SoundHandle*> active_sounds;

        /// data ///
        void* personal_variables;
        int personal_variables_size;

        bool is_clone = false;

        /// pen ///
        // TODO initialize these correctly
        bool pendown = true;
        hsv pen_color = {0, 1, 1};
        uint8_t pen_alpha = 255;
        uint32_t pen_color_packed = 0xFF00FF00; // 0xAABBGGRR TODO does endianness matter?
        int pen_size = 1;

        void load_sound(std::string filename, std::string soundName);

        void load_costume(std::string filename, std::string costumeName, int x_offset, int y_offset);

        SDL_Texture *get_texture();
        SDL_Surface *get_surface();

        // pretty sure this won't end up doing anything
        void update() {}

        Sprite(SDL_Renderer *r, std::string n)
        {
            renderer = r;
            name = n;

            if (name == "Stage") ts::stage = this;
        }
    };
};