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

void RegistersWindow::draw(WINDOW *window)
{
    Window::draw(window);

    int posX = m_pos.x + 2;
    int posY = m_pos.y + 1;
    int colWidth = 18;

    if (m_gpr != nullptr)
    {
        for (int i = 0; i < 32; i++)
        {
            std::string line = fmt::format("{:>4}: 0x{:08X}", "$r" + std::to_string(i), m_gpr[i]);
            mvwaddstr(window, posY, posX, line.c_str());
            posY++;

            if (posY >= m_size.y - 1)
            {
                posY = m_pos.y + 1;
                posX += colWidth;
            }
        }
    }
    if (m_pc)
        mvwaddstr(window, posY, posX, fmt::format("{:>4}: 0x{:08X}", "PC", *m_pc).c_str());
    if (m_hi)
        mvwaddstr(window, posY + 1, posX, fmt::format("{:>4}: 0x{:08X}", "HI", *m_hi).c_str());
    if (m_lo)
        mvwaddstr(window, posY + 2, posX, fmt::format("{:>4}: 0x{:08X}", "LO", *m_lo).c_str());
}
