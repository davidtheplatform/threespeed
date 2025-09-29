#pragma once
#include <string>
#include "../types/ScratchTypes.h"
#include "../utils.h"
#include <cmath>
#include <algorithm>
#include <math.h>

namespace ts::block {
    ts::Number operator_add(ts::Number a, ts::Number b);
    ts::Number operator_subtract(ts::Number a, ts::Number b);
    ts::Number operator_multiply(ts::Number a, ts::Number b);
    ts::Number operator_divide(ts::Number a, ts::Number b);

    ts::Number operator_lt(ts::Number a, ts::Number b);
    ts::Number operator_gt(ts::Number a, ts::Number b);

    template<typename T1, typename T2>
    ts::Boolean operator_equals(T1 a, T2 b) {
        double n1 = ts::to_double(a);
        double n2 = ts::to_double(b);
        std::string s1 = ts::to_string(a);
        std::string s2 = ts::to_string(b);
        
        if (n1 == 0 && isWhitespace(s1)) {
            n1 = std::numeric_limits<double>::quiet_NaN();
        } else if (n2 == 0 && isWhitespace(s2)) {
            n2 = std::numeric_limits<double>::quiet_NaN();
        }
        
        if (std::isnan(n1) || std::isnan(n2)) {
            std::transform(s1.begin(), s1.end(), s1.begin(), [](unsigned char c){ return std::tolower(c); });
            std::transform(s2.begin(), s2.end(), s2.begin(), [](unsigned char c){ return std::tolower(c); });

            return s1 == s2;
        }
        // Handle the special case of Infinity
        if ((std::isinf(n1) && std::isinf(n2))) {
            return false;
        }

        return n1 == n2;
    }

    ts::Boolean operator_and(ts::Boolean a, ts::Boolean b);
    ts::Boolean operator_or(ts::Boolean a, ts::Boolean b);
    ts::Boolean operator_not(ts::Boolean a);

    ts::Boolean operator_and(ts::Boolean a);
    ts::Boolean operator_or(ts::Boolean a);
    ts::Boolean operator_and();
    ts::Boolean operator_or();
    ts::Boolean operator_not();

    // int operator_random(int a, int b);
    ts::Number operator_random(ts::Number a, ts::Number b);

    ts::String operator_join(std::string a, std::string b);
    ts::String operator_letter_of(ts::Number n, std::string s);
    ts::Number operator_length(std::string s);
    ts::Boolean operator_contains(ts::String full, ts::String sub);

    ts::Number operator_mod(ts::Number a, ts::Number b);
    ts::Number operator_round(ts::Number a);
    ts::Number operator_mathop(ts::Number v, std::string op);
};