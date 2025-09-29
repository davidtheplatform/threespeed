#pragma once

#include <string>

namespace ts
{

    class Value;
    class Number;
    class Boolean;

    /**
     * A scratch string
     */
    class String
    {
    public:
        std::string value;

        String(std::string s) {
            value = s;
        }

        operator double() const;
        operator int() const;
        operator bool() const;
        operator std::string() const;
        operator ts::Value() const;
        operator ts::Number() const;
        operator ts::Boolean() const;
        bool operator==(const ts::String& other) const;
        bool operator==(const int other) const;
        // bool operator==(const ts::String other) const;
    };
};