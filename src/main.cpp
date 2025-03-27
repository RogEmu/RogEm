/*
** EPITECH PROJECT, 2024
** RogEm
** File description:
** main
*/

#include <iostream>
#include <thread>

#include "System.hpp"

int main(int ac, char **av)
{
    if (ac != 2)
    {
        std::cout << "Missing BIOS filepath" << std::endl;
        return -1;
    }
    auto system = std::make_unique<System>(av[1]);

    system->run();
    return 0;
}
