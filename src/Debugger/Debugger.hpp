/*
** EPITECH PROJECT, 2025
** rogem
** File description:
** Debugger
*/

#ifndef DEBUGGER_HPP_
#define DEBUGGER_HPP_

#include <list>
#include <memory>

#include "HBoxLayout.hpp"

class CPU;
class Widget;

class Debugger {
    public:
        Debugger(const std::shared_ptr<CPU> &cpu);
        ~Debugger();

        void addWidget(const std::shared_ptr<Widget> &window);
        void update();

        bool isPaused() const;

        private:
        void beginCurses();
        void endCurses();
        void drawWindows();
        void initColors();

    private:
        std::shared_ptr<CPU> m_cpu;
        std::shared_ptr<HBoxLayout> m_layout;

        bool m_paused;
};

#endif /* !DEBUGGER_HPP_ */
