/*
** EPITECH PROJECT, 2025
** rogem
** File description:
** Debugger
*/

#include "Debugger.hpp"

#include <algorithm>

#include "System.hpp"

Debugger::Debugger(System *system) :
    m_system(system)
{
    loadBreakpointsFromFile();
}

Debugger::~Debugger()
{
}

bool Debugger::isPaused() const
{
    return m_system->getState() == SystemState::PAUSED;
}

void Debugger::stepOver()
{
    m_system->getCPU()->step();
}

uint32_t Debugger::getCpuReg(CpuReg reg) const
{
    return m_system->getCPU()->getReg(reg);
}

void Debugger::setCpuReg(CpuReg reg, uint32_t value)
{
    m_system->getCPU()->setReg(reg, value);
}

uint32_t Debugger::getCop0Reg(uint8_t reg) const
{
    return m_system->getCPU()->getCop0Reg(reg);
}

void Debugger::setCop0Reg(uint8_t reg, uint32_t val)
{
    m_system->getCPU()->setCop0Reg(reg, val);
}

void Debugger::CPUReset()
{
    m_system->getCPU()->reset();
}

uint8_t Debugger::readByte(uint32_t addr) const
{
    return m_system->getBus()->loadByte(addr);
}

uint16_t Debugger::readHalfWord(uint32_t addr) const
{
    return m_system->getBus()->loadHalfWord(addr);
}

uint32_t Debugger::readWord(uint32_t addr) const
{
    return m_system->getBus()->loadWord(addr);
}

std::vector<uint8_t> *Debugger::memoryRange(uint32_t addr)
{
    auto slice = m_system->getBus()->getMemoryRange(addr);
    return slice;
}

Disassembler &Debugger::getDisassembler()
{
    return m_disassembler;
}

void Debugger::loadBios(const char *path)
{
    m_system->loadBios(path);
    CPUReset();
}

void Debugger::loadExecutable(const char *path)
{
    m_system->loadExecutable(path);
    CPUReset();
}

//The following functions are required for nlohmann-json to work, but are called automatically upon conversion
void to_json(nlohmann::json& j, const Breakpoint& b)
{
    j = nlohmann::json {
        {"addr", b.addr},
        {"instructionType", static_cast<int>(b.instructionType)},
        {"label", b.label},
        {"enabled", b.enabled}
    };
}

void from_json(const nlohmann::json& j, Breakpoint& b)
{
    b.addr = j.at("addr").get<uint32_t>();
    b.instructionType = static_cast<BreakpointType>(j.at("instructionType").get<int>());
    b.label = j.at("label").get<std::string>();
    b.enabled = j.at("enabled").get<bool>();
}

void Debugger::addBreakpoint(uint32_t addr, BreakpointType type, const std::string &label, bool isRunTo)
{
    m_breakpoints.push_back({addr, type, label, true, isRunTo});
    saveBreakpointsToFile();
}

long Debugger::getBreakpointIndex(uint32_t addr)
{
    for (int i = 0; i < static_cast<int>(m_breakpoints.size()); i++) {
        if (m_breakpoints[i].addr == addr) {
            return i;
        }
    }
    return -1;
}

bool Debugger::isBreakpointEnabled(long index)
{
    if (index < 0 || index >= static_cast<long>(m_breakpoints.size())) {
        return false;
    }
    return m_breakpoints[index].enabled;
}

void Debugger::toggleBreakpoint(long index, bool enable)
{
    if (index < 0 || index >= static_cast<long>(m_breakpoints.size())) {
        return;
    }
    m_breakpoints[index].enabled = enable;
}

void Debugger::removeBreakpoint(long index)
{
    if (index < 0 || index >= static_cast<long>(m_breakpoints.size())) {
        return;
    }
    m_breakpoints.erase(m_breakpoints.begin() + index);
    saveBreakpointsToFile();
}

void Debugger::saveBreakpointsToFile()
{
    std::ofstream file(breakpointsFilePath);
    if (file.is_open()) {
        nlohmann::json j = m_breakpoints;
        file << j.dump(4);
    }
}

void Debugger::loadBreakpointsFromFile()
{
    std::ifstream file(breakpointsFilePath);
    if (file.is_open()) {
        nlohmann::json j;
        file >> j;
        m_breakpoints = j.get<std::vector<Breakpoint>>();
    }
}

std::vector<Breakpoint> &Debugger::getBreakpoints()
{
    return m_breakpoints;
}


void Debugger::update()
{
    uint32_t pc = getCpuReg(CpuReg::PC);

    for (auto bp : m_breakpoints) {
        if (!bp.enabled) {
            continue;
        }
        if (bp.addr == pc) {
            if (bp.instructionType == BreakpointType::EXEC) {
                m_system->setState(SystemState::PAUSED);
                if (bp.isRunTo == true) {
                    removeBreakpoint(getBreakpointIndex(bp.addr));
                }
            }
        }
    }
}

void Debugger::pause(bool pause)
{
    m_system->setState(pause ? SystemState::PAUSED : SystemState::RUNNING);
    m_system->tick();
}
