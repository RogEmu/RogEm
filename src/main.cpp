/*
** EPITECH PROJECT, 2024
** RogEm
** File description:
** main
*/

#include <iostream>
#include "BIOS.h"
#include "CPU.h"
#include "Debugger.hpp"
#include <ncurses.h>
#include <thread>

#define COLOR_BRIGHT_WHITE 15
#define COLOR_BRIGHT_BLACK 8

// void beginCurses()
// {
//     initscr();             // Start ncurses mode
//     cbreak();              // Disable line buffering
//     noecho();              // Don't echo user input
//     keypad(stdscr, TRUE);  // Enable arrow keys
//     curs_set(0);           // Hide the cursor
//     nodelay(stdscr, true);
//     start_color();

//     init_pair(1, COLOR_YELLOW, COLOR_BLACK);
//     init_pair(3, COLOR_WHITE, COLOR_BLACK);
//     init_pair(2, COLOR_BRIGHT_WHITE, COLOR_BRIGHT_BLACK);

//     refresh();
// }

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
    Debugger dbg(&cpu);

    while (true)
    {
        cpu.step();
        dbg.update();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    return 0;
}
