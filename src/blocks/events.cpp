#include "events.h"
#include "scheduler.h"
#include <string>

void ts::block::event_broadcast(std::string broadcast) {
    Event *e = new Event(EventType::event_whenbroadcastreceived, broadcast);
    ts::start_event(e);
}

ts::Thread ts::block::event_broadcastandwait(std::string broadcast) {
    // TODO this should ignore threads that were started by a different block
    Event *e = new Event(EventType::event_whenbroadcastreceived, broadcast);
    ts::start_event(e);

    co_yield {ts::thread_signal::YIELD};

    while (true) {
        bool should_yield = false;
        for (auto thread : ts::active_threads) {
            if (thread->handler->event->type != ts::EventType::event_whenbroadcastreceived) continue;
            if (*thread->handler->event->data.broadcast == broadcast) {
                should_yield = true;
                break;
            }
        }
        if (should_yield) co_yield {ts::thread_signal::YIELD};
        else break;
    }

    co_return {ts::thread_signal::EXIT};
}