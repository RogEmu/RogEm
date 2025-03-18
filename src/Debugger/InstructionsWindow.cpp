/*
** EPITECH PROJECT, 2025
** rogem
** File description:
** InstructionsWindow
*/

#include "InstructionsWindow.hpp"
#include "Disassembler.h"
#include "Bus.h"

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

void InstructionsWindow::drawElements() const
{
    Window::drawElements();

    const int num_lines = m_size.y - 2; // Leave space for borders
    const int half_window = num_lines / 2;

    // Show instructions before and after the current PC
    for (int i = -half_window; i < half_window; i++) {
        uint32_t addr = (*m_pc) + (i * 4);
        Instruction instr = static_cast<Instruction>(m_bus->loadWord(addr));
        std::string line = Disassembler::disassemble(addr, instr);

        int row = (i + half_window) + 1; // Adjust row position

        // if (line.length() < static_cast<size_t>(m_size.x))
        //     line.append(static_cast<size_t>(m_size.x) - line.length() - 4, ' ');
        if ((addr / 4 % 2 == 0)) wattron(m_window, COLOR_PAIR(2)); else wattron(m_window, COLOR_PAIR(3));
        if (addr == (*m_pc)) wattron(m_window, A_BOLD | A_STANDOUT | COLOR_PAIR(1));
        mvwprintw(m_window, row, 2, line.c_str());
        if ((addr / 4 % 2 == 0)) wattroff(m_window, COLOR_PAIR(2)); else wattroff(m_window, COLOR_PAIR(3));
        if (addr == (*m_pc)) wattroff(m_window, A_BOLD | A_STANDOUT | COLOR_PAIR(1));
    }
}
