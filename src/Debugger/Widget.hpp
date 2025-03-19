/*
** EPITECH PROJECT, 2025
** rogem
** File description:
** Widget
*/

#ifndef WIDGET_HPP_
#define WIDGET_HPP_

#include <ncurses.h>
#include <memory>
#include <list>

struct vec2
{
    int x;
    int y;
};

class Widget
{
    public:
        Widget(int x, int y, int w, int h);
        ~Widget();

        vec2 size() const;
        vec2 position() const;

        virtual void resize(int w, int h);
        void move(int x, int y);

        void setColorPair(int pair);
        int coloPair() const;

        virtual void draw(WINDOW *window);

        void setParent(const std::shared_ptr<Widget> &parent);

    protected:
        void drawBorder(WINDOW *window);

    protected:
        vec2 m_pos;
        vec2 m_size;
        int m_colorPair;

        std::shared_ptr<Widget> m_parent;
};

#endif /* !WIDGET_HPP_ */
