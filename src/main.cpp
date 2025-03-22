/*
** EPITECH PROJECT, 2024
** RogEm
** File description:
** main
*/

#include <iostream>
#include <thread>

#include "BIOS.h"
#include "CPU.h"
#include "RAM.h"
#include "Debugger.hpp"

int main(int ac, char **av)
{
    if (ac != 2)
    {
        std::cout << "Missing BIOS filepath" << std::endl;
        return -1;
    }
    auto bios = std::make_shared<BIOS>(av[1]);
    auto ram = std::make_shared<RAM>();
    auto bus = std::make_shared<Bus>(bios, ram);
    auto cpu = std::make_shared<CPU>(bus);
    auto dbg = std::make_shared<Debugger>(cpu);

    while (dbg->isRunning())
    {
        cpu->step();
        dbg->update();
    }
    return 0;
}
