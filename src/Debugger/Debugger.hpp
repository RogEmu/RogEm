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
class Window;

class Debugger {
    public:
        Debugger(CPU *cpu);
        ~Debugger();

        void addWindow(const std::shared_ptr<Window> &window);
        void update(void);

        bool isPaused() const;

        private:
        void beginCurses() const;
        void endCurses() const;
        void drawWindows(void);

    private:
        CPU *m_cpu;
        std::list<std::shared_ptr<Window>> m_windows;

        bool m_paused;
};

#endif /* !DEBUGGER_HPP_ */
