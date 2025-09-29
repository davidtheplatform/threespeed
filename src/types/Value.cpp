#include "Value.h"

#include "String.h"
#include "Number.h"

#include "cast.h"

#include <algorithm>

#include <assert.h>
#include <cmath>

std::string ts::Value::get() {
    return operator std::string();
}

double ts::Value::get_d() {
    return operator double();
}

ts::Value::operator std::string() const {
    switch (type) {
        case ts::ScratchType::STRING:
        {
            return value_s;
        }
        case ts::ScratchType::NUMBER:
        {
            return ts::cast::d2s(value_d);
        }
        case ts::ScratchType::BOOLEAN:
        {
            return ts::cast::b2s(value_b);
        }
    }
    
    assert(false); // unreachable
}

ts::Value::operator double() const {
    switch (type) {
        case ts::ScratchType::STRING:
        {
            return ts::cast::s2d(value_s);
        }
        case ts::ScratchType::NUMBER:
        {
            return value_d;
        }
        case ts::ScratchType::BOOLEAN:
        {
            return (double) value_b;
        }
    }
    
    assert(false); // unreachable
}

ts::Value::operator int() const {
    switch (type) {
        case ts::ScratchType::STRING:
        {
            return ts::cast::s2d(value_s);
        }
        case ts::ScratchType::NUMBER:
        {
            return value_d;
        }
        case ts::ScratchType::BOOLEAN:
        {
            return (int) value_b;
        }
    }
    
    assert(false); // unreachable
}

ts::Value::operator bool() const {
    switch (type) {
        case ts::ScratchType::STRING:
        {
            return ts::cast::s2b(value_s);
        }
        case ts::ScratchType::NUMBER:
        {
            return (bool) value_d; // TODO: is this correct?
        }
        case ts::ScratchType::BOOLEAN:
        {
            return value_b;
        }
    }
    
    assert(false); // unreachable
}

ts::Value::operator ts::Number() const {
    return (double) *this;
}

ts::Value::operator ts::String() const {
    return (std::string) *this;
}

bool ts::Value::operator==(const ts::Value& other) const {
    double n1 = (double) *this;
    double n2 = (double) other;

    // TODO check if n1 or n2 are whitespace

    if (std::isnan(n1) || std::isnan(n1)) {
        // TODO ignore case
        return ((std::string) *this) == ((std::string) other);
    }

    return n1 == n2;
}

bool ts::Value::operator==(const String& other) const {
    return (std::string) *this == (std::string) other;
}