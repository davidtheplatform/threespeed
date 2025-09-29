#pragma once

#include <string>

namespace ts
{
    class Number;
    class String;

    enum ScratchType {
        STRING,
        NUMBER,
        BOOLEAN
    };

    /**
     * Generic type, could be anything. Parent class for all other datatypes.
     * Used when somethings type can't be determined at compile time (eg. reading from a list).
     * Because of this it's more expensive (both in terms of memory and computation)
     */
    class Value
    {
        ScratchType type;
        
        std::string value_s;
        double value_d;
        bool value_b;
    public:
        Value(double v) {
            value_d = v;
            type = ScratchType::NUMBER;
        }
        Value(int v) {
            value_d = v;
            type = ScratchType::NUMBER;
        }

        Value(std::string v) {
            value_s = std::string(v);
            type = ScratchType::STRING;
        }

        Value(bool v) {
            value_b = v;
            type = ScratchType::BOOLEAN;
        }

        Value(const Value &v) {
            type = v.type;

            value_b = v.value_b;
            value_d = v.value_d;
            value_s = v.value_s;
        }

        Value() {
            value_s = "";
            type = ScratchType::STRING;
        }

        /**
         * get() and get_d() are aliases of operator std::string and operator double respectively
         */
        std::string get();
        double get_d();
        operator double() const;
        operator int() const;
        operator bool() const;
        operator std::string() const;
        operator ts::Number() const;
        operator ts::String() const;

        bool operator==(const Value& other) const;
        bool operator==(const String& other) const;

        friend double to_double(ts::Value&);
    };
};