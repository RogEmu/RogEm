/*
** EPITECH PROJECT, 2025
** rogem
** File description:
** RegistersWindow
*/

#include "RegistersWindow.hpp"

#include <fmt/format.h>

RegistersWindow::RegistersWindow(int x, int y, int w, int h) :
    Window(x, y, w, h),
    m_gpr(nullptr)
{
}

RegistersWindow::~RegistersWindow()
{
}

void RegistersWindow::setGPR(uint32_t *const gpr)
{
    m_gpr = gpr;
}

void RegistersWindow::setSpecialRegisters(uint32_t *const pc, uint32_t *const hi, uint32_t *const lo)
{
    m_pc = pc;
    m_hi = hi;
    m_lo = lo;
}

void RegistersWindow::drawElements() const
{
    Window::drawElements();
    int i = 0;

    if (m_gpr != nullptr)
    {
        for (; i < 32; i++) {
            std::string line = fmt::format("${:<4}: 0x{:08X}", "r" + std::to_string(i), m_gpr[i]);
            mvwprintw(m_window, i + 1, 2, line.c_str());
        }
    }
    if (m_pc)
        mvwprintw(m_window, i + 2, 2, fmt::format("PC:  0x{:08X}", *m_pc).c_str());
    if (m_hi)
        mvwprintw(m_window, i + 3, 2, fmt::format("HI:  0x{:08X}", *m_hi).c_str());
    if (m_lo)
        mvwprintw(m_window, i + 4, 2, fmt::format("LO:  0x{:08X}", *m_lo).c_str());
}
