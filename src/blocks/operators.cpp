#include "operators.h"
#include <cmath>
#include "Logger.h"

#define PI 3.141592653

namespace ts::block
{
    ts::Number operator_add(ts::Number a, ts::Number b)
    {
        return (double) a + (double) b;
    }
    ts::Number operator_subtract(ts::Number a, ts::Number b)
    {
        return (double) a - (double) b;
    }
    ts::Number operator_multiply(ts::Number a, ts::Number b)
    {
        return (double) a * (double) b;
    }
    ts::Number operator_divide(ts::Number a, ts::Number b)
    {
        double r = (double) a / (double) b;
        if (std::isnan(r) || std::isinf(r)) return 0;
        return r;
    }

    ts::Number operator_lt(ts::Number a, ts::Number b) {
        return (double) a < (double) b;
    }
    ts::Number operator_gt(ts::Number a, ts::Number b) {
        return (double) a > (double) b;
    }

    ts::Boolean operator_and(ts::Boolean a, ts::Boolean b)
    {
        return a && b;
    }
    ts::Boolean operator_or(ts::Boolean a, ts::Boolean b)
    {
        return a || b;
    }
    ts::Boolean operator_not(ts::Boolean a)
    {
        return !a;
    }

    ts::Boolean operator_and(ts::Boolean a) {
        return false;
    }
    ts::Boolean operator_or(ts::Boolean a) {
        return a;
    }
    ts::Boolean operator_and() {
        return false;
    }
    ts::Boolean operator_or() {
        return false;
    }
    ts::Boolean operator_not() {
        return true;
    }

    // int operator_random(int a, int b)
    // {
    //     return rand() % abs(a - b) + a; // TODO use a better method
    // }
    ts::Number operator_random(ts::Number a, ts::Number b)
    {
        double raw = ((double)rand() / RAND_MAX) * abs((double) a - (double) b) + (double)a;
        // if ((double) ((int) a) == (double) a && (double) ((int) b) == (double) b) return round(raw);
        return raw;
    }

    ts::String operator_join(std::string a, std::string b)
    {
        return a + b;
    }
    ts::String operator_letter_of(ts::Number n, std::string s)
    {
        return std::to_string(s.at((int) n));
    }
    ts::Number operator_length(std::string s)
    {
        return s.size();
    }
    ts::Boolean operator_contains(ts::String full, ts::String sub) {
        return full.value.find(sub.value) != std::string::npos;
    }

    ts::Number operator_mod(ts::Number a, ts::Number b)
    {
        return fmod((double) a, (double) b); // TODO test if this is correct
    }
    ts::Number operator_round(ts::Number a)
    {
        return round((double) a); // TODO non integer rounding
    }
    ts::Number operator_mathop(ts::Number v, std::string op)
    {
        if (op == "abs")
        {
            return abs((double) v);
        }
        if (op == "floor")
        {
            return floor((double) v);
        }
        if (op == "ceiling")
        {
            return ceil((double) v);
        }
        if (op == "sqrt")
        {
            double r = sqrt((double) v);
            if (std::isnan(r)) return 0;
            return r;
        }
        if (op == "sin")
        {
            return sin((double) v * PI / 180);
        }
        if (op == "cos")
        {
            return cos((double) v * PI / 180);
        }
        if (op == "tan")
        {
            return tan((double) v * PI / 180);
        }
        if (op == "asin")
        {
            return asin((double) v * 180 / PI);
        }
        if (op == "acos")
        {
            return acos((double) v * 180 / PI);
        }
        if (op == "atan")
        {
            return atan((double) v * 180 / PI);
        }
        if (op == "ln")
        {
            return log((double) v);
        }
        if (op == "log")
        {
            return log10((double) v);
        }
        if (op == "e ^")
        {
            return exp((double) v);
        }
        if (op == "10 ^")
        {
            return pow(10, (double) v);
        }

        LOG_WARN("Illegal operation for operator_mathop (" + op + ")\n");
        return 0;
    }
}

#undef PI