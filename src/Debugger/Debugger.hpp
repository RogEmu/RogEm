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

class CPU;
class Widget;
class MemoryWindow;

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
        std::shared_ptr<Widget> m_rootWidget;

        std::shared_ptr<MemoryWindow> m_memWindow;

        bool m_paused;
};

#endif /* !DEBUGGER_HPP_ */
