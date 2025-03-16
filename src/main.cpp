/*
** EPITECH PROJECT, 2024
** RogEm
** File description:
** main
*/

#include <iostream>
#include "BIOS.h"
#include "CPU.h"
#include "RAM.h"
#include "Debugger.hpp"

#include "Debugger/Window.hpp"
#include "Debugger/RegistersWindow.hpp"
#include "Debugger/InstructionsWindow.hpp"

#include <ncurses.h>
#include <thread>

void init_ncurses()
{
    initscr();             // Start ncurses mode
    cbreak();              // Disable line buffering
    noecho();              // Don't echo user input
    keypad(stdscr, TRUE);  // Enable arrow keys
    curs_set(0);           // Hide the cursor
    refresh();
    start_color();

    init_pair(1, COLOR_RED, COLOR_BLACK);
}

void cleanup_ncurses()
{
    endwin();
}

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

    init_ncurses();

    InstructionsWindow insWin(1, 1, 60, 38);
    RegistersWindow regWin(61, 1, 21, 38);

    insWin.setTitle("Instructions");
    regWin.setTitle("Registers");

    regWin.setGPR(cpu.m_registers);
    regWin.setSpecialRegisters(&cpu.m_pc, &cpu.m_hi, &cpu.m_lo);

    insWin.setPc(&cpu.m_pc);
    insWin.setBus(&bus);

    while (true)
    {
        // getchar();
        cpu->step();
        dbg->update();
    }
    cleanup_ncurses();
    return 0;
}
