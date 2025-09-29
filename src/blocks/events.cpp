#include "events.h"
#include "scheduler.h"
#include <string>

void ts::block::event_broadcast(std::string broadcast) {
    Event *e = new Event(EventType::event_whenbroadcastreceived, broadcast);
    ts::start_event(e);
}

ts::Thread ts::block::event_broadcastandwait(std::string broadcast) {
    
}