#include <iostream>
#include <fstream>
#include "types/List.h"

namespace ts
{
    extern std::ifstream listfile;

    int init_listloader();

    template <typename T>
    void load_list(ts::List<T> &list)
    {
        list.clear();

        std::string listlen;
        std::getline(listfile, listlen, '\x00');
        int len = std::stoi(listlen);

        for (int i = 0; i < len; i++)
        {
            char c;
            listfile.read(&c, 1);
            std::string current;
            std::getline(listfile, current, '\x00');

            if (c == 'i')
            {
                list.append(T(std::stoi(current)));
            }
            if (c == 'f')
            {
                list.append(T(std::stof(current)));
            }
            if (c == 's')
            {
                list.append(T(current));
            }
        }
    }
};
