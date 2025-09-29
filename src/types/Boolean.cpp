#include "Value.h"
#include "Boolean.h"
#include "Number.h"

#include "cast.h"

#include <assert.h>
#include <cmath>

ts::Boolean::Boolean(bool n)
{
    value = n;
}

ts::Boolean::operator std::string() const
{
    return ts::cast::b2s(value);
}

ts::Boolean::operator double() const
{
    assert(value == 0 || value == 1); // if this fails please file a bug report
    return value;
}

ts::Boolean::operator int() const
{
    assert(value == 0 || value == 1);
    return value;
}

ts::Boolean::operator bool() const
{
    return value;
}

ts::Boolean::operator ts::Value() const
{
    return ts::Value(value);
}

ts::Boolean::operator ts::Number() const
{
    return value;
}