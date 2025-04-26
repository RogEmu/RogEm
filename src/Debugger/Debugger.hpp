#ifndef DEBUGGER_HPP_
#define DEBUGGER_HPP_

#include <list>
#include <memory>
#include <cstdint>
#include <vector>
#include <fstream>
#include <nlohmann/json.hpp>
#include "Disassembler.h"
#include "CPU.h"

class IWindow;
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
    bool isRunTo;
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
        uint32_t getCpuReg(CpuReg reg) const;
        void setCpuReg(CpuReg reg, uint32_t value);
        uint32_t getCop0Reg(uint8_t reg) const;
        void setCop0Reg(uint8_t reg, uint32_t val);
        void CPUReset();

        // Memory Controls
        uint8_t readByte(uint32_t addr) const;
        uint16_t readHalfWord(uint32_t addr) const;
        uint32_t readWord(uint32_t addr) const;

        // Get window
        std::list<std::shared_ptr<IWindow>> &getWindows() { return m_windows; }

        // Breakpoints
        void addBreakpoint(uint32_t addr, BreakpointType type, const std::string &label, bool isRunTo);
        void removeBreakpoint(long index);
        void toggleBreakpoint(long index, bool enable);
        bool isBreakpointEnabled(long index);
        long getBreakpointIndex(uint32_t addr);
        void saveBreakpointsToFile();
        void loadBreakpointsFromFile();
        std::vector<Breakpoint> &getBreakpoints();
        void setResumeOnBreakpoint(bool resume);
        void setBreakpoint(uint32_t addr, BreakpointType type, const std::string &label, bool enabled, bool isRunTo);

        std::vector<uint8_t> *memoryRange(uint32_t addr);

    private:
        System *m_system;

        std::list<std::shared_ptr<IWindow>> m_windows;
        uint32_t m_currentMemAddr;

        std::vector<Breakpoint> m_breakpoints;
        std::string breakpointsFilePath = "breakpoints.json";

        bool m_paused;
        float m_simSpeed;
        bool m_resumeOnBreakpoint;
};

#endif /* !DEBUGGER_HPP_ */
