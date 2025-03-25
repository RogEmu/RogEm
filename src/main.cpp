/*
** EPITECH PROJECT, 2024
** RogEm
** File description:
** main
*/

#include <iostream>
#include <thread>

#include "Debugger.hpp"
#include "System.hpp"

int main(int ac, char **av)
{
    if (ac != 2)
    {
        std::cout << "Missing BIOS filepath" << std::endl;
        return -1;
    }
    auto bios = std::make_shared<BIOS>(av[1]);
    auto system = std::make_shared<System>(bios);

    while (system->isRunning())
    {
        system->run();
    }
    return 0;
}
