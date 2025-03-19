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

#include "Widget.hpp"

enum class WindowMode
{
    DYNAMIC,
    FIXED
};

class Window : public Widget
{
    public:
        Window(int x, int y, int w, int h);
        ~Window();

        void setMode(WindowMode mode);

        void setTitle(const std::string &title);
        std::string getTitle(void) const;

        virtual void draw(WINDOW *window) override;

    protected:
        std::string m_title;
        WindowMode m_mode;
};

#endif /* !WINDOW_HPP_ */
