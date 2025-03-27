#ifndef DEBUGGER_HPP_
#define DEBUGGER_HPP_

#include <list>
#include <memory>
#include <cstdint>

class IWindow;
struct CPU;
class System;

class Debugger
{
    public:
        Debugger(System *system);
        ~Debugger();

        void update();

        void pause(bool pause);
        bool isPaused() const;

        float getSimulationSpeed() const;
        void setSimulationSpeed(float speed);

        void stepOver();

        uint32_t getPc() const;

    private:
        System *m_system;

        std::list<std::shared_ptr<IWindow>> m_windows;

        bool m_paused;
        float m_simSpeed;
};

#endif /* !DEBUGGER_HPP_ */
