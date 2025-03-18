/*
** EPITECH PROJECT, 2025
** rogem
** File description:
** Debugger
*/

#include "Debugger.hpp"
#include <ncurses.h>

#include "CPU.h"
#include "Window.hpp"
#include "RegistersWindow.hpp"
#include "InstructionsWindow.hpp"

#define COLOR_BRIGHT_BLACK 8
#define COLOR_BRIGHT_RED 9
#define COLOR_BRIGHT_GREEN 10
#define COLOR_BRIGHT_YELLOW 11
#define COLOR_BRIGHT_BLUE 12
#define COLOR_BRIGHT_MAGENTA 13
#define COLOR_BRIGHT_CYAN 14
#define COLOR_BRIGHT_WHITE 15

Debugger::Debugger(const std::shared_ptr<CPU> &cpu) :
    m_cpu(cpu)
{
    beginCurses();

    auto instructions = std::make_shared<InstructionsWindow>(0, 0, 55, 38);
    instructions->setTitle("Instructions");
    instructions->setBus(m_cpu->m_bus);
    instructions->setPc(&m_cpu->m_pc);
    addWindow(instructions);

    auto registers = std::make_shared<RegistersWindow>(0, 0, 21, 38);
    registers->setTitle("Registers");
    registers->setGPR(cpu->m_registers);
    registers->setSpecialRegisters(&m_cpu->m_pc, &m_cpu->m_hi, &m_cpu->m_lo);
    addWindow(registers);
}

Debugger::~Debugger()
{
    endCurses();
}

void Debugger::addWindow(const std::shared_ptr<Window> &window)
{
    m_windows.push_back(window);
}

void Debugger::update(void)
{
    int c = getch();

    if (c == 'p')
        m_paused = !m_paused;

    drawWindows();
}

bool Debugger::isPaused() const
{
    return m_paused;
}

void Debugger::drawWindows(void)
{
    int newtWinX = 0;

    for (auto &window : m_windows)
    {
        window->move(newtWinX, 2);
        window->draw();
        newtWinX += window->size().x + 2;
    }
}

void Debugger::beginCurses() const
{
    initscr();             // Start ncurses mode
    cbreak();              // Disable line buffering
    noecho();              // Don't echo user input
    keypad(stdscr, TRUE);  // Enable arrow keys
    curs_set(0);           // Hide the cursor
    nodelay(stdscr, true);
    start_color();

    init_pair(1, COLOR_YELLOW, COLOR_BLACK);
    init_pair(3, COLOR_WHITE, COLOR_BLACK);
    init_pair(2, COLOR_BRIGHT_WHITE, COLOR_BRIGHT_BLACK);

    refresh();
}

void Debugger::endCurses() const
{
    endwin();
}
