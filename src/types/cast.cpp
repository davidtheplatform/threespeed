#include "cast.h"
#include <algorithm>

namespace ts::cast {
    double s2d(const std::string &s) {
        const char* input = s.c_str();
        char* end;
        double val = strtod(input, &end);
        if (end == input || *end != '\0') return 0;
        return val;
    }

    bool s2b(const std::string &s) {
        if (s.size() > 5)
            return true;
        if ((s.size() == 0) ||
            (s == "0"))
        {
            return false;
        }
        if (s.size() < 5)
            return true;
        std::string l = s;
        std::transform(l.begin(), l.end(), l.begin(), [](unsigned char c){ return std::tolower(c); });
        return !(l == "false");
    }

    std::string d2s(const double &d) {
        return std::to_string(d);
    }

    std::string b2s(const bool &b) {
        if (b) return "true";
        return "false";
    }
}