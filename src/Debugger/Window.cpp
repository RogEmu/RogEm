/*
** EPITECH PROJECT, 2025
** rogem
** File description:
** Window
*/

#include "Window.hpp"

#include <fmt/format.h>

Window::Window(int x, int y, int w, int h) :
    m_pos({x, y}),
    m_size({w, h})
{
    m_window = newwin(h, w, y, x);
    box(m_window, 0, 0);
}

Window::~Window()
{
    delwin(m_window);
}

void Window::resize(int x, int y)
{
    m_size.x = x;
    m_size.y = y;
}

void Window::move(int x, int y)
{
    m_pos.x = x;
    m_pos.y = y;
}

void Window::draw(void)
{
    werase(m_window);
    wresize(m_window, m_size.y, m_size.x);
    box(m_window, 0, 0);
    drawElements();
    wrefresh(m_window);
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

void Window::drawElements() const
{
    wattron(m_window, A_BOLD);
    wattron(m_window, COLOR_PAIR(1));
    mvwprintw(m_window, 0, 2, fmt::format(" {} ", m_title).c_str());
    wattroff(m_window, COLOR_PAIR(1));
    wattroff(m_window, A_BOLD);
}
