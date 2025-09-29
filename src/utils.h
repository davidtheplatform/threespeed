/**
 * Various utility functions that don't belong anywhere else
 */

#pragma once

#include <utility>
#include <string>
#include <types/ScratchTypes.h>

namespace ts
{

    /**
     * Convert from scratch coordinates to screen coordinates
     * Scratch: 0,0 is centered, right and up is +x +y
     * screen: 0,0 is at the top right, right and up is +x -y
     */
    std::pair<int, int> coord_scratch2sdl(int x, int y);

    /**
     * Convert from screen coordinates to scratch coordinates
     * Scratch: 0,0 is centered, right and up is +x +y
     * screen: 0,0 is at the top right, right and up is +x -y
     */
    std::pair<int, int> coord_sdl2scratch(int x, int y);

    /**
     * lowercase a string
     */
    std::string to_lower(std::string s);

    /**
     * Compare two things with scratch casting rules
     */
    int compare(double a, double b);
    int compare(std::string a, double b);
    int compare(double a, std::string b);
    int compare(std::string a, std::string b);

    bool isWhitespace(std::string str);

    std::string to_string(int);
    std::string to_string(double);
    template<typename T>
    std::string to_string(T val) {
        return (std::string) val;
    }

    /**
     * Does *not* apply scratch casting rules
     */
    double to_double(double);
    double to_double(ts::Number);
    double to_double(ts::String);
    double to_double(ts::Boolean);
    double to_double(ts::Value&);
    double to_double(std::string);
    template<typename T>
    double to_double(T val) {
        return ((ts::Number) val).value;
    }
};