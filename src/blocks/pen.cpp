#include "pen.h"
#include "../rendering/Pen.h"
#include "../scheduler.h"
#include <string>

namespace ts::block {
    void pen_clear()
    {
        ts::queue_pen_action({ts::PenActionType::CLEAR, ts::current_sprite, nullptr});
    }

    void pen_penDown()
    {
        ts::queue_pen_action({ts::PenActionType::PENDOWN, ts::current_sprite, nullptr});
    }
    void pen_penUp()
    {
        ts::queue_pen_action({ts::PenActionType::PENUP, ts::current_sprite, nullptr});
    }

    void pen_setPenSizeTo(ts::Number size)
    {
        ts::queue_pen_action({ts::PenActionType::SETSIZE, ts::current_sprite, new double(size)});
    }
    void pen_changePenSizeBy(ts::Number size) {
        ts::queue_pen_action({ts::PenActionType::CHANGESIZE, ts::current_sprite, new double(size)});
    }

    void pen_setPenColorToColor(ts::String color)
    {
        ts::queue_pen_action({ts::PenActionType::SETCOLOR, ts::current_sprite, new double(0xAA0000AA)});
    }
    void pen_setPenColorToColor(uint32_t color)
    {
        ts::queue_pen_action({ts::PenActionType::SETCOLOR, ts::current_sprite, &color});
    }
    void pen_setPenColorToColor(ts::Number color) {
        ts::queue_pen_action({ts::PenActionType::SETCOLOR, ts::current_sprite, new uint32_t((int) color)});
    }
    void pen_setPenColorToColor(ts::Value color) {
        ts::queue_pen_action({ts::PenActionType::SETCOLOR, ts::current_sprite, new uint32_t((int) color)});
    }

    void pen_setPenColorParamTo(ts::String param, ts::Number value) {
        ts::queue_pen_action({ts::PenActionType::SETPARAM, ts::current_sprite, new pen_param_action{param, value}});
    }
    void pen_changePenColorParamBy(ts::String param, ts::Number value) {
        ts::queue_pen_action({ts::PenActionType::CHANGEPARAM, ts::current_sprite, new pen_param_action{param, value}});
    }

    ts::String pen_menu_colorParam(ts::String param) {
        return param;
    }

    void pen_stamp() {
        ts::queue_pen_action({ts::PenActionType::STAMP, ts::current_sprite, nullptr});
    }
};