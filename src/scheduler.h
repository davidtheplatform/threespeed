/**
 * Thread scheduler to control when compiled scratch code runs
 */

#pragma once

#include <coroutine>
#include <functional>
#include "Events.h"
#include <memory>
#include <list>

namespace ts
{
    namespace thread_signal {
        constexpr uint_fast32_t EXIT = 0x00000001;
        constexpr uint_fast32_t YIELD = 0x00000002;
        constexpr uint_fast32_t DELETE = 0x00000008;
    };
    struct ThreadSignal {
        uint_fast32_t result;
    };

    struct EventHandler;

    struct Thread
    {
        struct promise_type;
        using handle_type = std::coroutine_handle<promise_type>;

        struct promise_type
        {
            bool has_finished = false;
            ThreadSignal signal;
            promise_type() = default;
            Thread get_return_object() { return std::coroutine_handle<promise_type>::from_promise(*this); }
            std::suspend_always initial_suspend() { return {}; }
            std::suspend_always final_suspend() noexcept { return {}; }
            std::suspend_always yield_value(ThreadSignal s) noexcept {
                signal = s;
                return {};
            }
            void unhandled_exception() {}
            void return_value(ThreadSignal s) {
                signal = s;
                has_finished = true;
                return;
            }
        };

        handle_type h_;

        ts::Sprite *sprite;
        EventHandler *handler; // handler that spawned this thread

        Thread(handle_type h) : h_(h) {}
        Thread(const Thread&) = delete; // non-copyable
        Thread& operator=(const Thread& other) = delete;
        Thread(Thread&& other) noexcept : h_(other.h_) {
            other.h_ = nullptr;
        }
        Thread& operator=(Thread&& other) noexcept {
            // TODO: should this do something?
            return other;
        }
        ~Thread() { if (h_) h_.destroy(); }

        ThreadSignal resume()
        {
            if ((bool) h_ && !h_.promise().has_finished) {
                h_();
                return h_.promise().signal;
            }
            return ThreadSignal {
                thread_signal::EXIT
            };
        }
    };

    extern std::list<EventHandler> event_handlers;
    extern std::vector<std::shared_ptr<ts::Thread>> active_threads;

    extern bool should_refresh;

    extern Sprite* current_sprite;

    void tick_threads();

    void register_handler(EventHandler h);

    /**
     * Start all handlers that listen for @param e
     */
    void start_event(Event *e);
};

/**
 * Call a custom block in a way that preserves its yield points
 */
#define CUSTOM_CALL(func) {\
    ts::Thread handle = func; \
    while (true) { \
        ts::ThreadSignal signal = handle.resume(); \
        /* printf("signal %p: %lx\n", handle.h_.address(), signal.result); */ \
        if (signal.result & ts::thread_signal::EXIT) break; \
        co_yield {ts::thread_signal::YIELD}; \
    } \
}
