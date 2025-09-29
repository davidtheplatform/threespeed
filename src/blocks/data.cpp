#include "data.h"

namespace ts::block {
    void data_showvariable(VariableMonitor *monitor) {
        monitor->shown = true;
    }
    void data_hidevariable(VariableMonitor *monitor) {
        monitor->shown = false;
    }
};