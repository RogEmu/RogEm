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
    scrollok(m_window, true);
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

void InstructionsWindow::setBus(Bus *bus)
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

        if (addr == (*m_pc)) wattron(m_window, A_BOLD | A_REVERSE);
        mvwprintw(m_window, row, 2, line.c_str());
        if (addr == (*m_pc)) wattroff(m_window, A_BOLD | A_REVERSE);
    }
}
