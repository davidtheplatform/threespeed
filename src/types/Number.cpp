#include "Number.h"

#include "Value.h"
#include "String.h"
#include "Boolean.h"

#include "cast.h"
#include "Logger.h"

#include <assert.h>
#include <cmath>


ts::Number::Number(double&& n)
{
#ifdef TS_DEBUG
    if (std::isnan(n)) {
        LOG_WARN("Number(double&&) got NaN");
    }
#endif

    value = n;
}

ts::Number::Number(double& n) {
#ifdef TS_DEBUG
    if (std::isnan(n)) {
        LOG_WARN("Number(double&) got NaN");
    }
#endif

    value = n;
}

ts::Number::operator std::string() const
{
    return ts::cast::d2s(value);
}

ts::Number::operator const double() const
{
    return value;
}

ts::Number::operator int() const {
    return value;
}

ts::Number::operator bool() const
{
    if (value) return 1;
    return 0;
}

ts::Number::operator ts::Value() const
{
    return ts::Value(value);
}

ts::Number::operator ts::String() const {
    return ts::cast::d2s(value);
}

ts::Number::operator ts::Boolean() const {
    if (value) return 1;
    return 0;
}
