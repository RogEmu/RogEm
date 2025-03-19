/*
** EPITECH PROJECT, 2025
** rogem
** File description:
** Window
*/

#include "Window.hpp"

#include <fmt/format.h>

Window::Window(int x, int y, int w, int h) :
    Widget(x, y, w, h)
{
}

Window::~Window()
{
}

void Window::setMode(WindowMode mode)
{
    if (mode != m_mode)
    {
        m_mode = mode;
        // Check if window needs resize and notify parent
    }
}

void Window::setTitle(const std::string &title)
{
    if (title != m_title)
    {
        m_title = title;
    }
}

std::string Window::getTitle(void) const
{
    return m_title;
}

void Window::draw(WINDOW *window)
{
    drawBorder(window);
    wattron(window, A_BOLD | COLOR_PAIR(1));
    mvwprintw(window, m_pos.y, m_pos.x + 2, fmt::format(" {} ", m_title).c_str());
    wattroff(window, A_BOLD | COLOR_PAIR(1));
}
