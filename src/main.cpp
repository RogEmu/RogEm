/*
** EPITECH PROJECT, 2024
** RogEm
** File description:
** main
*/

#include <iostream>
#include "BIOS.h"
#include "CPU.h"

int main(int ac, char **av)
{
    if (ac != 2)
    {
        std::cout << "Missing BIOS filepath" << std::endl;
        return -1;
    }
    BIOS bios(av[1]);
    Bus bus(bios);
    CPU cpu(bus);

    while (true)
    {
        // getchar();
        cpu.step();
    }
    return 0;
}
