/**
 * Representation of a scratch list.
 * This means:
 * * List indices start at 1
 * * The strings "all", "random", "last", and "any" are valid indices
 * This can't be split into a seperate cpp file due to the use of templates.
 */

#pragma once
#include <vector>
#include <string>
#include "String.h"
#include <math.h>

namespace ts
{
    template <class T>
    class List
    {
        std::vector<T> contents;

    public:
        List(std::initializer_list<T> contents) {
            this->contents = contents;
        }

        List() {
            this->contents = std::vector<T>();
        }

        T at(int n) {
            if (n - 1 < contents.size() && n > 0) return contents.at(n - 1);
            return T();
        }
        void append(T value) {
            if (contents.size() < 200000) contents.push_back(value);
        }
        void remove(int n) {
            if (n - 1 < contents.size() && n > 0) contents.erase(contents.begin() + n - 1);
        }
        void clear() {
            contents.clear();
        }
        void insert_at(int n, T value) {
            contents.insert(contents.begin() + n - 1, value);
        }
        void replace(int n, T value) {
            if (n - 1 < contents.size() && n > 0) contents[n - 1] = value;
        }
        int index_of(T value) {
            for (int i = 0; i < contents.size(); i++) {
                if (contents[i] == value) {
                    return i + 1;
                }
            }
            return 0;
        }
        int length() {
            return contents.size();
        }
        int size() {
            return contents.size();
        }
        bool contains(T value) {
            return index_of(value) != 0;
        }

        operator std::string() const {
            std::string out;
            for (auto e : contents) {
                out += (std::string) e;
                out += " ";
            }

            return out;
        }

        operator ts::String() const {
            std::string out;
            for (auto e : contents) {
                out += (std::string) e;
                out += " ";
            }

            return ts::String(out);
        }

        operator double() const {
            return std::numeric_limits<double>::quiet_NaN();
        }
    };
};
