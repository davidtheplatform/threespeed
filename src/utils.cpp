#include "utils.h"

#include <math.h>

#include <charconv>
#include <assert.h>

#include "types/ScratchTypes.h"

namespace ts {

std::pair<int, int> coord_scratch2sdl(int x, int y) {
    return std::pair(x + 240, -y + 180);
}

std::pair<int, int> coord_sdl2scratch(int x, int y) {
    return std::pair(x - 240, -y + 180);
}

bool isWhitespace(std::string str) {
    return str.find_first_not_of(" \t\n\v\f\r") == std::string::npos;
}

std::string to_string(int val) { return std::to_string(val); }
std::string to_string(double val) { return std::to_string(val); }

double to_double(double val) { return val; }
double to_double(ts::Number val) { return val.value; }
double to_double(ts::String val) { return to_double(val.value); }
double to_double(ts::Value& val) {
    if (val.type == ScratchType::NUMBER) return val.value_d;
    if (val.type == ScratchType::BOOLEAN) return val.value_b;
    if (val.type == ScratchType::STRING) return to_double(val.value_s);
    assert(false); // unreachable
}
double to_double(std::string val) {
    double result;
    auto [ptr, ec] =
        std::from_chars(val.data(), val.data() + val.size(), result);
    if (ec == std::errc()) return result;
    return std::numeric_limits<double>::quiet_NaN();
}
};  // namespace ts