#ifndef DEBUGGER_HPP_
#define DEBUGGER_HPP_

#include <list>
#include <memory>

class IWindow;
struct CPU;

class Debugger
{
    public:
        Debugger(const std::shared_ptr<CPU> &cpu);
        ~Debugger();

        void update();

        void pause(bool pause);
        bool isPaused() const;

        float getSimulationSpeed() const;
        void setSimulationSpeed(float speed);

    private:
        std::shared_ptr<CPU> m_cpu;
        std::list<std::shared_ptr<IWindow>> m_windows;

        bool m_paused;
        float m_simSpeed;
};

#endif /* !DEBUGGER_HPP_ */
