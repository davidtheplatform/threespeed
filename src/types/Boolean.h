#pragma once

#include "Value.h"

namespace ts
{

    /**
     * A scratch boolean
     */
    class Boolean
    {
        bool value;

    public:
        Boolean(bool n);

        operator double() const;
        operator int() const;
        operator bool() const;
        operator std::string() const;
        operator ts::Value() const;
        operator ts::Number() const;
    };
};