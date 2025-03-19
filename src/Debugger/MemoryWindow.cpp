/*
** EPITECH PROJECT, 2025
** rogem
** File description:
** MemoryWindow
*/

#include "MemoryWindow.hpp"

#include <fmt/format.h>
#include "Bus.h"

MemoryWindow::MemoryWindow() :
    Window(0, 0, 0, 0),
    m_startAddress(0)
{
}

MemoryWindow::~MemoryWindow()
{
}

void MemoryWindow::setBus(const std::shared_ptr<Bus> &bus)
{
    m_bus = bus;
}

void MemoryWindow::draw(WINDOW *window)
{
    Window::draw(window);

    int startX = m_pos.x + 1;
    int startY = m_pos.y + 1;
    int nbLines = m_size.y - 2;
    uint32_t startAddr = m_startAddress;

    wattron(window, COLOR_PAIR(1) | A_BOLD);
    mvwaddstr(window, startY, startX, "            00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F");
    wattroff(window, COLOR_PAIR(1) | A_BOLD);
    startY++;
    for (int i = 0; i < nbLines - 1; i++)
    {
        drawMemoryLine(window, startX, startY + i, startAddr);
        startAddr += 16;
    }
}

void MemoryWindow::addressUp()
{
    m_startAddress = std::min(UINT32_MAX, m_startAddress + 64);
}

void MemoryWindow::addressDown()
{
    m_startAddress = std::max((uint32_t)0, m_startAddress - 64);
}

void MemoryWindow::drawMemoryLine(WINDOW *window, int x, int y, uint32_t addr)
{
    wattron(window, COLOR_PAIR(1) | A_BOLD);
    mvwaddstr(window, y, x, fmt::format("0x{:08X}: ", addr).c_str());
    wattroff(window, COLOR_PAIR(1) | A_BOLD);

    wattron(window, (addr / 16 % 2) == 0 ? COLOR_PAIR(3) : COLOR_PAIR(2));
    for (int i = 0; i < 16; i++)
    {
        uint8_t byte = m_bus->loadByte(addr + i);
        waddstr(window, fmt::format("{:02X} ", byte).c_str());
    }
    wattroff(window, (addr / 16 % 2) == 0 ? COLOR_PAIR(3) : COLOR_PAIR(2));
}
