/*
** EPITECH PROJECT, 2025
** rogem
** File description:
** Widget
*/

#include "Widget.hpp"

Widget::Widget(int x, int y, int w, int h) :
    m_pos({x, y}),
    m_size({w, h})
{
}

Widget::~Widget()
{
}

vec2 Widget::size(void) const
{
    return m_size;
}

vec2 Widget::position(void) const
{
    return m_pos;
}

void Widget::resize(int w, int h)
{
    m_size.x = w;
    m_size.y = h;
}

void Widget::move(int x, int y)
{
    m_pos.x = x;
    m_pos.y = y;
}

void Widget::setColorPair(int pair)
{
    m_colorPair = pair;
}

int Widget::coloPair() const
{
    return m_colorPair;
}

void Widget::draw(WINDOW *window)
{
    vec2 parentPos = {0, 0};

    if (m_parent)
        parentPos = m_parent->position();

    wattron(window, COLOR_PAIR(m_colorPair));
    // Fill widget background
    for (int i = 0; i < m_size.y; i++) {
        for (int j = 0; j < m_size.x; j++) {
            mvwaddch(window, m_pos.y + i + parentPos.y, m_pos.x + j + parentPos.x, ' '); // Fill with spaces
        }
    }
    wattroff(window, COLOR_PAIR(m_colorPair));
}

void Widget::setParent(const std::shared_ptr<Widget> &parent)
{
    m_parent = parent;
}

void Widget::drawBorder(WINDOW *window)
{
    vec2 relPos = m_pos;

    if (m_parent)
    {
        relPos.x += m_parent->m_pos.x;
        relPos.y += m_parent->m_pos.y;
    }

    for (int i = 0; i < m_size.x; i++) {
        mvwaddch(window, relPos.y, relPos.x + i, '-');  // Top border
        mvwaddch(window, relPos.y + m_size.y - 1, relPos.x + i, '-');  // Bottom border
    }
    for (int i = 0; i < m_size.y; i++) {
        mvwaddch(window, relPos.y + i, relPos.x, '|');  // Left border
        mvwaddch(window, relPos.y + i, relPos.x + m_size.x - 1, '|');  // Right border
    }
    mvwaddch(window, relPos.y, relPos.x, '+');  // Corners
    mvwaddch(window, relPos.y, relPos.x + m_size.x - 1, '+');
    mvwaddch(window, relPos.y + m_size.y - 1, relPos.x, '+');
    mvwaddch(window, relPos.y + m_size.y - 1, relPos.x + m_size.x - 1, '+');
}
