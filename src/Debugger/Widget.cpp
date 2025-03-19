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
    // fprintf(stderr, "New size: %d, %d color(%d)\n", m_size.x, m_size.y, m_colorPair);
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
    vec2 absPos = m_pos;

    wattron(window, COLOR_PAIR(m_colorPair));
    for (int i = 0; i < m_size.y; i++)
        for (int j = 0; j < m_size.x; j++)
            mvwaddch(window, absPos.y + i, absPos.x + j, ' ');
    wattroff(window, COLOR_PAIR(m_colorPair));
}

void Widget::setParent(Widget *parent)
{
    m_parent = parent;
}

void Widget::setId(const std::string &id)
{
    if (id != m_id)
        m_id = id;
}

std::string Widget::id() const
{
    return m_id;
}

void Widget::drawBorder(WINDOW *window)
{
    vec2 absPos = m_pos;

    for (int i = 0; i < m_size.x; i++) {
        mvwaddch(window, absPos.y, absPos.x + i, '-');  // Top border
        mvwaddch(window, absPos.y + m_size.y - 1, absPos.x + i, '-');  // Bottom border
    }
    for (int i = 0; i < m_size.y; i++) {
        mvwaddch(window, absPos.y + i, absPos.x, '|');  // Left border
        mvwaddch(window, absPos.y + i, absPos.x + m_size.x - 1, '|');  // Right border
    }
    mvwaddch(window, absPos.y, absPos.x, '+');  // Corners
    mvwaddch(window, absPos.y, absPos.x + m_size.x - 1, '+');
    mvwaddch(window, absPos.y + m_size.y - 1, absPos.x, '+');
    mvwaddch(window, absPos.y + m_size.y - 1, absPos.x + m_size.x - 1, '+');
}
