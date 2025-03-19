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

    auto hLayout = std::make_shared<HBoxLayout>();
    hLayout->resize(COLS, LINES);

    auto vLayout = std::make_shared<VBoxLayout>();
    vLayout->resize(COLS, LINES);
    vLayout->addWidget(hLayout);

    auto widget1 = std::make_shared<Widget>(0, 0, 5, 5);
    widget1->setColorPair(20);
    hLayout->addWidget(widget1);

    auto widget2 = std::make_shared<Widget>(0, 0, 5, 5);
    widget2->setColorPair(21);
    hLayout->addWidget(widget2);

    auto widget3 = std::make_shared<Widget>(0, 0, 5, 5);
    widget3->setColorPair(22);
    hLayout->addWidget(widget3);

    auto widget4 = std::make_shared<Widget>(0, 0, 5, 5);
    widget4->setColorPair(20);
    vLayout->addWidget(widget4);

    auto widget5 = std::make_shared<Widget>(0, 0, 5, 5);
    widget5->setColorPair(21);
    vLayout->addWidget(widget5);

    m_rootWidget = vLayout;

    // auto label = std::make_shared<Label>(0, 0, COLS, 1);
    // label->setText("PSX DEBUGGER v0.0.1");
    // label->setSpan(3);
    // m_layout[0]->addWidget(label);

    // auto instructions = std::make_shared<InstructionsWindow>(0, 1, 55, 38);
    // instructions->setTitle("Instructions");
    // instructions->setBus(m_cpu->m_bus);
    // instructions->setPc(&m_cpu->m_pc);
    // instructions->setSpan(2);
    // m_layout[1]->addWidget(instructions);

    // auto registers = std::make_shared<RegistersWindow>(0, 1, 21, 38);
    // registers->setTitle("Registers");
    // registers->setGPR(cpu->m_registers);
    // registers->setSpecialRegisters(&m_cpu->m_pc, &m_cpu->m_hi, &m_cpu->m_lo);
    // registers->setSpan(1);
    // m_layout[1]->addWidget(registers);
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
