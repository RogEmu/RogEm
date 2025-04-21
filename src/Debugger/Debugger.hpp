#ifndef DEBUGGER_HPP_
#define DEBUGGER_HPP_

#include <list>
#include <memory>
#include <cstdint>
#include <vector>
#include "Disassembler.h"

class IWindow;
struct CPU;
class System;

enum class BreakpointType
{
    EXEC,
    READ,
    WRITE
};

struct Breakpoint
{
    uint32_t addr;
    BreakpointType instructionType;
    std::string label;
    bool enabled;
};

class Debugger
{
    public:
        Debugger(System *system);
        ~Debugger();

        void update();
        void draw();

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

        // Memory Controls
        uint8_t readByte(uint32_t addr) const;
        uint16_t readHalfWord(uint32_t addr) const;
        uint32_t readWord(uint32_t addr) const;

        // Breakpoints
        void addBreakpoint(uint32_t addr, BreakpointType type, const std::string &label);
        void removeBreakpoint(long index);
        void toggleBreakpoint(long index, bool enable);
        bool isBreakpointEnabled(long index);
        long getBreakpointIndex(uint32_t addr);
        std::vector<Breakpoint> &getBreakpoints();

        std::vector<uint8_t> *memoryRange(uint32_t addr);

    private:
        System *m_system;

        std::list<std::shared_ptr<IWindow>> m_windows;

        std::vector<Breakpoint> m_breakpoints;

        bool m_paused;
        float m_simSpeed;
};

#endif /* !DEBUGGER_HPP_ */
