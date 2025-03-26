#ifndef DEBUGGER_HPP_
#define DEBUGGER_HPP_

#include <list>

#include "RegisterWindow.hpp"
#include "MemoryWindow.hpp"
#include "InstructionWindow.hpp"

struct CPU;

class Debugger
{
    public:
        Debugger(const std::shared_ptr<CPU> &cpu);
        ~Debugger();

        void update();

        bool isPaused() const;
        float getSimSpeed() const { return m_simSpeed; }

    private:
        std::shared_ptr<CPU> m_cpu;
        std::list<std::shared_ptr<IWindow>> m_windows;

        bool m_systemPaused;
        float m_simSpeed;
};

#endif /* !DEBUGGER_HPP_ */
