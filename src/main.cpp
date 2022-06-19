#include "lib.hpp"
#include <iostream>

int main(int argc, char *argv[])
{
    if (argc > 1)
    {
        find_path(argv[1]);
    }
    else
    {
        std::cout << "Not enough arguments given" << '\n';
    }
    return 0;
}