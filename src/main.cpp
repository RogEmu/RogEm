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

    while (true)
    {
        cpu.step();
        insWin.addInstruction(cpu.m_pc, Instruction{bus.loadWord(cpu.m_pc)});
        insWin.draw();
        regWin.draw();
        // draw_registers(cpu.m_registers, cpu.m_hi, cpu.m_lo, cpu.m_pc);
        // draw_instructions(cpu.m_pc, {});
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    cleanup_ncurses();
    return 0;
}
