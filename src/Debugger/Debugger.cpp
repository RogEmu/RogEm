/*
** EPITECH PROJECT, 2025
** rogem
** File description:
** Debugger
*/

#include "Debugger.hpp"
#include <ncurses.h>
#include <fmt/format.h>
#include <memory>

#include "CPU.h"
#include "HBoxLayout.hpp"
#include "VBoxLayout.hpp"
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

    auto vLayout = std::make_shared<VBoxLayout>();
    auto hLayout = std::make_shared<HBoxLayout>();

    vLayout->setId("VLAYOUT");
    hLayout->setId("HLAYOUT");

    m_rootWidget = vLayout;

    auto ramWindow = std::make_shared<Window>(0, 0, 0, 0); // GREEN
    ramWindow->setTitle("Memory Viewer");

    auto registers = std::make_shared<RegistersWindow>(0, 0, 0, 0);
    registers->setTitle("Registers");
    registers->setGPR(cpu->m_registers);
    registers->setSpecialRegisters(&m_cpu->m_pc, &m_cpu->m_hi, &m_cpu->m_lo);

    auto instructionWindow = std::make_shared<InstructionsWindow>(0, 0, 0, 0); // RED
    instructionWindow->setTitle("Instructions");
    instructionWindow->setBus(m_cpu->m_bus);
    instructionWindow->setPc(&m_cpu->m_pc);

    vLayout->addWidget(hLayout);
    vLayout->addWidget(ramWindow);
    hLayout->addWidget(instructionWindow);
    hLayout->addWidget(registers);

    // auto label = std::make_shared<Label>(0, 0, COLS, 1);
    // label->setText("PSX DEBUGGER v0.0.1");
    // label->setSpan(3);
    // m_layout[0]->addWidget(label);
}

Debugger::~Debugger()
{
    endCurses();
}

void Debugger::addWidget(const std::shared_ptr<Widget> &window)
{
    (void)window;
    // m_widgets.push_back(window);
}

void Debugger::update(void)
{
    int c = getch();

    if (c == 'p')
        m_paused = !m_paused;
    if (c == KEY_RESIZE)
    {
        m_rootWidget->resize(COLS, LINES);
    }
    drawWindows();
}

bool Debugger::isPaused() const
{
    return m_paused;
}

void Debugger::drawWindows(void)
{
    werase(stdscr);
    m_rootWidget->draw(stdscr);
    wrefresh(stdscr);
}

void Debugger::initColors()
{
    init_pair(1, COLOR_YELLOW, COLOR_BLACK);
    init_pair(3, COLOR_WHITE, COLOR_BLACK);
    init_pair(2, COLOR_BRIGHT_WHITE, COLOR_BRIGHT_BLACK);

    init_pair(20, COLOR_WHITE, COLOR_GREEN);
    init_pair(21, COLOR_WHITE, COLOR_BLUE);
    init_pair(22, COLOR_WHITE, COLOR_RED);
    init_pair(23, COLOR_WHITE, COLOR_YELLOW);
    init_pair(24, COLOR_WHITE, COLOR_MAGENTA);
    init_pair(25, COLOR_WHITE, COLOR_CYAN);
}

void Debugger::beginCurses()
{
    initscr();             // Start ncurses mode
    cbreak();              // Disable line buffering
    noecho();              // Don't echo user input
    keypad(stdscr, TRUE);  // Enable arrow keys
    curs_set(0);           // Hide the cursor
    nodelay(stdscr, true);
    start_color();
    initColors();

    refresh();
}

void Debugger::endCurses()
{
    endwin();
}
