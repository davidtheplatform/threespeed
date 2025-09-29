#pragma once

#include <string>

namespace ts
{
    class Value;
    class String;
    class Boolean;

    /**
     * A scratch number/double
     */
    class Number
    {
    public:
        double value;

        Number(double&& n);
        Number(double& n);

        operator const double() const;
        operator int() const;
        operator bool() const;
        operator std::string() const;
        operator ts::Value() const;
        operator ts::String() const;
        operator ts::Boolean() const;
    };
};