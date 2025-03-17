/*
** EPITECH PROJECT, 2024
** RogEm
** File description:
** main
*/

#include <iostream>
#include "BIOS.h"
#include "CPU.h"

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
    nodelay(stdscr, true);
    refresh();
    start_color();

    init_pair(1, 220, COLOR_BLACK);
    init_pair(2, COLOR_WHITE, 236);
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
    BIOS bios(av[1]);
    Bus bus(bios);
    CPU cpu(bus);

    init_ncurses();

    InstructionsWindow insWin(1, 1, 60, 38);
    RegistersWindow regWin(61, 1, 21, 38);

    insWin.setTitle("Instructions");
    regWin.setTitle("Registers");

    regWin.setGPR(cpu.m_registers);
    regWin.setSpecialRegisters(&cpu.m_pc, &cpu.m_hi, &cpu.m_lo);

    insWin.setPc(&cpu.m_pc);
    insWin.setBus(&bus);

    int ch = 0;
    bool pause = false;

    while (true)
    {
        ch = getch();

        fprintf(stderr, "%c", ch);

        if (ch == 'p')
        {
            pause = !pause;
        }

        if (pause)
        {
            if (ch != ' ')
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                continue;
            }
        }

        cpu.step();
        insWin.addInstruction(cpu.m_pc, Instruction{bus.loadWord(cpu.m_pc)});
        insWin.draw();
        regWin.draw();
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
    cleanup_ncurses();
    return 0;
}
