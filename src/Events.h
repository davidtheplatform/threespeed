#pragma once

#include <string>
#include <queue>
#include <SDL2/SDL2.h>
#include "Logger.h"
#include "memory"

namespace ts
{
    class Sprite;

    enum class EventType
    {
        event_whenflagclicked,
        event_whenkeypressed,
        event_whenbroadcastreceived,
        control_start_as_clone
    };

    struct Event
    {
        EventType type;
        union
        {
            SDL_Keycode key;
            std::string* broadcast;
        } data;

        Event(ts::EventType type, std::string broadcast) {
            this->type = type;
            this->data.broadcast = new std::string(broadcast);
        }

        Event(ts::EventType type, SDL_Keycode key) {
            this->type = type;
            this->data.key = key;
        }

        ~Event()
        {
            switch (type) {
                case EventType::event_whenbroadcastreceived:
                    delete data.broadcast;
                    break;
            }
        }

        bool operator==(const Event &other)
        {
            if (type != other.type)
                return false;
            switch (type)
            {
            case EventType::event_whenkeypressed:
                return data.key == other.data.key;
            case EventType::event_whenbroadcastreceived:
                return *(data.broadcast) == *(other.data.broadcast);
            case EventType::event_whenflagclicked:
                return true;
            }

            LOG_WARN("invalid event comparison");
            return false;
        }
    };

    struct Thread;

    typedef Thread (*HandlerFunc)();

    struct EventHandler
    {
        Event *event;
        Sprite *sprite;
        HandlerFunc callback;
        std::shared_ptr<ts::Thread> thread;
    };
};