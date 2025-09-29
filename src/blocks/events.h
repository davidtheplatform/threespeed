#include <string>
#include "../scheduler.h"

namespace ts::block {
    void event_broadcast(std::string broadcast);
    ts::Thread event_broadcastandwait(std::string broadcast);
};