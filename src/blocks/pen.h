#include <string>
#include "types/ScratchTypes.h"
#include <cstdint>

namespace ts::block {
    void pen_clear();
    void pen_penDown();
    void pen_penUp();

    void pen_setPenSizeTo(ts::Number size);
    void pen_changePenSizeBy(ts::Number size);

    void pen_setPenColorToColor(ts::String color);
    void pen_setPenColorToColor(uint32_t color);
    void pen_setPenColorToColor(ts::Number color);
    void pen_setPenColorToColor(ts::Value color);
    void pen_setPenColorParamTo(ts::String param, ts::Number value);
    void pen_changePenColorParamBy(ts::String param, ts::Number value);
    ts::String pen_menu_colorParam(ts::String param);

    void pen_stamp();
}