/*
** EPITECH PROJECT, 2025
** rogem
** File description:
** InstructionsWindow
*/

#include "InstructionsWindow.hpp"
#include "Disassembler.h"
#include "Bus.h"

#include <algorithm>

InstructionsWindow::InstructionsWindow(int x, int y, int w, int h) :
    Window(x, y, w, h)
{
}

InstructionsWindow::~InstructionsWindow()
{
}

void InstructionsWindow::setPc(uint32_t *const pc)
{
    m_pc = pc;
}

void InstructionsWindow::addInstruction(uint32_t pc, const Instruction &i)
{
    m_instructionMap[pc] = i;
}

void InstructionsWindow::setBus(const std::shared_ptr<Bus> &bus)
{
    m_bus = bus;
}

void InstructionsWindow::draw(WINDOW *window)
{
    Window::draw(window);

    const int num_lines = m_size.y - 1; // Leave space for borders
    const int half_window = num_lines / 2;
    const int maxWidth = std::max(m_size.x - 3, 0);

    // Show instructions before and after the current PC
    for (int i = -half_window; i < half_window; i++)
    {
        uint32_t addr = (*m_pc) + (i * 4);
        Instruction instr = static_cast<Instruction>(m_bus->loadWord(addr));
        std::string line = Disassembler::disassemble(addr, instr);

        int row = m_pos.y + (i + half_window) + 1; // Adjust row position

        line.resize(maxWidth, ' ');

        if ((addr / 4 % 2 == 0)) wattron(window, COLOR_PAIR(2)); else wattron(window, COLOR_PAIR(3));
        if (addr == (*m_pc))
        {
            wattron(window, A_BOLD | A_STANDOUT | COLOR_PAIR(1));
            mvwaddch(window, row, m_pos.x + 1, '>');
        }
        mvwaddnstr(window, row, m_pos.x + 2, line.c_str(), maxWidth);
        if ((addr / 4 % 2 == 0)) wattroff(window, COLOR_PAIR(2)); else wattroff(window, COLOR_PAIR(3));
        if (addr == (*m_pc)) wattroff(window, A_BOLD | A_STANDOUT | COLOR_PAIR(1));
    }
}
