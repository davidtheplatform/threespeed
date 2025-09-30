#include "scheduler.h"

#include <list>
#include <memory>

#include "clone.h"
#include "rendering/Render.h"

bool ts::should_refresh = false;
std::list<ts::EventHandler> ts::event_handlers;
std::vector<std::shared_ptr<ts::Thread>> ts::active_threads;
ts::Sprite* ts::current_sprite;

void ts::tick_threads() {
    for (int i = 0; i < ts::active_threads.size(); i++) {
        current_sprite = ts::active_threads[i]->sprite;
        ThreadSignal result = ts::active_threads[i]->resume();
        if (result.result & thread_signal::EXIT) {
            ts::active_threads.at(i)->handler->thread.reset();
            ts::active_threads.erase(std::next(ts::active_threads.begin(), i));
            --i;
        }
        if (result.result & thread_signal::DELETE) {
            if (ts::current_sprite->is_clone) {
                for (int j = 0; j < ts::active_threads.size(); j++) {
                    if (active_threads[j]->sprite == current_sprite) {
                        ts::active_threads.erase(std::next(ts::active_threads.begin(), j));
                        if (j <= i) i--;
                    }
                }
                std::erase_if(ts::event_handlers, [](EventHandler h) { return h.sprite == ts::current_sprite; });
                ts::delete_sprite(ts::current_sprite);
            }
        }
    }
}

void ts::register_handler(EventHandler h) {
    event_handlers.push_back(h);
}

void ts::start_event(Event* e) {
    for (auto& handler : ts::event_handlers) {
        if (*handler.event == *e) {
            if (handler.thread) {
                *handler.thread = handler.callback();
                handler.thread->sprite = handler.sprite;
                handler.thread->handler = &handler;
            } else {
                ts::active_threads.push_back(std::make_shared<ts::Thread>(handler.callback()));
                ts::active_threads.at(ts::active_threads.size() - 1)->sprite = handler.sprite;
                ts::active_threads.at(ts::active_threads.size() - 1)->handler = &handler;

                handler.thread = ts::active_threads.at(ts::active_threads.size() - 1);
            }
        }
    }

    delete e;
}