#include "String.h"

#include "Value.h"
#include "Number.h"
#include "Boolean.h"
#include "cast.h"

#include <algorithm>

#include <assert.h>
#include <cmath>
#include <cstdint>

ts::String::operator std::string() const
{
    return value;
}

ts::String::operator double() const
{
    return ts::cast::s2d(this->value);
}

ts::String::operator int() const {
    return (double) *this;
}

ts::String::operator bool() const
{
    return ts::cast::s2b(this->value);
}

ts::String::operator ts::Value() const
{
    return Value(value);
}

ts::String::operator ts::Number() const {
    return (double) *this;
}

ts::String::operator ts::Boolean() const {
    return (bool) *this;
}

bool ts::String::operator==(const ts::String& other) const {
    return this->value == other.value; // TODO correct comparison
}

bool ts::String::operator==(const int other) const {
    return std::stoi(value) == other;
}