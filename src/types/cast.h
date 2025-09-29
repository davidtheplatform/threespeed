#include <string>

namespace ts::cast {
    double s2d(const std::string &s);
    bool s2b(const std::string &s);

    std::string d2s(const double &d);
    std::string b2s(const bool &b);
}