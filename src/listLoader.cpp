#include "listLoader.h"

namespace ts
{
    std::ifstream listfile;

    int init_listloader()
    {
        listfile.open("listdata");

        return 0;
    }
}