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

        // Debugger CPU Controls
        void stepOver();
        uint32_t getGPR(uint8_t reg) const;
        void setGPR(uint8_t reg, uint32_t value);
        uint32_t getSpecialReg(uint8_t reg) const;
        void setSpecialReg(uint8_t reg, uint32_t val);
        uint32_t getCop0Reg(uint8_t reg) const;
        void setCop0Reg(uint8_t reg, uint32_t val);

    private:
        System *m_system;

        std::list<std::shared_ptr<IWindow>> m_windows;

        bool m_paused;
        float m_simSpeed;
};

#endif /* !DEBUGGER_HPP_ */
