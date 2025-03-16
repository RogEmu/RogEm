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

struct Position
{
    int x;
    int y;
};

using Size = Position;

class Window {
    public:
        Window(int x, int y, int w, int h);
        ~Window();

        void resize(int x, int y);
        void move(int x, int y);
        void draw(void);

        void setTitle(const std::string &title);
        std::string getTitle(void) const;

    protected:
        virtual void drawElements() const;

    protected:
        Position m_pos;
        Size m_size;
        std::string m_title;
        WINDOW *m_window;
};

#endif /* !WINDOW_HPP_ */
