/*
** EPITECH PROJECT, 2025
** rogem
** File description:
** Window
*/

#ifndef WINDOW_HPP_
#define WINDOW_HPP_

#include <ncurses.h>
#include <string>

struct Vec2i
{
    int x;
    int y;
};

class Window {
    public:
        Window(int x, int y, int w, int h);
        ~Window();

        Vec2i size(void) const;

        void resize(int x, int y);
        void move(int x, int y);
        void draw(void);

        void setTitle(const std::string &title);
        std::string getTitle(void) const;

    protected:
        virtual void drawElements() const;

    protected:
        Vec2i m_pos;
        Vec2i m_size;
        std::string m_title;
        WINDOW *m_window;
};

#endif /* !WINDOW_HPP_ */
