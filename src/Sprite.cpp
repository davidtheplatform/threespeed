#include "Sprite.h"

namespace ts
{
    void Sprite::load_sound(std::string filename, std::string soundName)
    {
        Mix_Chunk *chunk = Mix_LoadWAV(filename.c_str());

        if (chunk == NULL)
        {
            LOG_ERROR("Failed to load " + filename + " (" + Mix_GetError() + ")");
            exit(-1);
        }

        sounds.emplace_back(chunk);
        sound_names.emplace(soundName, chunk);
        sound_numbers.emplace(soundName, sounds.size() - 1);

        LOG_INFO("Loaded sound " + soundName + " from " + filename);
    }

    void Sprite::load_costume(std::string filename, std::string costumeName, int x_offset, int y_offset)
    {
        SDL_Texture *texture = IMG_LoadTexture(renderer, filename.c_str());

        if (texture == NULL)
        {
            if (std::string(SDL_GetError()) == "Couldn't parse SVG image")
            {
                LOG_WARN("Invalid SVG image: " + filename);
                SDL_DestroyTexture(texture);
                texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB32, SDL_TEXTUREACCESS_STATIC, 1, 1); // TODO: load the question mark costume instead
            }
            else
            {
                LOG_ERROR("Failed to load " + filename + " (" + SDL_GetError() + ")");
                exit(-1);
            }
        }

        costumes.emplace_back(texture);
        costume_names.emplace(costumeName, texture);
        costume_numbers.emplace(costumeName, costumes.size() - 1);
        costume_offsets.emplace_back(std::pair<int, int>(x_offset, y_offset));
        costume_names_v.emplace_back(costumeName);

        LOG_INFO("Loaded costume " + costumeName + " from " + filename);
    }

    /**
     * Get a texture for the current costume
     */
    SDL_Texture *Sprite::get_texture()
    {
        return costumes[current_costume];
    }
};